// Copyright YTSS 2023. All Rights Reserved.

#include "Initialization/BlueprintableSubsystemAssetFunctions.h"
#include "BlueprintableSubsystemLogging.h"
#include "Blueprintables/BlueprintableGameInstanceSubsystem.h"
#include "Blueprintables/BlueprintableLocalPlayerSubsystem.h"
#include "Blueprintables/BlueprintableWorldSubsystem.h"
#include "Engine/ObjectLibrary.h"
#include "Interfaces/IPluginManager.h"

void FBlueprintableSubsystemAssetFunctions::FindAllPluginsDirectory(TSet<FString>& PluginDirectorys)
{
	IPluginManager& PluginManager = IPluginManager::Get();

	auto Plugins = PluginManager.GetEnabledPluginsWithContent();
	TSet<FString> PluginNames;

	for (auto& Plugin : Plugins)
	{
		if (Plugin->GetBaseDir().Contains("Marketplace") || Plugin->GetLoadedFrom() == EPluginLoadedFrom::Project)
		{
			PluginNames.Add(Plugin->GetMountedAssetPath());
		}
	}

	PluginDirectorys.Add("/Game/");
}

void FBlueprintableSubsystemAssetFunctions::LoadSubsystemByPaths(UClass* SubsystemClass, const TSet<FString>& Paths)
{
	UObjectLibrary* ObjectLibrary = UObjectLibrary::CreateLibrary(SubsystemClass, true, GIsEditor);
	ObjectLibrary->AddToRoot();
	if (!IsValid(ObjectLibrary))
	{
		UE_LOG(LogBlueprintableSubsystem, Warning,
		       TEXT("Unable to create BPable_Subsystem ObjectLibrary. Cause the SubBlueprint of BPable_... cant be uesd"
		       ));
		return;
	}
	ObjectLibrary->LoadBlueprintAssetDataFromPaths(Paths.Array());
	ObjectLibrary->LoadAssetsFromAssetData();
}

void FBlueprintableSubsystemAssetFunctions::LoadAllBPableSubsystemByPaths(const TSet<FString>& Paths)
{
	LoadSubsystemByPaths(UBlueprintableGameInstanceSubsystem::StaticClass(), Paths);
	LoadSubsystemByPaths(UBlueprintableLocalPlayerSubsystem::StaticClass(), Paths);
	LoadSubsystemByPaths(UBlueprintableWorldSubsystem::StaticClass(), Paths);
}
