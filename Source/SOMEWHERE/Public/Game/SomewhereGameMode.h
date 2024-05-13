// 

#pragma once

#include "CoreMinimal.h"
#include "OnlineSessionSettings.h"
#include "GameFramework/GameModeBase.h"
#include "SomewhereGameMode.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSomewhereGameMode, Log, All);

class ASomewherePlayerController;
class ASomewhereCharacter;
class USaveGameComponent;


/**
 * 
 */
UCLASS()
class SOMEWHERE_API ASomewhereGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASomewhereGameMode();

	UPROPERTY()
	USaveGameComponent* SaveGameComponent;

	/*
	 * AGameModeBase
	 */
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	
	/*
	 * Login
	 */
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	virtual APlayerController* Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	void PreLogout(APlayerController* Exiting);
	virtual void Logout(AController* Exiting) override;

	/* Fires before OnNetCleanup of exiting PlayerController */
	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnPreLogout")
	void K2_PreLogout(APlayerController* ExitingController);
	
	
	/*
	 * Player
	*/
	UPROPERTY(BlueprintReadWrite)
	TMap<FString, ASomewhereCharacter*> SleepingBodies;
	
    virtual FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal) override;

	UFUNCTION()
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

	UFUNCTION(BlueprintNativeEvent)
	void StartPlayer(ASomewherePlayerController* NewPlayer);

	virtual void RestartPlayer(AController* NewPlayer) override;
	
	UFUNCTION(BlueprintImplementableEvent, DisplayName="RestartPlayer")
	void K2_RestartPlayer(APlayerController* NewPlayer);
	
	
	/*
	 * Session
	 */
	
	/* Default session name. Must be of value 'DEDICATED_SERVER' */
	UPROPERTY()
	FName SessionName;

	/* Current seesion settings */
	TSharedPtr<FOnlineSessionSettings> SessionSettings;

	IOnlineSubsystem* OnlineSubsystem;
	IOnlineSessionPtr OnlineSession;
	
	void CreateSession();
	void AddSessionSettingFromEnvVariable(const FString& Name, const FString& DefaultValue) const;
	void OnSessionCreated(FName Name, bool bSuccess);
	FDelegateHandle CreateSessionCompleteDelegateHandle;

	void RegisterPlayer(const APlayerController* NewPlayer) const;
	void UnregisterPlayer(const APlayerController* Exiting) const;
	
	void DestroySession() const;
};
