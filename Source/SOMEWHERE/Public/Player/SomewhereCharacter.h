#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayAbilitySystem//AttributeSet/HealthAttributeSet.h"
#include "GameplayAbilitySystem/AttributeSet/MovementAttributeSet.h"
#include "Save/ISavableActor.h"
#include "SomewhereCharacter.generated.h"


class ASomewherePlayerState;
class USomewhereAbilitySystemComponent;
class ASomewherePlayerController;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerPossessedFirstTimeSignature, ASomewherePlayerController*, SomewherePlayerController);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerPossessedSignature, ASomewherePlayerController*, SomewherePlayerController);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerUnpossessedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerSleepSignature);


UCLASS()
class SOMEWHERE_API ASomewhereCharacter : public ACharacter, public IAbilitySystemInterface, public ISavableActor
{
	GENERATED_BODY()

	ASomewhereCharacter();


	/*
	 * AActor
	 */
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	
	/*
	 * APawn
	 */
	virtual void NotifyControllerChanged() override;

	
	/*
	 * ACharacter
	 */
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;
	virtual void OnRep_PlayerState() override;
	virtual void OnRep_Controller() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	
	/*
	 * ISavableActor
	 */
	public:
	virtual TArray<FInventoryItems> GetItemsToSave_Implementation() override;
	virtual bool LoadSavedItems_Implementation(const TArray<FInventoryItems>& ContainerPayloads) override;
	

	/*
	 * ASomewhereCharacter 
	 */
public:
	/** Somewhere Player Controller currently possessing this character */
	UFUNCTION(BlueprintCallable, Category = "Somewhere Character")
	ASomewherePlayerController* GetSomewherePlayerController() const;

	/* If Somewhere Character is possessed by a player, returns its Somewhere Player State. Needed for network play as controllers are not replicated to clients. */
	UFUNCTION(BlueprintCallable, Category = "Somewhere Character")
	ASomewherePlayerState* GetSomewherePlayerState() const;
	
	/* Event called after player possessed this character for the first time, on the server and owning client. */
	UFUNCTION(BlueprintImplementableEvent)
	void OnPlayerControllerLinked(ASomewherePlayerController* SomewherePlayerController);

	/* Event called after player possessed this character for the first time, on the server and owning client. */
	UPROPERTY(BlueprintAssignable, DisplayName="On Player Controller Linked")
	FPlayerPossessedFirstTimeSignature OnPlayerControllerLinkedDelegate;
	
	/* Event called after player possessed this character, on the server and owning client. */
	UFUNCTION(BlueprintImplementableEvent)
	void OnPlayerPossessed(ASomewherePlayerController* SomewherePlayerController);

	/* Event called after player possessed this character, on the server and owning client. */
	UPROPERTY(BlueprintAssignable, DisplayName="On Player Possessed")
	FPlayerPossessedSignature OnPlayerPossessedDelegate;
	
	/* Event called after player unpossessed this character, on the server and owning client. */
	UFUNCTION(BlueprintImplementableEvent)
	void OnPlayerUnpossessed();

	/* Event called after player unpossessed this character, on the server and owning client. */
	UPROPERTY(BlueprintAssignable, DisplayName="On Player Unpossessed")
	FPlayerUnpossessedSignature OnPlayerUnpossessedDelegate;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void SetSleepingState(bool Sleeping);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_IsSleeping, Category="Somewhere Character")
	bool bIsSleeping;

	UFUNCTION()
	void OnRep_IsSleeping();

	UFUNCTION(BlueprintImplementableEvent)
	void OnPlayerWentToSleep();

	UPROPERTY(BlueprintAssignable, DisplayName="On Player Went To Sleep")
	FPlayerSleepSignature OnPlayerWentToSleepDelegate;

	UFUNCTION(BlueprintImplementableEvent)
	void OnPlayerWokeUp();

	UPROPERTY(BlueprintAssignable, DisplayName="On Player Woke Up")
	FPlayerSleepSignature OnPlayerWokeUpDelegate;

protected:
private:

	/*
	 * Online
	 */
public:
	UPROPERTY(BlueprintReadOnly, Replicated)
	FString PlayFabId;
	
	UPROPERTY(BlueprintReadOnly, Replicated)
	FString Username;

	/*
	 * Gameplay Ability System
	 */
public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Somewhere Character")
	USomewhereAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Somewhere Character")
	UHealthAttributeSet* HealthAttributeSet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Somewhere Character")
	UMovementAttributeSet* MovementAttributeSet;
};
