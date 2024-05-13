// Copyright YTSS 2023. All Rights Reserved.


#include "Initialization/ES_BPableSubsystem_PluginProxy.h"
#include "Initialization/BlueprintableSubsystemAssetFunctions.h"

void UES_BPableSubsystem_PluginProxy::Initialize(FSubsystemCollectionBase& Collection)
{
	TSet<FString> PreparedPaths;
	FIND_AllPluginsDirectory(PreparedPaths);
	LOAD_ByPaths_BlueprintableSubsystem(PreparedPaths);
}
