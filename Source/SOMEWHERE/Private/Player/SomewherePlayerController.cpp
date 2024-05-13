#include "Player/SomewherePlayerController.h"

#include "Framework/Application/NavigationConfig.h"
#include "Game/SomewhereGameMode.h"
#include "Library/SomewhereStatics.h"
#include "Net/UnrealNetwork.h"
#include "Player/SomewhereCharacter.h"
#include "Player/SomewherePlayerState.h"

ASomewherePlayerController::ASomewherePlayerController(): bIsReady(false),
                                                          LinkedSomewhereCharacter(nullptr),
                                                          GameplayWidget(nullptr), HUDWidget(nullptr)
{
}

void ASomewherePlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalPlayerController())
	{
		TSharedRef<FNavigationConfig> Navigation = MakeShared<FNavigationConfig>();
		// Navigation->bKeyNavigation = false;
		Navigation->bTabNavigation = false;
		// Navigation->bAnalogNavigation = false;
		FSlateApplication::Get().SetNavigationConfig(Navigation);
	}

	if (IsNetMode(NM_Standalone))
		CheckPlayerControllerReadiness();
}

void ASomewherePlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASomewherePlayerController, LinkedSomewhereCharacter)
}

void ASomewherePlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	CheckPlayerControllerReadiness();
}

void ASomewherePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (InputComponent)
	{
		InputComponent->BindTouch(IE_Pressed, this, &ASomewherePlayerController::HandleTouchInputStarted);
		InputComponent->BindTouch(IE_Repeat, this, &ASomewherePlayerController::HandleTouchInputMoved);
		InputComponent->BindTouch(IE_Released, this, &ASomewherePlayerController::HandleTouchInputEnded);
	}
}

void ASomewherePlayerController::PawnLeavingGame()
{
	SetPawn(nullptr);
}

void ASomewherePlayerController::OnNetCleanup(UNetConnection* Connection)
{
	if (GetLocalRole() == ROLE_Authority && PlayerState != NULL)
	{
		if (ASomewhereGameMode* SomewhereGameMode = Cast<ASomewhereGameMode>(GetWorld()->GetAuthGameMode()))
		{
			SomewhereGameMode->PreLogout(this);
		}
	}

	Super::OnNetCleanup(Connection);
}

void ASomewherePlayerController::HandleTouchInputStarted(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	OnTouchInputStarted.Broadcast(FingerIndex, Location);
}

void ASomewherePlayerController::HandleTouchInputMoved(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	OnTouchInputMoved.Broadcast(FingerIndex, Location);
}

void ASomewherePlayerController::HandleTouchInputEnded(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (Location.X != -1.0f || Location.Y != -1.0f)
	{
		OnTouchInputEnded.Broadcast(FingerIndex, Location);
	}
}

void ASomewherePlayerController::CheckPlayerControllerReadiness()
{
	if (IsValid(PlayerState))
		ServerPlayerControllerReady();
}

void ASomewherePlayerController::ServerPlayerControllerReady_Implementation()
{
	bIsReady = true;
	OnPlayerControllerReady(this);
	OnPlayerControllerReadyDelegate.Broadcast(this);
}

ASomewhereCharacter* ASomewherePlayerController::GetLinkedSomewhereCharacter() const
{
	return LinkedSomewhereCharacter;
}
ASomewherePlayerState* ASomewherePlayerController::GetSomewherePlayerState() const
{
	return Cast<ASomewherePlayerState>(PlayerState);
}

void ASomewherePlayerController::RestartPlayer_Implementation()
{
	USomewhereStatics::GetSomewhereGameMode(this)->RestartPlayer(this);
}

void ASomewherePlayerController::ServerRestartPlayerDa()
{
	LinkedSomewhereCharacter = nullptr;
	RestartPlayer();
}
