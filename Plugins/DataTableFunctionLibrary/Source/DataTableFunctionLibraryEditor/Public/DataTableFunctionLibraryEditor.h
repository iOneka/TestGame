// Copyright 2022 Just2Devs. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include "Modules/ModuleManager.h"

class FDataTableFunctionLibraryEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

public:
	TSharedPtr<FExtender> ToolbarExtender;
	FString ConfigFileName = "DataTableConfig.ini";
	FString ConfigFilePath;
	FString GameConfigFileName = "DefaultGame.ini";
	FString GameConfigFilePath;
	FString FileSavePath;
	FString DataTableSection = "DataTableKeys";
	UDataTable* ReimportDataTable = nullptr;
	
public:
	void OnAssetOpened(UObject* Asset, IAssetEditorInstance* AssetEditorInstance);
	void OnPostEngineInit();
	TSharedRef<SWidget> CreateReimportMenu(UDataTable* DataTable);
	void OnReimportButtonClicked(UDataTable* DataTable);
	FText GetGoogleSheetKeyFromDataTable(UDataTable* DataTable);
	void OnGoogleSheetKeyTextCommitted(const FText& Text, ETextCommit::Type TextCommit, UDataTable* DataTable);
	FString GetKeyFromDataTable(UDataTable* DataTable);
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	
	FText GetGoogleSheetTabNameFromDataTable(UDataTable* DataTable);
	void OnGoogleSheetTabNameTextCommited(const FText& Text, ETextCommit::Type TextCommit, UDataTable* DataTable);
	FString GetTabNameFromDataTable(UDataTable* DataTable);
};


