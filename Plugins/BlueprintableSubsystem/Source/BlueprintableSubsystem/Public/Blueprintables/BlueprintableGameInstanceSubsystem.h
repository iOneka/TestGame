// Copyright YTSS 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/World.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "BlueprintableGameInstanceSubsystem.generated.h"

/**
 * Provides a GameInstanceSubsystem base class from which SubBlueprint classes can be derived
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class BLUEPRINTABLESUBSYSTEM_API UBlueprintableGameInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UBlueprintableGameInstanceSubsystem();

	virtual UWorld* GetWorld() const override final;

protected:
	uint8 bInitialized:1;

private:
	// Middle function Begin
	void PostWorldInitialization(UWorld* World, const UWorld::InitializationValues IVS);
	void StartGameInstance(UGameInstance* GameInstance);
	// Middle function End
public:
	// UGameInstanceSubsystem implementation Begin
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// UGameInstanceSubsystem implementation End
protected:
	// Blueprint functions Begin

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
	 * @param Outer - The outer of UBPable_LocalPlayerSubsystem should be a UGameInstance
	 * @return - The return value determines whether to create the Subsystem
	 */
	UFUNCTION(BlueprintNativeEvent, DisplayName="ShouldCreateSubsystem")
	bool ReceiveShouldCreateSubsystem(UObject* Outer) const;

public:
	/**Return Outer of this Subsystem*/
	UFUNCTION(BlueprintPure, Category="GameInstanceSubsystem", meta=(KeyWords="Get"))
	UGameInstance* GetGameInstance() const { return Super::GetGameInstance(); }

	// Blueprint functions End
};
