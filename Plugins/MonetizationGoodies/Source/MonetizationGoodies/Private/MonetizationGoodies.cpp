// Copyright (c) 2020 Nineva Studios

#include "MonetizationGoodies.h"
#include "MonetizationGoodiesSettings.h"
#include "MonetizationGoodiesLog.h"

#include "Engine/Engine.h"
#include "UObject/UObjectGlobals.h"
#include "Developer/Settings/Public/ISettingsModule.h"

#define LOCTEXT_NAMESPACE "FMonetizationGoodiesModule"

void FMonetizationGoodiesModule::StartupModule()
{
	FString PluginName = "MonetizationGoodies";

	MonetizationGoodiesSettings = NewObject<UMonetizationGoodiesSettings>((UObject*) GetTransientPackage(), "MonetizationGoodiesSettings", RF_Standalone);
	MonetizationGoodiesSettings->AddToRoot();

	// Register settings
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Project", "Plugins", "MonetizationGoodies",
			LOCTEXT("RuntimeSettingsName", "Monetization Goodies"),
			LOCTEXT("RuntimeSettingsDescription", "Configure Monetization Goodies"),
			MonetizationGoodiesSettings);
	}
}

void FMonetizationGoodiesModule::ShutdownModule()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "MonetizationGoodies");
	}

	if (!GExitPurge)
	{
		// If we're in exit purge, this object has already been destroyed
		MonetizationGoodiesSettings->RemoveFromRoot();
	}
	else
	{
		MonetizationGoodiesSettings = nullptr;
	}
}

UMonetizationGoodiesSettings* FMonetizationGoodiesModule::GetSettings() const
{
	return MonetizationGoodiesSettings;
}

#undef LOCTEXT_NAMESPACE

DEFINE_LOG_CATEGORY(LogMonetizationGoodies);

IMPLEMENT_MODULE(FMonetizationGoodiesModule, MonetizationGoodies)
