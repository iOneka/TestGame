
#pragma once

#include "CoreMinimal.h"
#include "ControlSchemeWidget.h"
#include "Engine/DataTable.h"
#include "InputMappingContext.h"
#include "FMovementSchemeMap.generated.h"


USTRUCT(BlueprintType)
struct FMovementSchemeMap : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Scheme Widget"))
	TSoftClassPtr<UControlSchemeWidget> SchemeWidget;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Mapping Context"))
	UInputMappingContext* MappingContext;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Move Action"))
	UInputAction* MoveAction;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Look Action"))
	UInputAction* LookAction;
};
