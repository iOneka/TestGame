// 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "PlayerSave.generated.h"

struct FInventoryItem;


USTRUCT()
struct FHealthStats
{
	GENERATED_BODY()

	float Health;
	float Hunger;
	float Thirst;
};

/**
 * 
 */
UCLASS()
class SOMEWHERE_API UPlayerSave : public USaveGame
{
	GENERATED_BODY()

public:
	FHealthStats HealthStats;
	TArray<FInventoryItem> Items;
};
