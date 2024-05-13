// 


#include "Save/SaveGameComponent.h"

#include "Game/SomewhereGameMode.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Library/PlatformStatics.h"
#include "Objects/Construction.h"
#include "Objects/Cupboard.h"
#include "Objects/Prop.h"
#include "Objects/Vehicle.h"
#include "Player/SomewhereCharacter.h"
#include "Player/Health/HealthComponent.h"
#include "Save/SaveTypes.h"

DEFINE_LOG_CATEGORY(LogSave);

class AProp;

USaveGameComponent::USaveGameComponent(): SomewhereGameMode(nullptr),
                                          SavedProps(nullptr),
                                          SavedVehicles(nullptr),
                                          SavedPlayers(nullptr),
                                          SavedConstructions(nullptr)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
}

void USaveGameComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetNetMode() != NM_DedicatedServer || UPlatformStatics::IsEditor())
		return;

	SomewhereGameMode = Cast<ASomewhereGameMode>(UGameplayStatics::GetGameMode(this));
	check(SomewhereGameMode)

	// Load props
	FAsyncLoadGameFromSlotDelegate PropsLoadedDelegate;
	UE_LOG(LogSave, Log, TEXT("Loading saved props..."))
	PropsLoadedDelegate.BindUObject(this, &USaveGameComponent::LoadProps);
	UGameplayStatics::AsyncLoadGameFromSlot(TEXT("props"), 0, PropsLoadedDelegate);

	// Load vehicles
	FAsyncLoadGameFromSlotDelegate VehiclesLoadedDelegate;
	UE_LOG(LogSave, Log, TEXT("Loading saved vehicles..."))
	VehiclesLoadedDelegate.BindUObject(this, &USaveGameComponent::LoadVehicles);
	UGameplayStatics::AsyncLoadGameFromSlot(TEXT("vehicles"), 0, VehiclesLoadedDelegate);

	// Load players
	FAsyncLoadGameFromSlotDelegate PlayersLoadedDelegate;
	UE_LOG(LogSave, Log, TEXT("Loading saved players..."))
	PlayersLoadedDelegate.BindUObject(this, &USaveGameComponent::LoadPlayers);
	UGameplayStatics::AsyncLoadGameFromSlot(TEXT("players"), 0, PlayersLoadedDelegate);

	// Load constructions
	FAsyncLoadGameFromSlotDelegate ConstructionsLoadedDelegate;
	UE_LOG(LogSave, Log, TEXT("Loading saved constructions..."))
	ConstructionsLoadedDelegate.BindUObject(this, &USaveGameComponent::LoadConstructions);
	UGameplayStatics::AsyncLoadGameFromSlot(TEXT("constructions"), 0, ConstructionsLoadedDelegate);


	// Periodic timer
	GetWorld()->GetTimerManager().SetTimer(PeriodicTimerHandle, this, &USaveGameComponent::PeriodicSave, 10, true);
}

void USaveGameComponent::PeriodicSave()
{
	UE_LOG(LogSave, Log, TEXT("Periodic save started..."))

	SaveProps();
	SaveVehicles();
	SavePlayers();
	SaveConstructions();
}

