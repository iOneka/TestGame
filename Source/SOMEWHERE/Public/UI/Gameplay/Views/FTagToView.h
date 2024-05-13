// 

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "View.h"
#include "Engine/DataTable.h"
#include "FTagToView.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FTagToView : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Categories = "GameplayWidget.Views"))
	FGameplayTag Tag;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftClassPtr<UView> View;
};
