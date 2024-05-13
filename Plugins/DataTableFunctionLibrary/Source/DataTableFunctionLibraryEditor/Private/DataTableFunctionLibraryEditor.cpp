// Copyright 2022 Just2Devs. All Rights Reserved.

#include "DataTableFunctionLibraryEditor.h"
#include "DataTableEditorUtils.h"
#include "DataTableFunctionLibraryPinFactory.h"
#include "HttpModule.h"
#include "DataTableEditor/Public/DataTableEditorModule.h"
#include "DataTableEditor/Public/IDataTableEditor.h"
#include "Interfaces/IHttpResponse.h"
#include "Kismet/DataTableFunctionLibrary.h"
#include "Misc/FileHelper.h"
#include "EditorStyleSet.h"

#define LOCTEXT_NAMESPACE "FDataTableFunctionLibraryEditorModule"

void FDataTableFunctionLibraryEditorModule::StartupModule()
{
	// Bind to the engine post init 
	FCoreDelegates::OnPostEngineInit.AddRaw(this, &FDataTableFunctionLibraryEditorModule::OnPostEngineInit);

	// Default values for reimport data table feature
	ConfigFilePath = FPaths::Combine(FPaths::ProjectConfigDir(), ConfigFileName);
	GameConfigFilePath = FPaths::Combine(FPaths::ProjectConfigDir(), GameConfigFileName);
	FString DataCSVName = "data.csv";
	FileSavePath = FPaths::Combine(FPaths::ProjectSavedDir(), DataCSVName);

	// Create and register the data table function library pin factory
	const TSharedPtr<FDataTableFunctionLibraryPinFactory> DataTableFunctionLibraryPinFactory = MakeShareable(new FDataTableFunctionLibraryPinFactory());
	FEdGraphUtilities::RegisterVisualPinFactory(DataTableFunctionLibraryPinFactory);
}

void FDataTableFunctionLibraryEditorModule::ShutdownModule()
{
}

void FDataTableFunctionLibraryEditorModule::OnAssetOpened(UObject* Asset, IAssetEditorInstance* AssetEditorInstance)
{
	if (UDataTable* DataTable = Cast<UDataTable>(Asset))
	{
		IDataTableEditor* DataTableEditor = StaticCast<IDataTableEditor*>(AssetEditorInstance);
		if (!DataTableEditor) return;
		ToolbarExtender = MakeShareable(new FExtender);
		ToolbarExtender->AddToolBarExtension(
			"DataTableCommands",
			EExtensionHook::After,
			nullptr,
			FToolBarExtensionDelegate::CreateLambda([this, DataTable](FToolBarBuilder& ToolbarBuilder)
			{
				ToolbarBuilder.BeginSection("ReimportCommands");
				{
					ToolbarBuilder.AddToolBarButton(
						FUIAction(FExecuteAction::CreateLambda([this, DataTable]()
						{
							this->OnReimportButtonClicked(DataTable);
						})),
						NAME_None,
						LOCTEXT("ReimportIconText", "Reimport"),
						LOCTEXT("ReimportIconTooltip", "Import the data table from a Google Sheet document."),
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION == 1
						FSlateIcon(FAppStyle::GetAppStyleSetName(), "TextureEditor.Reimport")
#else
						FSlateIcon(FEditorStyle::GetStyleSetName(), "TextureEditor.Reimport")
#endif
					);

					ToolbarBuilder.AddComboButton(
						FUIAction(),
						FOnGetContent::CreateRaw(this, &FDataTableFunctionLibraryEditorModule::CreateReimportMenu, DataTable),
						LOCTEXT("ReimportMenuText", ""),
						LOCTEXT("ReimportMenuTooltip", "Google sheet import data table menu."),
						FSlateIcon()
					);
				}
				ToolbarBuilder.EndSection();
			}));
		DataTableEditor->AddToolbarExtender(ToolbarExtender);
	}
}

void FDataTableFunctionLibraryEditorModule::OnPostEngineInit()
{
	if (GEditor)
	{
		GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OnAssetOpenedInEditor().AddRaw(this, &FDataTableFunctionLibraryEditorModule::OnAssetOpened);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Could not get GEditor in DataTableFunctionLibrarayEditorModule."));
	}
}