void USaveGameComponent::SaveProps()
{
	UE_LOG(LogSave, Log, TEXT("Saving props..."))

	// Создаём новый объект сохранения, если его нету
	if (!IsValid(SavedProps))
		SavedProps = Cast<USavedProps>(UGameplayStatics::CreateSaveGameObject(USavedProps::StaticClass()));

	// Удаляем старые данные
	SavedProps->PlayerProps.Empty();
	SavedProps->WorldProps.Empty();

	// Ищем все пропы на карте
	TArray<AActor*> FoundProps;
	UGameplayStatics::GetAllActorsOfClass(this, AProp::StaticClass(), FoundProps);
	UE_LOG(LogSave, Log, TEXT("Saving props...(found %d props)"), FoundProps.Num())

	WorldPropsCount = 0;
	PlayerPropsCount = 0;

	for (const auto Actor : FoundProps)
	{
		AProp* Prop = Cast<AProp>(Actor);

		FSavedProp SavedProp = FSavedProp();

		SavedProp.Transform = Prop->GetActorTransform();
		SavedProp.Class = Prop->GetClass();
		SavedProp.Items = ISavableActor::Execute_GetItemsToSave(Prop);

		SavedProp.bCanBeDamaged = Prop->CanBeDamaged();
		SavedProp.Health = Prop->Health;
		SavedProp.MaxHealth = Prop->MaxHealth;

		// Записываем пропы, которые поставили игроки
		if (Prop->bRuntimeSpawned)
		{
			if (Prop->IsA(ACupboard::StaticClass()))
			{
				ACupboard* Cupboard = Cast<ACupboard>(Prop);

				FSavedProp_Cupboard SavedProp_Cupboard{SavedProp};
				SavedProp_Cupboard.AuthorizedPlayers = Cupboard->AuthorizedPlayers;

				SavedProps->PlayerProps.Add(SavedProp_Cupboard);
			}
			else
			{
				SavedProps->PlayerProps.Add(SavedProp);
			}

			PlayerPropsCount++;
		}
		// Записываем пропы, которые изначально были на карте
		else
		{
			const FVector Location = Prop->GetActorLocation();
			SavedProps->WorldProps.Add(
				FVector(
					FMath::RoundToInt(Location.X),
					FMath::RoundToInt(Location.Y),
					FMath::RoundToInt(Location.Z)
				),
				SavedProp);

			WorldPropsCount++;
		}
	}

	// Сохраняем
	FAsyncSaveGameToSlotDelegate AsyncSaveGameToSlotDelegate;
	AsyncSaveGameToSlotDelegate.BindUObject(this, &USaveGameComponent::OnPropsSaved);
	UGameplayStatics::AsyncSaveGameToSlot(SavedProps, TEXT("props"), 0, AsyncSaveGameToSlotDelegate);
}

void USaveGameComponent::OnPropsSaved(const FString& SlotName, int UserIndex, const bool Success) const
{
	if (Success)
		UE_LOG(LogSave, Log, TEXT("Saving props...OK (%d world props, %d player props)"), WorldPropsCount,
	       PlayerPropsCount)
	else
		UE_LOG(LogSave, Error, TEXT("Saving props...ERROR (hui znayet, ne great success)"))
}


void USaveGameComponent::LoadProps(const FString& SlotName, const int32 UserIndex, USaveGame* SaveGame)
{
	if (!IsValid(SaveGame))
	{
		UE_LOG(LogSave, Warning,
		       TEXT("Loading saved props...NOT OK (Cannot load SavedProps object. Maybe save file does not exist)"))
		return;
	}

	SavedProps = Cast<USavedProps>(SaveGame);
	UE_LOG(LogSave, Log, TEXT("Loading saved props...OK"))

	// World пропы
	TArray<AActor*> FoundProps;
	UGameplayStatics::GetAllActorsOfClass(this, AProp::StaticClass(), FoundProps);

	UE_LOG(LogSave, Log, TEXT("Spawning props..."))
	UE_LOG(LogSave, Log, TEXT("Spawning props...(%d props pending)"),
	       SavedProps->WorldProps.Num() + SavedProps->PlayerProps.Num())
	WorldPropsCount = 0;
	PlayerPropsCount = 0;

	for (const auto Actor : FoundProps)
	{
		AProp* ExistingProp = Cast<AProp>(Actor);

		// По идее USaveGameComponent::LoadProps вызывается на старте сервера,
		// где кроме world пропов ничего нет. На всякий проверяем мб, хз
		if (ExistingProp->bRuntimeSpawned)
			continue;

		// Грубое местоположение world пропа
		const FVector Location = Actor->GetActorLocation();
		FVector RoundedLocation = FVector(
			FMath::RoundToInt(Location.X),
			FMath::RoundToInt(Location.Y),
			FMath::RoundToInt(Location.Z)
		);

		// Есть ли какое нибудь сохранение по этой локации для этого пропа?
		const FSavedProp* SavedWorldProp = SavedProps->WorldProps.Find(RoundedLocation);
		if (!SavedWorldProp || Actor->GetClass()->GetPathName() != SavedWorldProp->Class->GetPathName())
			continue;


		ExistingProp->SetCanBeDamaged(SavedWorldProp->bCanBeDamaged);
		ExistingProp->Health = SavedWorldProp->Health;
		ExistingProp->MaxHealth = SavedWorldProp->MaxHealth;
		ISavableActor::Execute_LoadSavedItems(ExistingProp, SavedWorldProp->Items);

		WorldPropsCount++;
	}

	// Player пропы 
	for (auto PlayerProp : SavedProps->PlayerProps)
	{
		TSubclassOf<AProp> PropClass = PlayerProp.Class.LoadSynchronous();
		AProp* Prop = GetWorld()->SpawnActorDeferred<AProp>(PropClass, PlayerProp.Transform);

		Prop->SetCanBeDamaged(PlayerProp.bCanBeDamaged);
		Prop->Health = PlayerProp.Health;
		Prop->MaxHealth = PlayerProp.MaxHealth;
		Prop->bRuntimeSpawned = true;

		UGameplayStatics::FinishSpawningActor(Prop, PlayerProp.Transform);

		ISavableActor::Execute_LoadSavedItems(Prop, PlayerProp.Items);

		PlayerPropsCount++;
	}
	UE_LOG(LogSave, Log, TEXT("Spawning props...OK (%d world props, %d player props)"), WorldPropsCount,
	       PlayerPropsCount)
}

