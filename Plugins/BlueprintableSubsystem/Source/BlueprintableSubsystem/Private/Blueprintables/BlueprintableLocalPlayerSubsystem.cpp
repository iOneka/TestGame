// Copyright YTSS 2023. All Rights Reserved.

#include "Blueprintables/BlueprintableLocalPlayerSubsystem.h"
#include "BlueprintableSubsystemLogging.h"

void UBlueprintableLocalPlayerSubsystem::PostWorldInitialization(UWorld* World, const UWorld::InitializationValues IVS)
{
	World->OnWorldBeginPlay.AddUObject(this, &UBlueprintableLocalPlayerSubsystem::ReceiveWorldBeginPlay);
}

void UBlueprintableLocalPlayerSubsystem::StartGameInstance(UGameInstance* GameInstance)
{
	if(GWorld)
	{
		GWorld->OnWorldBeginPlay.AddUObject(this, &UBlueprintableLocalPlayerSubsystem::ReceiveWorldBeginPlay);
	}
}

bool UBlueprintableLocalPlayerSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return Super::ShouldCreateSubsystem(Outer) && ReceiveShouldCreateSubsystem(Outer);
}

void UBlueprintableLocalPlayerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	check(!bInitialized)

	// World delegation assign
	FWorldDelegates::OnStartGameInstance.AddUObject(this, &UBlueprintableLocalPlayerSubsystem::StartGameInstance);
	FWorldDelegates::OnPostWorldInitialization.AddUObject(this, &UBlueprintableLocalPlayerSubsystem::PostWorldInitialization);
	FWorldDelegates::OnWorldBeginTearDown.AddUFunction(this, "ReceiveWorldBeginTearingDown");

	ReceiveInitialize();

	bInitialized=true;
	
	UE_LOG(LogBlueprintableSubsystem, Log, TEXT("%s_Initialize"), *GetName())
}

void UBlueprintableLocalPlayerSubsystem::Deinitialize()
{
	check(bInitialized);
	ReceiveDeinitialize();
	bInitialized=false;
	UE_LOG(LogBlueprintableSubsystem, Log, TEXT("%s_Deinitialize"), *GetName())
}

bool UBlueprintableLocalPlayerSubsystem::ReceiveShouldCreateSubsystem_Implementation(UObject* Outer) const
{
	return true;
}