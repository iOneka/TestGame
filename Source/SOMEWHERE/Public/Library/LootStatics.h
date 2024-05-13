// 

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LootStatics.generated.h"


struct FInventoryItem;

USTRUCT(BlueprintType)
struct FLootItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FDataTableRowHandle Item;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Min;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Max;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	double Chance;
};


USTRUCT(BlueprintType)
struct FLoot : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FLootItem> Items;

	UPROPERTY()
	int32 Count;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 MinCount;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 MaxCount;

	UPROPERTY(BlueprintReadWrite, EditAnywhere,meta=(ClampMin="0",ClampMax="100",MakeStructureDefaultValue=100))
	int32 ChanceOfMaxCount;
	
};

/**
 * 
 */
UCLASS()
class SOMEWHERE_API ULootStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure)
	static TArray<FInventoryItem> CalculateLoot(FLoot Loot);
};
