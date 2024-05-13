#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "View.generated.h"

/**
 * 
 */
UCLASS(Abstract, HideDropdown)
class SOMEWHERE_API UView : public UUserWidget
{
	GENERATED_BODY()

public:
	
	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* ViewOpenAnimation;

	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* ViewCloseAnimation;

	/**
	 * Called after open animation of this view played
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void OnViewOpened();

	/**
	 * Called before close animation will play
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void OnViewClosed();
};