TSharedRef<SWidget> FDataTableFunctionLibraryEditorModule::CreateReimportMenu(UDataTable* DataTable)
{
	FMenuBuilder MenuBuilder(false, nullptr);

	MenuBuilder.BeginSection(NAME_None, LOCTEXT("DataTableReimportMenu", "Data Table Reimport Menu"));
	MenuBuilder.AddEditableText(
		LOCTEXT("GoogleSheetKeyText", "Google Sheet Key"),
		LOCTEXT("GoogleSheetKeyTooltip", "The google sheet doc key used to import the data table"),
		FSlateIcon(),
		GetGoogleSheetKeyFromDataTable(DataTable),
		FOnTextCommitted::CreateRaw(this, &FDataTableFunctionLibraryEditorModule::OnGoogleSheetKeyTextCommitted, DataTable),
		FOnTextChanged(),
		false);
	MenuBuilder.AddEditableText(
		LOCTEXT("GoogleSheetTabText", "Google Sheet Tab"),
		LOCTEXT("GoogleSheetTabTextTooltip", "The name of the google sheet tab to reimport. Can be empty if the first sheet wants to be imported"),
		FSlateIcon(),
		GetGoogleSheetTabNameFromDataTable(DataTable),
		FOnTextCommitted::CreateRaw(this, &FDataTableFunctionLibraryEditorModule::OnGoogleSheetTabNameTextCommited, DataTable),
		FOnTextChanged(),
		false);
	MenuBuilder.EndSection();
	return MenuBuilder.MakeWidget();
}

void FDataTableFunctionLibraryEditorModule::OnReimportButtonClicked(UDataTable* DataTable)
{
	if (DataTable)
	{
		const FString GoogleSheetKey = GetGoogleSheetKeyFromDataTable(DataTable).ToString();
		if (GoogleSheetKey.IsEmpty())
		{
			const FText Title = FText::FromString("Google Sheet Key Not Valid");
			const FText Message = FText::FromString("You must provide a google sheet key to reimport the data table.");
			FMessageDialog::Open(EAppMsgType::Ok, EAppReturnType::Ok, Message, &Title);
			return;
		}

		const FString GoogleSheetTabName = GetGoogleSheetTabNameFromDataTable(DataTable).ToString();

		FHttpModule* HTTP = &FHttpModule::Get();
		if (HTTP)
		{
			auto Request = HTTP->CreateRequest();
			Request->OnProcessRequestComplete().BindRaw(this, &FDataTableFunctionLibraryEditorModule::OnResponseReceived);
			Request->SetURL("https://docs.google.com/spreadsheets/d/" + GoogleSheetKey + "/export?format=csv");
			if(GoogleSheetTabName.IsEmpty()) Request->SetURL("https://docs.google.com/spreadsheets/d/" + GoogleSheetKey + "/export?format=csv");
			else Request->SetURL("https://docs.google.com/spreadsheets/d/" + GoogleSheetKey + "/gviz/tq?tqx=out:csv&sheet=" + GoogleSheetTabName);
			Request->SetVerb("GET");
			Request->SetHeader("User-Agent", "X-UnrealEngine-Agent");
			Request->SetHeader("Content-Type", "text/html");
			ReimportDataTable = DataTable;
			Request->ProcessRequest();
		}
	}
}

FText FDataTableFunctionLibraryEditorModule::GetGoogleSheetKeyFromDataTable(UDataTable* DataTable)
{
	if (!IFileManager::Get().FileExists(*ConfigFilePath))
	{
		return FText::FromString("");
	}

	if (GConfig)
	{
		GConfig->LoadFile(ConfigFileName);
		FString Value;
		const bool bFoundValue = GConfig->GetString(*DataTableSection, *GetKeyFromDataTable(DataTable), Value, ConfigFilePath);
		if (bFoundValue) return FText::FromString(Value);
		return FText::FromString("");
	}

	return FText::FromString("");
}

void FDataTableFunctionLibraryEditorModule::OnGoogleSheetKeyTextCommitted(const FText& Text, ETextCommit::Type TextCommit, UDataTable* DataTable)
{
	if (!IFileManager::Get().FileExists(*ConfigFilePath))
	{
		const FString DummyContent;
		FFileHelper::SaveStringToFile(DummyContent, *ConfigFilePath);
		if(GConfig)
		{
			GLog->Logf(TEXT("Name %s"), *GameConfigFilePath);
			GConfig->LoadFile(GameConfigFilePath);
			const FString StagingSection = "Staging";
			const FString Key = "+BlacklistConfigFiles";
			const FString Value = FString(FApp::GetProjectName()) + "/Config/DataTableConfig.ini";
			GConfig->SetString(*StagingSection, *Key, *Value, GameConfigFilePath);
			GConfig->Flush(false, GameConfigFilePath);
		}
	}

	if (GConfig)
	{
		GConfig->LoadFile(ConfigFileName);
		GConfig->SetString(*DataTableSection, *GetKeyFromDataTable(DataTable), *Text.ToString(), ConfigFilePath);
		GConfig->Flush(false, ConfigFilePath);
	}
}

FString FDataTableFunctionLibraryEditorModule::GetKeyFromDataTable(UDataTable* DataTable)
{
	const int32 Index = DataTable->GetFullName().Find(" ", ESearchCase::IgnoreCase, ESearchDir::FromEnd);
	const FString DataTableName = DataTable->GetFullName().RightChop(Index + 1);
	return DataTableName;
}

