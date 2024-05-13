// 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SaveTypes.generated.h"

class AVehicle;
struct FInventoryItems;
struct FInventoryItem;

USTRUCT(BlueprintType)
struct FSavedProp
{
	GENERATED_BODY()

	UPROPERTY()
	FTransform Transform;

	UPROPERTY()
	TSoftClassPtr<AActor> Class;

	UPROPERTY()
	TArray<FInventoryItems> Items;

	UPROPERTY()
	bool bCanBeDamaged;

	UPROPERTY()
	float Health;

	UPROPERTY()
	float MaxHealth;
};

USTRUCT(BlueprintType)
struct FSavedProp_Cupboard : public FSavedProp
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FString> AuthorizedPlayers;
};

/**
 * 
 */
UCLASS()
class SOMEWHERE_API USavedProps : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TArray<FSavedProp> PlayerProps;

	UPROPERTY()
	TMap<FVector, FSavedProp> WorldProps;
};


USTRUCT(BlueprintType)
struct FSavedVehicle
{
	GENERATED_BODY()

	UPROPERTY()
	FTransform Transform;

	UPROPERTY()
	TSoftClassPtr<AVehicle> Class;

	UPROPERTY()
	TArray<FInventoryItems> Items;

	UPROPERTY()
	float Health;

	UPROPERTY()
	float MaxHealth;
};


/**
 * 
 */
UCLASS()
class SOMEWHERE_API USavedVehicles : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TArray<FSavedVehicle> Vehicles;
};


USTRUCT(BlueprintType)
struct FSavedPlayer
{
	GENERATED_BODY()

	UPROPERTY()
	FString PlayFabId;

	UPROPERTY()
	FString Username;

	UPROPERTY()
	FTransform Transform;

	UPROPERTY()
	TArray<FInventoryItems> Items;

	UPROPERTY()
	float Health;

	UPROPERTY()
	float Hunger;

	UPROPERTY()
	float Thirst;
};


/**
 * 
 */
UCLASS()
class SOMEWHERE_API USavedPlayers : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TArray<FSavedPlayer> Players;
};


USTRUCT()
struct FSavedConstruction
{
	GENERATED_BODY()
	
	UPROPERTY()
	FTransform Transform;

	UPROPERTY()
	TSoftClassPtr<AActor> Class;

	UPROPERTY()
	float Health;

	UPROPERTY()
	float MaxHealth;
};


/**
 * 
 */
UCLASS()
class SOMEWHERE_API USavedConstructions : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TArray<FSavedConstruction> Constructions;
};


