#include "Player/SomewhereCharacter.h"

#include "GameplayAbilitySystem/SomewhereAbilitySystemComponent.h"
#include "Library/SomewhereStatics.h"
#include "Net/UnrealNetwork.h"
#include "Player/Health/HealthComponent.h"
#include "Player/SomewherePlayerController.h"
#include "Player/SomewherePlayerState.h"

ASomewhereCharacter::ASomewhereCharacter()
{
	PrimaryActorTick.bCanEverTick = true;


	AbilitySystemComponent = CreateDefaultSubobject<USomewhereAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
}

void ASomewhereCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	check(AbilitySystemComponent)

	// Initialize AbilitySystemComponent
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	// Health Attribute Set
	HealthAttributeSet = NewObject<UHealthAttributeSet>(AbilitySystemComponent->GetOwner());
	AbilitySystemComponent->AddAttributeSetSubobject(HealthAttributeSet);
	
	// Movement Attribute Set
	MovementAttributeSet = NewObject<UMovementAttributeSet>(AbilitySystemComponent->GetOwner());
	AbilitySystemComponent->AddAttributeSetSubobject(MovementAttributeSet);


	// Initialize components that require attribute sets
	if (UHealthComponent* HealthComponent = FindComponentByClass<UHealthComponent>())
	{
		HealthComponent->InitializeHealthComponent(AbilitySystemComponent, HealthAttributeSet);
	}
}

void ASomewhereCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ASomewhereCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASomewhereCharacter, bIsSleeping)
	DOREPLIFETIME(ASomewhereCharacter, PlayFabId)
	DOREPLIFETIME(ASomewhereCharacter, Username)
}

void ASomewhereCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	UE_LOG(LogTemp, Log, TEXT("ASomewhereCharacter::NotifyControllerChanged"))

	ASomewherePlayerController* SomewherePlayerController = Cast<ASomewherePlayerController>(Controller);
	if(!IsValid(SomewherePlayerController))
		return;
	
	UE_LOG(LogTemp, Log, TEXT("ASomewhereCharacter::NotifyControllerChanged - is valid checked"))
	
	// Fire OnPlayerLinked events
	if(!SomewherePlayerController->LinkedSomewhereCharacter)
	{
		if(GetPlayerState())
			PlayFabId = SomewherePlayerController->GetSomewherePlayerState()->PlayFabID;
		
		UE_LOG(LogTemp, Log, TEXT("ASomewhereCharacter::NotifyControllerChanged - Fire OnPlayerLinked events"))
		SomewherePlayerController->LinkedSomewhereCharacter = this;
		SomewherePlayerController->OnPlayerCharacterLinked(this);
		SomewherePlayerController->OnPlayerCharacterLinkedDelegate.Broadcast(this);

		OnPlayerControllerLinked(SomewherePlayerController);
		OnPlayerControllerLinkedDelegate.Broadcast(SomewherePlayerController);
	}

	// Fire OnPlayerPossessed & OnPlayerUnpossessed events
	if(IsValid(SomewherePlayerController))
	{
		UE_LOG(LogTemp, Log, TEXT("ASomewhereCharacter::NotifyControllerChanged - Fire OnPlayerPossessed events"))
		OnPlayerPossessed(SomewherePlayerController);
		OnPlayerPossessedDelegate.Broadcast(SomewherePlayerController);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("ASomewhereCharacter::NotifyControllerChanged - Fire OnPlayerUnpossessed events"))
		OnPlayerUnpossessed();
		OnPlayerUnpossessedDelegate.Broadcast();
	}
}

void ASomewhereCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	AbilitySystemComponent->RefreshAbilityActorInfo();
}

void ASomewhereCharacter::UnPossessed()
{
	Super::UnPossessed();
}

void ASomewhereCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	AbilitySystemComponent->RefreshAbilityActorInfo();
}

void ASomewhereCharacter::OnRep_Controller()
{
	Super::OnRep_Controller();
	AbilitySystemComponent->RefreshAbilityActorInfo();
}

void ASomewhereCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

TArray<FInventoryItems> ASomewhereCharacter::GetItemsToSave_Implementation()
{
	return ISavableActor::GetItemsToSave_Implementation();
}

bool ASomewhereCharacter::LoadSavedItems_Implementation(const TArray<FInventoryItems>& ContainerPayloads)
{
	return ISavableActor::LoadSavedItems_Implementation(ContainerPayloads);
}

ASomewherePlayerController* ASomewhereCharacter::GetSomewherePlayerController() const
{
	return Cast<ASomewherePlayerController>(Controller);
}

ASomewherePlayerState* ASomewhereCharacter::GetSomewherePlayerState() const
{
	return Cast<ASomewherePlayerState>(GetPlayerState());
}

void ASomewhereCharacter::SetSleepingState(const bool Sleeping)
{
	if(bIsSleeping == Sleeping)
		return;
	bIsSleeping = Sleeping;
	
	if(bIsSleeping)
	{
		OnPlayerWentToSleep();
		OnPlayerWentToSleepDelegate.Broadcast();
	}
	else
	{
		OnPlayerWokeUp();
		OnPlayerWokeUpDelegate.Broadcast();
	}
}

void ASomewhereCharacter::OnRep_IsSleeping()
{
	if(bIsSleeping)
	{
		OnPlayerWentToSleep();
		OnPlayerWentToSleepDelegate.Broadcast();
	}
	else
	{
		OnPlayerWokeUp();
		OnPlayerWokeUpDelegate.Broadcast();
	}
}

UAbilitySystemComponent* ASomewhereCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
