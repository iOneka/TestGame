// Copyright YTSS 2023. All Rights Reserved.

#include "Blueprintables/BlueprintableGameInstanceSubsystem.h"
#include "BlueprintableSubsystemLogging.h"

UBlueprintableGameInstanceSubsystem::UBlueprintableGameInstanceSubsystem(): bInitialized(false)
{
}

UWorld* UBlueprintableGameInstanceSubsystem::GetWorld() const
{
	return GetGameInstance() ? GetGameInstance()->GetWorld() : nullptr;
}

void UBlueprintableGameInstanceSubsystem::PostWorldInitialization(UWorld* World, const UWorld::InitializationValues IVS)
{
	World->OnWorldBeginPlay.AddUObject(this, &UBlueprintableGameInstanceSubsystem::ReceiveWorldBeginPlay);
}

void UBlueprintableGameInstanceSubsystem::StartGameInstance(UGameInstance* GameInstance)
{
	if (GWorld)
	{
		GWorld->OnWorldBeginPlay.AddUObject(this, &UBlueprintableGameInstanceSubsystem::ReceiveWorldBeginPlay);
	}
}

bool UBlueprintableGameInstanceSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return Super::ShouldCreateSubsystem(Outer) && ReceiveShouldCreateSubsystem(Outer);
}

void UBlueprintableGameInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	check(!bInitialized)
	
	// World delegation assign
	FWorldDelegates::OnStartGameInstance.AddUObject(this, &UBlueprintableGameInstanceSubsystem::StartGameInstance);
	FWorldDelegates::OnPostWorldInitialization.
		AddUObject(this, &UBlueprintableGameInstanceSubsystem::PostWorldInitialization);
	FWorldDelegates::OnWorldBeginTearDown.AddUFunction(this, "ReceiveWorldBeginTearingDown");

	ReceiveInitialize();

	bInitialized = true;

	UE_LOG(LogBlueprintableSubsystem, Log, TEXT("%s_Initialize"), *GetName())
}

void UBlueprintableGameInstanceSubsystem::Deinitialize()
{
	check(bInitialized)
	ReceiveDeinitialize();
	bInitialized = false;
	UE_LOG(LogBlueprintableSubsystem, Log, TEXT("%s_Deinitialize"), *GetName())
}

bool UBlueprintableGameInstanceSubsystem::ReceiveShouldCreateSubsystem_Implementation(UObject* Outer) const
{
	return true;
}