void FDataTableFunctionLibraryEditorModule::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!Response.IsValid() || !EHttpResponseCodes::IsOk(Response->GetResponseCode()) || !bWasSuccessful)
	{
		FText Title;
		FText Message;
		if (!Response.IsValid())
		{
			Title = FText::FromString("HTTP Request Failed");
			Message = FText::FromString("HTTP response is not valid.");
		}

		if (!EHttpResponseCodes::IsOk(Response->GetResponseCode()))
		{
			const FString GoogleSheetKey = GetGoogleSheetKeyFromDataTable(ReimportDataTable).ToString();
			Title = FText::FromString("HTTP Status Code Is Not OK");
			Message = FText::FromString(FString::Printf(TEXT("Failed to get Google Sheet document using key '%s'.\n\nMake sure the key is corrent and that the document share policy is set to 'Anyone with the link'"), *GoogleSheetKey));
		}

		if (!bWasSuccessful)
		{
			Title = FText::FromString("Download Failed");
			Message = FText::FromString("Could not download Google Sheet document as CSV file.");
		}

		FMessageDialog::Open(EAppMsgType::Ok, EAppReturnType::Ok, Message, &Title);
		return;
	}

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	// Delete the file if it already exists
	if (!FileSavePath.IsEmpty() && FPaths::FileExists(*FileSavePath))
	{
		IFileManager& FileManager = IFileManager::Get();
		FileManager.Delete(*FileSavePath);
	}

	// Open / Create the file
	IFileHandle* FileHandle = PlatformFile.OpenWrite(*FileSavePath);
	if (FileHandle)
	{
		// Write the file
		FileHandle->Write(Response->GetContent().GetData(), Response->GetContentLength());

		Response.Reset();

		// Close the file
		delete FileHandle;
		
		if (ReimportDataTable)
		{
			const bool bSuccessfullyImported = UDataTableFunctionLibrary::FillDataTableFromCSVFile(ReimportDataTable, *FileSavePath);
			if (!bSuccessfullyImported)
			{
				const FText Title = FText::FromString("Failed Reimport");
				const FText Message = FText::FromString("Failed reimporting the data table.\nMake sure the Google Sheet document matches the data table struct.");
				FMessageDialog::Open(EAppMsgType::Ok, EAppReturnType::Ok, Message, &Title);
				return;
			}

			// Update the data table view
			FDataTableEditorUtils::FDataTableEditorManager::Get().PostChange(ReimportDataTable, FDataTableEditorUtils::EDataTableChangeInfo::RowList);

			// Delete csv file
			PlatformFile.DeleteFile(*FileSavePath);
		}
	}
	else
	{
		const FText Title = FText::FromString("Failed Downloading File");
		const FText Message = FText::FromString("Could not download the CSV file.");
		FMessageDialog::Open(EAppMsgType::Ok, EAppReturnType::Ok, Message, &Title);
	}
}

FText FDataTableFunctionLibraryEditorModule::GetGoogleSheetTabNameFromDataTable(UDataTable* DataTable)
{
	if (!IFileManager::Get().FileExists(*ConfigFilePath))
	{
		return FText::FromString("");
	}

	if (GConfig)
	{
		GConfig->LoadFile(ConfigFileName);
		FString Value;
		const bool bFoundValue = GConfig->GetString(*DataTableSection, *GetTabNameFromDataTable(DataTable), Value, ConfigFilePath);
		if (bFoundValue) return FText::FromString(Value);
		return FText::FromString("");
	}

	return FText::FromString("");
}

void FDataTableFunctionLibraryEditorModule::OnGoogleSheetTabNameTextCommited(const FText& Text, ETextCommit::Type TextCommit, UDataTable* DataTable)
{
	if (!IFileManager::Get().FileExists(*ConfigFilePath))
	{
		const FString DummyContent;
		FFileHelper::SaveStringToFile(DummyContent, *ConfigFilePath);
		if(GConfig)
		{
			GConfig->LoadFile(GameConfigFilePath);
			const FString StagingSection = "Staging";
			const FString Key = "+BlacklistConfigFiles";
			const FString Value = FString(FApp::GetProjectName()) + "/Config/DataTableConfig.ini";
			GConfig->SetString(*StagingSection, *Key, *Value, GameConfigFilePath);
			GConfig->Flush(false, GameConfigFilePath);
		}
	}

	if (GConfig)
	{
		GConfig->LoadFile(ConfigFileName);
		GConfig->SetString(*DataTableSection, *GetTabNameFromDataTable(DataTable), *Text.ToString(), ConfigFilePath);
		GConfig->Flush(false, ConfigFilePath);
	}
}

FString FDataTableFunctionLibraryEditorModule::GetTabNameFromDataTable(UDataTable* DataTable)
{
	const int32 Index = DataTable->GetFullName().Find(" ", ESearchCase::IgnoreCase, ESearchDir::FromEnd);
	const FString DataTableName = DataTable->GetFullName().RightChop(Index + 1) + "TabName";
	return DataTableName;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FDataTableFunctionLibraryEditorModule, DataTableFunctionLibraryEditor)