void USaveGameComponent::SaveVehicles()
{
	UE_LOG(LogSave, Log, TEXT("Saving vehicles..."))

	// Создаём новый объект сохранения, если его нету
	if (!IsValid(SavedVehicles))
		SavedVehicles = Cast<USavedVehicles>(UGameplayStatics::CreateSaveGameObject(USavedVehicles::StaticClass()));

	// Удаляем старые данные
	SavedVehicles->Vehicles.Empty();

	// Ищем все машинки на карте
	TArray<AActor*> FoundVehicles;
	UGameplayStatics::GetAllActorsOfClass(this, AVehicle::StaticClass(), FoundVehicles);
	UE_LOG(LogSave, Log, TEXT("Saving vehicles...(found %d vehicles)"), FoundVehicles.Num())

	VehiclesCount = 0;
	for (const auto Actor : FoundVehicles)
	{
		AVehicle* Vehicle = Cast<AVehicle>(Actor);

		FSavedVehicle SavedVehicle = FSavedVehicle();

		SavedVehicle.Transform = Vehicle->GetActorTransform();
		SavedVehicle.Class = Vehicle->GetClass();
		SavedVehicle.Items = ISavableActor::Execute_GetItemsToSave(Vehicle);

		SavedVehicle.Health = Vehicle->Health;
		SavedVehicle.MaxHealth = Vehicle->MaxHealth;

		SavedVehicles->Vehicles.Add(SavedVehicle);
		VehiclesCount++;
	}

	// Сохраняем
	FAsyncSaveGameToSlotDelegate AsyncSaveGameToSlotDelegate;
	AsyncSaveGameToSlotDelegate.BindUObject(this, &USaveGameComponent::OnVehiclesSaved);
	UGameplayStatics::AsyncSaveGameToSlot(SavedVehicles, TEXT("vehicles"), 0, AsyncSaveGameToSlotDelegate);
}

void USaveGameComponent::OnVehiclesSaved(const FString& SlotName, int UserIndex, const bool bSuccess) const
{
	if (bSuccess)
		UE_LOG(LogSave, Log, TEXT("Saving vehicles...OK (%d vehicles)"), VehiclesCount)
	else
		UE_LOG(LogSave, Error, TEXT("Saving vehicles...ERROR (hui znayet, ne great success)"))
}


void USaveGameComponent::LoadVehicles(const FString& SlotName, int UserIndex, USaveGame* SaveGame)
{
	if (!IsValid(SaveGame))
	{
		UE_LOG(LogSave, Warning,
		       TEXT("Loading saved vehicles...NOT OK (Cannot load SavedVehicles object. Maybe save file does not exist)"
		       ))
		return;
	}

	SavedVehicles = Cast<USavedVehicles>(SaveGame);
	UE_LOG(LogSave, Log, TEXT("Loading saved vehicles...OK"))

	// Если есть сейв, то можно удалить все существующие машины на карте
	TArray<AActor*> FoundVehicles;
	UGameplayStatics::GetAllActorsOfClass(this, AVehicle::StaticClass(), FoundVehicles);
	for (AActor* FoundVehicle : FoundVehicles)
		GetWorld()->DestroyActor(FoundVehicle);

	UE_LOG(LogSave, Log, TEXT("Spawning vehicles..."))
	UE_LOG(LogSave, Log, TEXT("Spawning vehicles...(%d pending)"), SavedVehicles->Vehicles.Num())

	// Спавним машинки
	for (auto SavedVehicle : SavedVehicles->Vehicles)
	{
		TSubclassOf<AVehicle> VehicleClass = SavedVehicle.Class.LoadSynchronous();
		AVehicle* Vehicle = GetWorld()->SpawnActorDeferred<AVehicle>(VehicleClass, SavedVehicle.Transform);

		Vehicle->Health = SavedVehicle.Health;
		Vehicle->MaxHealth = SavedVehicle.MaxHealth;

		UGameplayStatics::FinishSpawningActor(Vehicle, SavedVehicle.Transform);

		ISavableActor::Execute_LoadSavedItems(Vehicle, SavedVehicle.Items);
	}
	UE_LOG(LogSave, Log, TEXT("Spawning vehicles...OK"))
}

