// 

#pragma once

#include "CoreMinimal.h"
#include "UI/Gameplay/Views/View.h"
#include "InventoryView.generated.h"


UENUM(BlueprintType)
enum class EInventoryPageID : uint8
{
	Items	UMETA(DisplayName="Items"),
	Crafts	UMETA(DisplayName="Crafts"),
	Vault	UMETA(DisplayName="Vault"),
	Outfit	UMETA(DisplayName="Outfit"),
	Party	UMETA(DisplayName="Party"),
	Research	UMETA(DisplayName="Research"),
};

/**
 * 
 */
UCLASS(Abstract, HideDropdown)
class SOMEWHERE_API UInventoryView : public UView
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	UUserWidget* OpenPage(EInventoryPageID PageID);
};
