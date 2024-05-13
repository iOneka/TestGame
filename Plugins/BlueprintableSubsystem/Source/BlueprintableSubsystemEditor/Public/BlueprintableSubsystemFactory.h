// Copyright YTSS 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Blueprintables/BlueprintableGameInstanceSubsystem.h"
#include "Blueprintables/BlueprintableLocalPlayerSubsystem.h"
#include "Blueprintables/BlueprintableWorldSubsystem.h"

#include "Kismet2/KismetEditorUtilities.h"
#include "BlueprintableSubsystemFactory.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class BLUEPRINTABLESUBSYSTEMEDITOR_API UBlueprintableSubsystemFactory : public UFactory
{
	GENERATED_BODY()

public:
	UBlueprintableSubsystemFactory()
	{
		bCreateNew = true;
		bEditAfterNew = true;
	}

	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
	                                  UObject* Context, FFeedbackContext* Warn, FName CallingContext) override
	{
		return FKismetEditorUtilities::CreateBlueprint(InClass, InParent, InName, BPTYPE_Normal,
		                                               UBlueprint::StaticClass(),
		                                               UBlueprintGeneratedClass::StaticClass(), CallingContext);
	}

	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
	                                  UObject* Context, FFeedbackContext* Warn) override
	{
		return FactoryCreateNew(InClass, InParent, InName, Flags, Context, Warn, NAME_None);
	}
};


UCLASS(HideCategories=Object)
class BLUEPRINTABLESUBSYSTEMEDITOR_API UBlueprintableGameInstanceSubsystemFactory : public UBlueprintableSubsystemFactory
{
	GENERATED_BODY()

public:
	UBlueprintableGameInstanceSubsystemFactory()
	{
		SupportedClass = UBlueprintableGameInstanceSubsystem::StaticClass();
	}
};

UCLASS(HideCategories=Object)
class BLUEPRINTABLESUBSYSTEMEDITOR_API UBlueprintableWorldSubsystemFactory : public UBlueprintableSubsystemFactory
{
	GENERATED_BODY()

public:
	UBlueprintableWorldSubsystemFactory()
	{
		SupportedClass = UBlueprintableWorldSubsystem::StaticClass();
	}
};

UCLASS(HideCategories=Object)
class BLUEPRINTABLESUBSYSTEMEDITOR_API UBlueprintableLocalPlayerSubsystemFactory : public UBlueprintableSubsystemFactory
{
	GENERATED_BODY()

public:
	UBlueprintableLocalPlayerSubsystemFactory()
	{
		SupportedClass = UBlueprintableLocalPlayerSubsystem::StaticClass();
	}
};
