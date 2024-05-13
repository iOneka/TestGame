#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UI/Gameplay/GameplayWidget.h"
#include "SomewherePlayerController.generated.h"

class ASomewherePlayerState;
class ASomewhereCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTouchInputStartedSignature, ETouchIndex::Type, FingerIndex, const FVector, Location);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTouchInputMovedSignature, ETouchIndex::Type, FingerIndex, const FVector, Location);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTouchInputEndedSignature, ETouchIndex::Type, FingerIndex, const FVector, Location);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerCharacterLinkedSignature, ASomewhereCharacter*, SomewhereCharacter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerControllerReadySignature, ASomewherePlayerController*, SomewherePlayerController);


/**
 * 
 */
UCLASS()
class SOMEWHERE_API ASomewherePlayerController : public APlayerController
{
	GENERATED_BODY()
	
	ASomewherePlayerController();


	/*
	 * AActor overrides
	 */
	UFUNCTION()
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	
	/*
	 * AController overrides
	 */
	virtual void OnRep_PlayerState() override;


	/*
	 * APlayerController overrides
	 */
	virtual void SetupInputComponent() override;
	virtual void PawnLeavingGame() override;
	virtual void OnNetCleanup(UNetConnection* Connection) override;

	
	/**
	 * ASomewherePlayerController
	 */
public:
	UFUNCTION(BlueprintPure)
	ASomewherePlayerState* GetSomewherePlayerState() const;
protected:
private:

	
	/*
	 * Touch Input
	 */
public:
	UPROPERTY(BlueprintAssignable)
	FOnTouchInputStartedSignature OnTouchInputStarted;

	UPROPERTY(BlueprintAssignable)
	FOnTouchInputMovedSignature OnTouchInputMoved;

	UPROPERTY(BlueprintAssignable)
	FOnTouchInputEndedSignature OnTouchInputEnded;

protected:
	void HandleTouchInputStarted(const ETouchIndex::Type FingerIndex, const FVector Location);
	void HandleTouchInputMoved(const ETouchIndex::Type FingerIndex, const FVector Location);
	void HandleTouchInputEnded(const ETouchIndex::Type FingerIndex, const FVector Location);


	/*
	 * ASomewherePlayerController - Readiness
	 *
	 *		Когда игрок заходит на сервер до него должны успеть реплицироваться всякие классы и переменные.
	 * Если у игрока высокий пинг или мы делаем что то сразу после спавна контроллера игрока, то не факт
	 * что на клиенте нужные переменные успели инициализироваться. Поэтому проверяем на клиенте все переменные,
	 * а потом говорим серверу, что мы готовы.
	 *		(можно конечно было въебать Delay, но сука он в будущем жоОоско в колено стрельнет)
	 */
public:
	/* If Player Controller has all variables and classes replicated to him */
	UPROPERTY()
	bool bIsReady;
	
	/* Event when client's Player Controller has all variables and classes replicated to him, thus is ready */
	UFUNCTION(BlueprintImplementableEvent, BlueprintAuthorityOnly)
	void OnPlayerControllerReady(ASomewherePlayerController* SomewherePlayerController);

	/* Event when client's Player Controller has all variables and classes replicated to him, thus is ready */
	UPROPERTY(BlueprintAssignable, DisplayName="On Player Controller Ready")
	FOnPlayerControllerReadySignature OnPlayerControllerReadyDelegate;
	
protected:
	UFUNCTION()
	virtual void CheckPlayerControllerReadiness();
	
	UFUNCTION(Server, Reliable)
	virtual void ServerPlayerControllerReady();
	
private:
	
	
	/*
	 * ASomewherePlayerController - Linked character
	 *
	 *		Ссылка на тело игрока. Игрок не всегда позесит своё тело, например позесит машину и управляет ею.
	 *	Также и тело не всегда имеет контроллер игрока, например когда игрок отключён от сервера.
	 */
public:
	UPROPERTY(Replicated)
	ASomewhereCharacter* LinkedSomewhereCharacter;
	
	/* Returns linked character of this Player Controller */
	UFUNCTION(BlueprintPure)
	ASomewhereCharacter* GetLinkedSomewhereCharacter() const;

	/* Event when Player Controller possessed character for the first time */
	UFUNCTION(BlueprintImplementableEvent)
	void OnPlayerCharacterLinked(ASomewhereCharacter* SomewhereCharacter);

	/* Event when Player Controller possessed character for the first time */
	UPROPERTY(BlueprintAssignable, DisplayName="On Player Character Linked")
	FOnPlayerCharacterLinkedSignature OnPlayerCharacterLinkedDelegate;
	
protected:
private:

	
public:
	UFUNCTION(Server, Reliable)
	void RestartPlayer();
	
	UFUNCTION(BlueprintCallable)
	void ServerRestartPlayerDa();

	
	/**
	 * 
	 */
public:
	UPROPERTY(BlueprintReadWrite)
	UGameplayWidget* GameplayWidget;

	
	/**
	 * 
	 */
	UPROPERTY(BlueprintReadWrite)
	UHUDWidget* HUDWidget;

};
