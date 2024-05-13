// Copyright YTSS 2023. All Rights Reserved.

#include "Blueprintables/BlueprintableWorldSubsystem.h"
#include "BlueprintableSubsystemLogging.h"

bool UBlueprintableWorldSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return Super::ShouldCreateSubsystem(Outer) && ReceiveShouldCreateSubsystem(Cast<UWorld>(Outer));
}

void UBlueprintableWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	check(!bInitialized)
	
	ReceiveInitialize();
	
	bInitialized=true;
	
	UE_LOG(LogBlueprintableSubsystem, Log, TEXT("%s_Initialize"), *GetName())
}

void UBlueprintableWorldSubsystem::Deinitialize()
{
	check(bInitialized)
	ReceiveDeinitialize();
	bInitialized=false;
	UE_LOG(LogBlueprintableSubsystem, Log, TEXT("%s_Deinitialize"), *GetName())
}

void UBlueprintableWorldSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	ReceiveWorldBeginPlay();
}

bool UBlueprintableWorldSubsystem::ReceiveShouldCreateSubsystem_Implementation(UWorld* Outer) const
{
	return true;
}
