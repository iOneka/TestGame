// 

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/SaveGame.h"
#include "SaveGameComponent.generated.h"

class USavedConstructions;
class ASomewhereGameMode;
class USavedPlayers;
class USavedVehicles;
DECLARE_LOG_CATEGORY_EXTERN(LogSave, Log, All);

class AProp;
class USavedProps;

UCLASS(meta=(BlueprintSpawnableComponent))
class SOMEWHERE_API USaveGameComponent : public UActorComponent
{
	GENERATED_BODY()

	USaveGameComponent();

	virtual void BeginPlay() override;

protected:
	UPROPERTY()
	ASomewhereGameMode* SomewhereGameMode;
	
	FTimerHandle PeriodicTimerHandle;
	void PeriodicSave();

	/*
	 * Props
	 */
	UPROPERTY()
	USavedProps* SavedProps;
	
	UPROPERTY()
	int WorldPropsCount = 0;
	
	UPROPERTY()
	int PlayerPropsCount = 0;

	void SaveProps();
	void OnPropsSaved(const FString& SlotName, int UserIndex, bool Success) const;
	void LoadProps(const FString& SlotName, int32 UserIndex, USaveGame* SaveGame);
	
	/*
	 * Vehicles
	 */
	UPROPERTY()
	USavedVehicles* SavedVehicles;
	
	UPROPERTY()
	int VehiclesCount = 0;
	
	void SaveVehicles();
	void OnVehiclesSaved(const FString& SlotName, int UserIndex, bool bSuccess) const;
	void LoadVehicles(const FString& SlotName, int UserIndex, USaveGame* SaveGame);
	
	/*
	 * Players
	 */
	UPROPERTY()
	USavedPlayers* SavedPlayers;

	void SavePlayers();
	void OnPlayersSaved(const FString& SlotName, int UserIndex, bool bSuccess) const;
	void LoadPlayers(const FString& SlotName, int UserIndex, USaveGame* SaveGame);
	
	/*
	 * Constructions
	 */
	UPROPERTY()
	USavedConstructions* SavedConstructions;

	void SaveConstructions();
	void OnConstructionsSaved(const FString& SlotName, int UserIndex, bool bSuccess) const;
	void LoadConstructions(const FString& SlotName, int UserIndex, USaveGame* SaveGame);
};
