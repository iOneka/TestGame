// 

#include "Game/SomewhereGameMode.h"

#include "OnlineSessionSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Player/SomewherePlayerState.h"
#include "Player/SomewherePlayerController.h"
#include "Save/SaveGameComponent.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Library/PlatformStatics.h"

DEFINE_LOG_CATEGORY(LogSomewhereGameMode);

ASomewhereGameMode::ASomewhereGameMode()
{
	SaveGameComponent = CreateDefaultSubobject<USaveGameComponent>(TEXT("SaveGameComponent"));
	SessionName = FName(TEXT("DEDICATED_SERVER"));
}

void ASomewhereGameMode::BeginPlay()
{
	UE_LOG(LogSomewhereGameMode, Log, TEXT("BeginPlay()"))
	
	Super::BeginPlay();

	CreateSession();
}

void ASomewhereGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UE_LOG(LogSomewhereGameMode, Log, TEXT("EndPlay()"))
	UE_LOG(LogSomewhereGameMode, Log, TEXT("EndPlay(EndPlayReason: %s)"), *UEnum::GetValueAsString(EndPlayReason))
	
	Super::EndPlay(EndPlayReason);

	DestroySession();
}

void ASomewhereGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	UE_LOG(LogSomewhereGameMode, Log, TEXT("PreLogin()"))
	UE_LOG(LogSomewhereGameMode, Log, TEXT("PreLogin(Options: %s, Address: %s, UniqueId: %s)"), *Options, *Address, *UniqueId.ToString())
	
	// Проверяем PlayFabID у игрока. Если сервак тестовый, то пох+пох
	if(FApp::GetBuildConfiguration() == EBuildConfiguration::Shipping)
	{
		// Наличие опции в url
		if(!UGameplayStatics::HasOption(Options, TEXT("pfid")))
		{
			ErrorMessage = FString(TEXT("Player has no PlayFab ID"));
			UE_LOG(LogSomewhereGameMode, Error, TEXT("PreLogin() - Player has no PlayFabID"));
			UE_LOG(LogSomewhereGameMode, Error, TEXT("PreLogin() - (Options: %s, Address: %s, UniqueId: %s"), *Options, *Address, *UniqueId.ToString());
			return;
		}

		// Формат (PlayFab EntityId is a hex-encoded number with a length of 15-16 digits,
		// it should consist of letters from A to F and numbers from 0 to 9.)
		const FRegexPattern Pattern(TEXT("^[A-Fa-f0-9]{15,16}$"));
		const FString PlayFabId = UGameplayStatics::ParseOption(Options, TEXT("pfid"));
		
		FRegexMatcher Matcher(Pattern, PlayFabId);
		if (!Matcher.FindNext())
		{
			ErrorMessage = FString(TEXT("Player has invalid PlayFab ID"));
			UE_LOG(LogSomewhereGameMode, Error, TEXT("PreLogin() - Player has invalid PlayFab ID"));
			UE_LOG(LogSomewhereGameMode, Error, TEXT("PreLogin() - (Options: %s, Address: %s, UniqueId: %s, PlayFabID: %s"), *Options, *Address, *UniqueId.ToString(), *PlayFabId);
			return;
		}
	}
	
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
}

APlayerController* ASomewhereGameMode::Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	UE_LOG(LogSomewhereGameMode, Log, TEXT("Login()"))
	UE_LOG(LogSomewhereGameMode, Log, TEXT("Login(NewPlayer: %s, InRemoteRole: %s, Portal: %s, Options: %s, UniqueId: %s)"),
		*NewPlayer->GetName(), *UEnum::GetValueAsString(InRemoteRole), *Portal, *Options, *UniqueId.ToString())
	
	return Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);
}

void ASomewhereGameMode::PostLogin(APlayerController* NewPlayer)
{
	UE_LOG(LogSomewhereGameMode, Log, TEXT("PostLogin()"))
	UE_LOG(LogSomewhereGameMode, Log, TEXT("PostLogin(NewPlayer: %s)"), *NewPlayer->GetName())
	
	Super::PostLogin(NewPlayer);
	
	RegisterPlayer(NewPlayer);
}

void ASomewhereGameMode::PreLogout(APlayerController* Exiting)
{
	UE_LOG(LogSomewhereGameMode, Log, TEXT("PreLogout()"))
	UE_LOG(LogSomewhereGameMode, Log, TEXT("PreLogout(Exiting: %s)"), *Exiting->GetName())

	UnregisterPlayer(Exiting);
	
	K2_PreLogout(Exiting);
}

