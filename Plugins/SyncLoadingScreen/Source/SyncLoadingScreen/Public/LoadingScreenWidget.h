// 

#pragma once

#include "CoreMinimal.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "LoadingScreenWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract, HideDropdown)
class SYNCLOADINGSCREEN_API ULoadingScreenWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	FName LevelName;
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnPreTransition();
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnPostTransition(bool CustomBool, int CustomInt, const FString& CustomString);
};
