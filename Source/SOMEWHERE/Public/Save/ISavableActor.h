#pragma once
#include "Player/Inventory/InventoryTypes.h"

#include "ISavableActor.generated.h"


UINTERFACE(BlueprintType)
class SOMEWHERE_API USavableActor : public UInterface
{
	GENERATED_BODY()
};

class SOMEWHERE_API ISavableActor
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, DisplayName="Get Items To Save", Category="Savable Actor Interface")
	TArray<FInventoryItems> GetItemsToSave();
	
	UFUNCTION(BlueprintNativeEvent, DisplayName="Load Saved Items", Category="Savable Actor Interface")
	bool LoadSavedItems(const TArray<FInventoryItems>& Items);
};

