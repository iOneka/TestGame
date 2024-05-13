// Copyright YTSS 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Engine/World.h"
#include "BlueprintableWorldSubsystem.generated.h"

/**
 * UBlueprintableWorldSubsystem
 * Provides a WorldSubsystem base class from which SubBlueprint classes can be derived
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class BLUEPRINTABLESUBSYSTEM_API UBlueprintableWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UBlueprintableWorldSubsystem()
		: bInitialized(false)
	{
	}

protected:
	uint8 bInitialized:1;

public:
	// UWorldSubsystem implementation Begin
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual void PostInitialize() override { ReceivePostInitialize(); }
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	// UWorldSubsystem implementation End

protected:
	// UWorldSubsystem implementation Begin
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override
	{
		return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
	}

	// UWorldSubsystem implementation End


	//Blueprint functions Begin

	/**This event called after this subsystem is created.*/
	UFUNCTION(BlueprintImplementableEvent, DisplayName="Initialize")
	void ReceiveInitialize();

	/**This event called before this subsystem is destroyed.*/
	UFUNCTION(BlueprintImplementableEvent, DisplayName="Deinitialize")
	void ReceiveDeinitialize();

	/**
	 * This event called before this subsystem is created.
	 * This event is used to determine whether to create the subsystem singleton.
	 * @param Outer - The outer of UBPable_LocalPlayerSubsystem should be a UWorld
	 * @return - The return value determines whether to create the Subsystem
	 */
	UFUNCTION(BlueprintNativeEvent, DisplayName="ShouldCreateSubsystem")
	bool ReceiveShouldCreateSubsystem(UWorld* Outer) const;

	/**
	 * Called once all UWorldSubsystems have been initialized
	 */
	UFUNCTION(BlueprintImplementableEvent, DisplayName="PostInitialze")
	void ReceivePostInitialize();

	/**
	 * Called when world is ready to start gameplay before the game mode transitions to the correct state and call BeginPlay on all actors
	 */
	UFUNCTION(BlueprintImplementableEvent, DisplayName="WorldBeginPlay")
	void ReceiveWorldBeginPlay();

	//Blueprint functions End
};