void USaveGameComponent::SavePlayers()
{
	UE_LOG(LogSave, Log, TEXT("Saving players..."))

	// Создаём новый объект сохранения, если его нету
	if (!IsValid(SavedPlayers))
		SavedPlayers = Cast<USavedPlayers>(UGameplayStatics::CreateSaveGameObject(USavedPlayers::StaticClass()));

	// Удаляем старые данные
	SavedPlayers->Players.Empty();

	// Ищем всех игроков на карте
	TArray<AActor*> FoundPlayers;
	UGameplayStatics::GetAllActorsOfClass(this, ASomewhereCharacter::StaticClass(), FoundPlayers);
	UE_LOG(LogSave, Log, TEXT("Saving players...(found %d players)"), FoundPlayers.Num())

	for (const auto Actor : FoundPlayers)
	{
		ASomewhereCharacter* SomewhereCharacter = Cast<ASomewhereCharacter>(Actor);

		FSavedPlayer SavedPlayer = FSavedPlayer();

		SavedPlayer.PlayFabId = SomewhereCharacter->PlayFabId;
		SavedPlayer.Username = SomewhereCharacter->Username;

		SavedPlayer.Transform = SomewhereCharacter->GetActorTransform();
		SavedPlayer.Items = ISavableActor::Execute_GetItemsToSave(SomewhereCharacter);

		SavedPlayer.Health = SomewhereCharacter->FindComponentByClass<UHealthComponent>()->GetHealth();
		SavedPlayer.Hunger = SomewhereCharacter->FindComponentByClass<UHealthComponent>()->GetHunger();
		SavedPlayer.Thirst = SomewhereCharacter->FindComponentByClass<UHealthComponent>()->GetThirst();

		SavedPlayers->Players.Add(SavedPlayer);
	}

	// Сохраняем
	FAsyncSaveGameToSlotDelegate AsyncSaveGameToSlotDelegate;
	AsyncSaveGameToSlotDelegate.BindUObject(this, &USaveGameComponent::OnPlayersSaved);
	UGameplayStatics::AsyncSaveGameToSlot(SavedPlayers, TEXT("players"), 0, AsyncSaveGameToSlotDelegate);
}

void USaveGameComponent::OnPlayersSaved(const FString& SlotName, int UserIndex, const bool bSuccess) const
{
	if (bSuccess)
		UE_LOG(LogSave, Log, TEXT("Saving players...OK"))
	else
		UE_LOG(LogSave, Error, TEXT("Saving players...ERROR (hui znayet, ne great success)"))
}

void USaveGameComponent::LoadPlayers(const FString& SlotName, int UserIndex, USaveGame* SaveGame)
{
	if (!IsValid(SaveGame))
	{
		UE_LOG(LogSave, Warning,
		       TEXT("Loading saved players...NOT OK (Cannot load SavedVehicles object. Maybe save file does not exist)")
		)
		return;
	}

	SavedPlayers = Cast<USavedPlayers>(SaveGame);
	UE_LOG(LogSave, Log, TEXT("Loading saved players...OK"))

	UE_LOG(LogSave, Log, TEXT("Spawning players..."))
	UE_LOG(LogSave, Log, TEXT("Spawning players...(%d pending)"), SavedPlayers->Players.Num())

	// Спавним тела
	for (auto SavedPlayer : SavedPlayers->Players)
	{
		TSubclassOf<ASomewhereCharacter> PlayerCharacterClass{SomewhereGameMode->DefaultPawnClass};
		ASomewhereCharacter* SomewhereCharacter = GetWorld()->SpawnActor<ASomewhereCharacter>(PlayerCharacterClass, SavedPlayer.Transform);

		SomewhereCharacter->PlayFabId = SavedPlayer.PlayFabId;
		SomewhereCharacter->Username = SavedPlayer.Username;

		UHealthComponent* HealthComponent = SomewhereCharacter->FindComponentByClass<UHealthComponent>();
		HealthComponent->SetHealth(SavedPlayer.Health);
		HealthComponent->SetHunger(SavedPlayer.Hunger);
		HealthComponent->SetThirst(SavedPlayer.Thirst);

		ISavableActor::Execute_LoadSavedItems(SomewhereCharacter, SavedPlayer.Items);

		SomewhereCharacter->SetSleepingState(true);

		SomewhereGameMode->SleepingBodies.Add(SomewhereCharacter->PlayFabId, SomewhereCharacter);
	}

	UE_LOG(LogSave, Log, TEXT("Spawning players...OK"))
}

