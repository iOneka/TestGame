// Copyright YTSS 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions/AssetTypeActions_Blueprint.h"
#include "Blueprintables/BlueprintableGameInstanceSubsystem.h"
#include "Blueprintables/BlueprintableLocalPlayerSubsystem.h"
#include "Blueprintables/BlueprintableWorldSubsystem.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

class BLUEPRINTABLESUBSYSTEMEDITOR_API FBlueprintableGameInstanceSubsystemAssetTypeActions : public FAssetTypeActions_Base
{
public:
	virtual UClass* GetSupportedClass() const override
	{
		return UBlueprintableGameInstanceSubsystem::StaticClass();
	}

	virtual FText GetName() const override
	{
		return FText::FromString("Game Instance Subsystem");
	}
	
	virtual FColor GetTypeColor() const override
	{
		return FColor(63, 126, 255);
	}
	
	virtual uint32 GetCategories() override
	{
		if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
		{
			return FAssetToolsModule::GetModule().Get().FindAdvancedAssetCategory(FName("Subsystems"));
		}
		static EAssetTypeCategories::Type const AssetCategory = EAssetTypeCategories::Blueprint;
		return AssetCategory;
	}
};

class BLUEPRINTABLESUBSYSTEMEDITOR_API FBlueprintableWorldSubsystemAssetTypeActions : public FAssetTypeActions_Base
{
public:
	virtual UClass* GetSupportedClass() const override
	{
		return UBlueprintableWorldSubsystem::StaticClass();
	}

	virtual FText GetName() const override
	{
		return FText::FromString("World Subsystem");
	}
	
	virtual FColor GetTypeColor() const override
	{
		return FColor(63, 126, 255);
	}
	
	virtual uint32 GetCategories() override
	{
		if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
		{
			return FAssetToolsModule::GetModule().Get().FindAdvancedAssetCategory(FName("Subsystems"));
		}
		static EAssetTypeCategories::Type const AssetCategory = EAssetTypeCategories::Blueprint;
		return AssetCategory;
	}
};

class BLUEPRINTABLESUBSYSTEMEDITOR_API FBlueprintableLocalPlayerSubsystemAssetTypeActions : public FAssetTypeActions_Base
{
public:
	virtual UClass* GetSupportedClass() const override
	{
		return UBlueprintableLocalPlayerSubsystem::StaticClass();
	}

	virtual FText GetName() const override
	{
		return FText::FromString("Local Player Subsystem");
	}
	
	virtual FColor GetTypeColor() const override
	{
		return FColor(63, 126, 255);
	}
	
	virtual uint32 GetCategories() override
	{
		if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
		{
			return FAssetToolsModule::GetModule().Get().FindAdvancedAssetCategory(FName("Subsystems"));
		}
		static EAssetTypeCategories::Type const AssetCategory = EAssetTypeCategories::Blueprint;
		return AssetCategory;
	}
};



#undef LOCTEXT_NAMESPACE