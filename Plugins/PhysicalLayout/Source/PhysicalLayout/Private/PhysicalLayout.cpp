// Copyright Saeid Gholizade. All Rights Reserved. 2020

#include "PhysicalLayout.h"
#include "PhysicalLayoutMode.h"
#include "PhysicalLayoutCommands.h"
#include "PhysicalLayoutStyle.h"


#define LOCTEXT_NAMESPACE "FPhysicalLayoutModule"

void FPhysicalLayoutModule::StartupModule()
{
	FPhysicalLayoutStyle::Initialize();
	FPhysicalLayoutStyle::ReloadTextures();
	FPhysicalLayoutCommands::Register();

	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	FEditorModeRegistry::Get().RegisterMode<FPhysicalLayoutMode>(
		FPhysicalLayoutMode::EM_PhysicalLayoutModeId, 
		LOCTEXT("PhysicalLayoutEdModeName", "Physical Layout Mode"),
		FSlateIcon(FPhysicalLayoutStyle::GetStyleSetName(), "PhysicalLayout", "PhysicalLayout.Icon"),
		true);
}

void FPhysicalLayoutModule::ShutdownModule()
{
	FPhysicalLayoutStyle::Shutdown();
	FPhysicalLayoutCommands::Unregister();

	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FEditorModeRegistry::Get().UnregisterMode(FPhysicalLayoutMode::EM_PhysicalLayoutModeId);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FPhysicalLayoutModule, PhysicalLayout)