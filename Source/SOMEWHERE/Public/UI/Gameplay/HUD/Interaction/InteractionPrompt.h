// 

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UObject/Object.h"
#include "InteractionPrompt.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTestDelegate);

/**
 * 
 */
UCLASS(Abstract)
class SOMEWHERE_API UInteractionPrompt : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FTestDelegate OnPromptPressed;
};
