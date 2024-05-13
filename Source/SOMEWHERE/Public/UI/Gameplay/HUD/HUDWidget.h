// 

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "Blueprint/UserWidget.h"
#include "Interaction/InteractionWidget.h"
#include "HUDWidget.generated.h"

/**
 * 
 */
UCLASS(DisplayName="HUD Widget")
class SOMEWHERE_API UHUDWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* ShowAnimation;

	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* HideAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HUD Widget")
	UInteractionWidget* InteractionWidget;
};