void USaveGameComponent::SaveConstructions()
{
	UE_LOG(LogSave, Log, TEXT("Saving constructions..."))

	// Создаём новый объект сохранения, если его нету
	if (!IsValid(SavedConstructions))
		SavedConstructions = Cast<USavedConstructions>(UGameplayStatics::CreateSaveGameObject(USavedConstructions::StaticClass()));

	// Удаляем старые данные
	SavedConstructions->Constructions.Empty();

	// Ищем все постройки на карте
	TArray<AActor*> FoundConstructions;
	UGameplayStatics::GetAllActorsOfClass(this, AConstruction::StaticClass(), FoundConstructions);
	UE_LOG(LogSave, Log, TEXT("Saving constructions...(found %d constructions)"), FoundConstructions.Num())

	for (const auto Actor : FoundConstructions)
	{
		const AConstruction* Construction = Cast<AConstruction>(Actor);

		FSavedConstruction SavedConstruction = FSavedConstruction();

		SavedConstruction.Transform = Construction->GetActorTransform();
		SavedConstruction.Class = Construction->GetClass();
		SavedConstruction.Health = Construction->Health;
		SavedConstruction.MaxHealth = Construction->MaxHealth;

		SavedConstructions->Constructions.Add(SavedConstruction);
	}

	// Сохраняем
	FAsyncSaveGameToSlotDelegate AsyncSaveGameToSlotDelegate;
	AsyncSaveGameToSlotDelegate.BindUObject(this, &USaveGameComponent::OnConstructionsSaved);
	UGameplayStatics::AsyncSaveGameToSlot(SavedConstructions, TEXT("constructions"), 0, AsyncSaveGameToSlotDelegate);
}

void USaveGameComponent::OnConstructionsSaved(const FString& SlotName, int UserIndex, bool bSuccess) const
{
	if (bSuccess)
		UE_LOG(LogSave, Log, TEXT("Saving constructions...OK"))
	else
		UE_LOG(LogSave, Error, TEXT("Saving constructions...ERROR (hui znayet, ne great success)"))
}

void USaveGameComponent::LoadConstructions(const FString& SlotName, int UserIndex, USaveGame* SaveGame)
{
	if (!IsValid(SaveGame))
	{
		UE_LOG(LogSave, Warning,
		       TEXT("Loading saved constructions...NOT OK (Cannot load SavedConstructions object. Maybe save file does not exist)"))
		return;
	}

	SavedConstructions = Cast<USavedConstructions>(SaveGame);
	UE_LOG(LogSave, Log, TEXT("Loading saved constructions...OK"))

	UE_LOG(LogSave, Log, TEXT("Spawning constructions..."))
	UE_LOG(LogSave, Log, TEXT("Spawning constructions...(%d pending)"), SavedConstructions->Constructions.Num())

	for (auto SavedConstruction : SavedConstructions->Constructions)
	{
		TSubclassOf<AConstruction> ConstructionClass = SavedConstruction.Class.LoadSynchronous();
		AConstruction* Construction = GetWorld()->SpawnActorDeferred<AConstruction>(ConstructionClass, SavedConstruction.Transform);

		Construction->Health = SavedConstruction.Health;
		Construction->MaxHealth = SavedConstruction.MaxHealth;

		UGameplayStatics::FinishSpawningActor(Construction, SavedConstruction.Transform);
	}
	UE_LOG(LogSave, Log, TEXT("Spawning constructions...OK"))
}
