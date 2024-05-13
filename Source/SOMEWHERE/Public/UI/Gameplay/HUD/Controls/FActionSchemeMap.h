
#pragma once

#include "CoreMinimal.h"
#include "ControlSchemeWidget.h"
#include "Engine/DataTable.h"
#include "InputMappingContext.h"
#include "FActionSchemeMap.generated.h"


USTRUCT(BlueprintType)
struct FActionSchemeMap : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Scheme Widget"))
	TSoftClassPtr<UControlSchemeWidget> SchemeWidget;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Mapping Context"))
	UInputMappingContext* MappingContext;
};
