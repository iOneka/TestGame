// Copyright YTSS 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/World.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"
#include "BlueprintableLocalPlayerSubsystem.generated.h"

/**
 * UBlueprintableLocalPlayerSubsystem
 * Provides a LocalPlayerSubsystem base class from which SubBlueprint classes can be derived
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class BLUEPRINTABLESUBSYSTEM_API UBlueprintableLocalPlayerSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	UBlueprintableLocalPlayerSubsystem()
		: bInitialized(false)
	{
	}

	virtual UWorld* GetWorld() const override final
	{
		return IsValid(Cast<UObject>(GetLocalPlayerController())) ? GetLocalPlayerController()->GetWorld() : nullptr;
	}

protected:
	uint8 bInitialized:1;

private:
	// Middle function Begin
	void PostWorldInitialization(UWorld* World, const UWorld::InitializationValues IVS);
	void StartGameInstance(UGameInstance* GameInstance);
	// Middle function End
public:
	// ULocalPlayerSubsystem implementation Begin
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// ULocalPlayerSubsystem implementation End

protected:
	//Blueprint functions Begin

	/**The event called after this subsystem is created.*/
	UFUNCTION(BlueprintImplementableEvent, DisplayName="Initialize")
	void ReceiveInitialize();

	/**The event called before this subsystem is destroyed.*/
	UFUNCTION(BlueprintImplementableEvent, DisplayName="Deinitialize")
	void ReceiveDeinitialize();

	/**The event called after world begin play.*/
	UFUNCTION(BlueprintImplementableEvent, DisplayName="WorldBeginPlay")
	void ReceiveWorldBeginPlay();

	/**The event called before world tear down.*/
	UFUNCTION(BlueprintImplementableEvent, DisplayName="WorldBeginTearDown")
	void ReceiveWorldBeginTearingDown();
	/**
	 * The event called before this subsystem is created.
	 * This event is used to determine whether to create the subsystem singleton.
	 * @param Outer - The outer of UBPable_LocalPlayerSubsystem should be a ULocalPlayer
	 * @return - The return value determines whether to create the Subsystem
	 */
	UFUNCTION(BlueprintNativeEvent, DisplayName="ShouldCreateSubsystem")
	bool ReceiveShouldCreateSubsystem(UObject* Outer) const;

public:
	/**
	 * Return PlayerController of this Subsystem
	 * PlayerController does not exist when Initialize is executing
	 */
	UFUNCTION(BlueprintPure, Category="LocalPlayerSubsystem")
	APlayerController* GetLocalPlayerController() const
	{
		return GetLocalPlayer() ? GetLocalPlayer()->PlayerController : nullptr;
	}

	//Blueprint functions End
};
