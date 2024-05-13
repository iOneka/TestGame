// 

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CraftingTypes.generated.h"

USTRUCT(BlueprintType)
struct FCraftIngredient
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FDataTableRowHandle Item;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Amount;
};


USTRUCT(BlueprintType)
struct FCraft : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FDataTableRowHandle Item;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Amount;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	double Time;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FCraftIngredient> Ingredients;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float ServerStartTime;
};

UENUM(BlueprintType)
enum ECraftingLevel
{
	Local UMETA(DisplayName = "Local"),
	WorkbenchOne UMETA(DisplayName = "WorkbenchOne"),
	WorkbenchTwo UMETA(DisplayName = "WorkbenchTwo")
};



