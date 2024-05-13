// 

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <Player/Crafting/CraftingTypes.h>
#include "CraftsPage.generated.h"

/**
 * 
 */
UCLASS()
class SOMEWHERE_API UCraftsPage : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool SetCraftingLevel(ECraftingLevel CraftingLevel);
};