void ASomewhereGameMode::Logout(AController* Exiting)
{
	UE_LOG(LogSomewhereGameMode, Log, TEXT("Logout()"))
	UE_LOG(LogSomewhereGameMode, Log, TEXT("Logout(Exiting: %s)"), *Exiting->GetName())
	
	Super::Logout(Exiting);
}

FString ASomewhereGameMode::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal)
{
	UE_LOG(LogSomewhereGameMode, Log, TEXT("InitNewPlayer()"))
	UE_LOG(LogSomewhereGameMode, Log, TEXT("InitNewPlayer(NewPlayerController: %s, UniqueId: %s, Options: %s, Portal: %s)"),
		*NewPlayerController->GetName(), *UniqueId.ToString(), *Options, *Portal)
	
	FString ErrorMessage = Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);
	if(!ErrorMessage.IsEmpty())
		return ErrorMessage;

	// Записываем PlayFabID в PlayerState игрока 
	ASomewherePlayerState* PlayerState = Cast<ASomewherePlayerState>(NewPlayerController->PlayerState);
	
	if(UGameplayStatics::HasOption(Options, TEXT("pfid")))
	{
		PlayerState->PlayFabID = UGameplayStatics::ParseOption(Options, TEXT("pfid"));
		
		UE_LOG(LogSomewhereGameMode, Log, TEXT("InitNewPlayer() - Initialized new player with PlayFabID: %s"), *PlayerState->PlayFabID)
		return ErrorMessage;
	}

	PlayerState->PlayFabID = FString::Printf(TEXT("DEVCONTROLLER%d"), FMath::RandRange(0, 9999));
	
	UE_LOG(LogSomewhereGameMode, Warning, TEXT("InitNewPlayer() - Initialized new player with development PlayFabID: %s"), *PlayerState->PlayFabID)
	return ErrorMessage;
}

void ASomewhereGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	ASomewherePlayerController* SomewherePlayerController = Cast<ASomewherePlayerController>(NewPlayer);
	check(SomewherePlayerController)
	
	if(SomewherePlayerController->bIsReady)
		StartPlayer(SomewherePlayerController);
	else
		SomewherePlayerController->OnPlayerControllerReadyDelegate.AddDynamic(this, &ASomewhereGameMode::StartPlayer);
}

void ASomewhereGameMode::StartPlayer_Implementation(ASomewherePlayerController* NewPlayer)
{
}

void ASomewhereGameMode::RestartPlayer(AController* NewPlayer)
{
	if (NewPlayer == nullptr || NewPlayer->IsPendingKillPending())
		return;

	ASomewherePlayerController* SomewherePlayerController = Cast<ASomewherePlayerController>(NewPlayer);
	SomewherePlayerController->LinkedSomewhereCharacter = nullptr;
	
	K2_RestartPlayer(Cast<APlayerController>(NewPlayer));
}

void ASomewhereGameMode::CreateSession()
{
	if(GetNetMode() != NM_DedicatedServer || UPlatformStatics::IsEditor())
		return;
		
	UE_LOG(LogSomewhereGameMode, Log, TEXT("CreateSession()"))
	
	OnlineSubsystem = Online::GetSubsystem(this->GetWorld());
	OnlineSession = OnlineSubsystem->GetSessionInterface();

	SessionSettings = MakeShared<FOnlineSessionSettings>();
	SessionSettings->NumPublicConnections = FCString::Atoi(
		*UPlatformStatics::GetEnvironmentVariableDefault("SERVER_MAX_PLAYERS", "100")
	);
	SessionSettings->bShouldAdvertise = true;
	SessionSettings->bAllowJoinInProgress = true;
	SessionSettings->bAllowInvites = true;
	SessionSettings->bUsesPresence = true;
	SessionSettings->bAllowJoinViaPresence = true;
	SessionSettings->bAllowJoinViaPresenceFriendsOnly = true;

	const int DefaultServerID = FMath::RandRange(1000, 9999);
	
	AddSessionSettingFromEnvVariable("SERVER_ID", FString::Printf(TEXT("my_server_%d"), DefaultServerID));
	AddSessionSettingFromEnvVariable("SERVER_NAME", FString::Printf(TEXT("My Server #%d"), DefaultServerID));
	AddSessionSettingFromEnvVariable("SERVER_DESCRIPTION", "My dedicated game server");
	AddSessionSettingFromEnvVariable("SERVER_MAX_PLAYERS", "100");
	AddSessionSettingFromEnvVariable("SERVER_ALLOW_MOBILE", "true");
	AddSessionSettingFromEnvVariable("SERVER_ALLOW_DESKTOP", "true");
	
	AddSessionSettingFromEnvVariable("BUILD_VERSION", "0.0.0.0000.0000");
	AddSessionSettingFromEnvVariable("BUILD_CONFIGURATION", LexToString(FApp::GetBuildConfiguration()));

	if(FApp::GetBuildConfiguration() == EBuildConfiguration::Development)
	{
		AddSessionSettingFromEnvVariable("CHANGESET_NUMBER", "0");
		AddSessionSettingFromEnvVariable("CHANGESET_BRANCH", "/");
		AddSessionSettingFromEnvVariable("CHANGESET_DATE", "01/01/2000 12:12:12");
	}

	CreateSessionCompleteDelegateHandle = OnlineSession->AddOnCreateSessionCompleteDelegate_Handle(
		FOnCreateSessionComplete::FDelegate::CreateUObject(this, &ASomewhereGameMode::OnSessionCreated)
	);

	if (!OnlineSession->CreateSession(0, SessionName, *SessionSettings))
	{
	}
}

