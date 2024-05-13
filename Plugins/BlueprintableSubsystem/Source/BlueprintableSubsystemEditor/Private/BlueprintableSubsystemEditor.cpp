// Copyright YTSS 2023. All Rights Reserved.

#include "BlueprintableSubsystemEditor.h"

#include "AssetToolsModule.h"

#include "BlueprintableSubsystemAssetTypeActions.h"

#include "IAssetTools.h"

#define LOCTEXT_NAMESPACE "FBlueprintableSubsystemEditorModule"

void FBlueprintableSubsystemEditorModule::StartupModule()
{
	FAssetToolsModule::GetModule().Get().RegisterAdvancedAssetCategory(FName("Subsystems"), FText::FromString("Subsystems"));
	
	// GetAssetTool
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	
	// Spawn AssetTypeActions
	ActionTypes.Append
	(
		{
			MakeShareable(new FBlueprintableGameInstanceSubsystemAssetTypeActions),
			MakeShareable(new FBlueprintableWorldSubsystemAssetTypeActions),
			MakeShareable(new FBlueprintableLocalPlayerSubsystemAssetTypeActions)
		}
	);

	// Register AssetTypeAction
	for (auto Action : ActionTypes)
	{
		AssetTools.RegisterAssetTypeActions(Action.ToSharedRef());
	}
}

void FBlueprintableSubsystemEditorModule::ShutdownModule()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		IAssetTools& AssetTools = FModuleManager::Get().GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
		for (auto Action : ActionTypes)
		{
			AssetTools.UnregisterAssetTypeActions(Action.ToSharedRef());
		}
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FBlueprintableSubsystemEditorModule, BlueprintableSubsystemEditor)
