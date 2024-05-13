// 

#pragma once

#include "CoreMinimal.h"
#include "InteractionPrompt.h"
#include "Blueprint/UserWidget.h"
#include "InteractionWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class SOMEWHERE_API UInteractionWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, DisplayName="Add Prompt")
	UInteractionPrompt* AddPrompt(const TSoftObjectPtr<UTexture2D>& PromptIcon, const FText& PromptText);

	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, DisplayName="Remove Prompt")
	bool RemovePrompt(UInteractionPrompt* Prompt);
};
