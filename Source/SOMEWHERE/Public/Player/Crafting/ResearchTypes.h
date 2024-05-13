//

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ResearchTypes.generated.h"

USTRUCT(BlueprintType)
struct FResearchItemDefinition
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FDataTableRowHandle Item;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Amount;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	FDataTableRowHandle CraftUnlock;
};



USTRUCT(BlueprintType)
struct FResearchItem : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Row;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Column;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FResearchItemDefinition> ItemsInRow;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 UnlockedCount;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 ReqUnlockedCount;
	
};







