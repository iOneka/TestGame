// 


#include "UI/Gameplay/HUD/Interaction/InteractionWidget.h"

UInteractionPrompt* UInteractionWidget::AddPrompt_Implementation(const TSoftObjectPtr<UTexture2D>& PromptIcon, const FText& PromptText)
{
	return nullptr;
}

bool UInteractionWidget::RemovePrompt_Implementation(UInteractionPrompt* Prompt)
{
	return true;
}