void ASomewhereGameMode::AddSessionSettingFromEnvVariable(const FString& Name, const FString& DefaultValue) const
{
	SessionSettings->Settings.Add(
		FName(Name),
		FOnlineSessionSetting(
			UPlatformStatics::GetEnvironmentVariableDefault(Name, DefaultValue),
			EOnlineDataAdvertisementType::ViaOnlineService
		)
	);
}

void ASomewhereGameMode::OnSessionCreated(FName Name, const bool bSuccess)
{
	UE_LOG(LogSomewhereGameMode, Log, TEXT("OnSessionCreated()"))
	UE_LOG(LogSomewhereGameMode, Log, TEXT("OnSessionCreated(SessionName: %s, bSuccess: %hhd)"), *Name.ToString(), bSuccess)
	
	// Отписка
	OnlineSession->ClearOnCreateSessionCompleteDelegate_Handle(this->CreateSessionCompleteDelegateHandle);
	this->CreateSessionCompleteDelegateHandle.Reset();

	if(!bSuccess)
	{
		UE_LOG(LogSomewhereGameMode, Log, TEXT("Could not create a session"))
		return;
	}

	OnlineSession->StartSession(Name);
}

void ASomewhereGameMode::RegisterPlayer(const APlayerController* NewPlayer) const
{
	if(GetNetMode() != NM_DedicatedServer || UPlatformStatics::IsEditor())
		return;
		
	UE_LOG(LogSomewhereGameMode, Log, TEXT("RegisterPlayer()"))
	UE_LOG(LogSomewhereGameMode, Log, TEXT("RegisterPlayer(NewPlayer: %s)"), *NewPlayer->GetName());
	
	const UNetConnection* RemoteNetConnection = Cast<UNetConnection>(NewPlayer->Player);
	check(IsValid(RemoteNetConnection))

	const TSharedPtr<const FUniqueNetId> UniqueNetId = RemoteNetConnection->PlayerId.GetUniqueNetId();
	check(UniqueNetId != nullptr);
	
	if (!OnlineSession->RegisterPlayer(SessionName, *UniqueNetId, false))
	{
	}
}

void ASomewhereGameMode::UnregisterPlayer(const APlayerController* Exiting) const
{
	if(GetNetMode() != NM_DedicatedServer || UPlatformStatics::IsEditor())
		return;
	
	UE_LOG(LogSomewhereGameMode, Log, TEXT("UnregisterPlayer()"))
	UE_LOG(LogSomewhereGameMode, Log, TEXT("UnregisterPlayer(NewPlayer: %s)"), *Exiting->GetName());
	
	const UNetConnection* RemoteNetConnection = Cast<UNetConnection>(Exiting->Player);
	check(IsValid(RemoteNetConnection))

	const TSharedPtr<const FUniqueNetId> UniqueNetId = RemoteNetConnection->PlayerId.GetUniqueNetId();
	check(UniqueNetId != nullptr);
	
	if (!OnlineSession->UnregisterPlayer(SessionName, *UniqueNetId))
	{
	}
}

void ASomewhereGameMode::DestroySession() const
{
	if(GetNetMode() != NM_DedicatedServer || UPlatformStatics::IsEditor())
		return;
		
	if (!OnlineSession->DestroySession(SessionName))
	{
	}
}

