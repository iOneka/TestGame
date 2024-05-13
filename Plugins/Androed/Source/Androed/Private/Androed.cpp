// Copyright Epic Games, Inc. All Rights Reserved.

#include "Androed.h"
#include "AndroedStyle.h"
#include "AndroedCommands.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"

static const FName AndroedTabName("Androed");

#define LOCTEXT_NAMESPACE "FAndroedModule"

void FAndroedModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	FAndroedStyle::Initialize();
	FAndroedStyle::ReloadTextures();

	FAndroedCommands::Register();

	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FAndroedCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FAndroedModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(
		FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FAndroedModule::RegisterMenus));
}

void FAndroedModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FAndroedStyle::Shutdown();

	FAndroedCommands::Unregister();
}

void FAndroedModule::PluginButtonClicked()
{
	const FString ProjectPath = FPaths::ConvertRelativePathToFull(FPaths::GetProjectFilePath());
	const FString EnginePath = FPaths::ConvertRelativePathToFull(FPaths::EngineDir());
	const FString ScriptPath = FPaths::ConvertRelativePathToFull(
		FPaths::Combine(FPaths::ProjectPluginsDir(), "Androed/Source/Androed/Python/deploy.py")
	);
	
	const FString Script = FString::Printf(TEXT("/K py \"%s\""), *ScriptPath);
	const FString Arguments = FString::Printf(
		TEXT("--project=\"%s\" --engine=\"%s\""),
		*ProjectPath,
		*EnginePath
	);
	
	const FString Command = Script + " " + Arguments;
	
	// Run python
	const FProcHandle Proc = FPlatformProcess::CreateProc(
		TEXT("cmd"), *Command, true, false, false, nullptr, 0, nullptr, nullptr
	);
	
	if (!Proc.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Could not start process"));
	}
}

void FAndroedModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FAndroedCommands::Get().PluginAction, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
			{
				FToolMenuEntry& Entry = Section.AddEntry(
					FToolMenuEntry::InitToolBarButton(FAndroedCommands::Get().PluginAction));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FAndroedModule, Androed)
