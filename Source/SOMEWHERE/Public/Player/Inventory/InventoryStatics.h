// 

#pragma once

#include "CoreMinimal.h"
#include "InventoryTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InventoryStatics.generated.h"


class UItemContainer;
/**
 * 
 */
UCLASS()
class SOMEWHERE_API UInventoryStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

protected:
	UFUNCTION()
	static UDataTable* GetItemsDataTable();

public:
	/**
	 * Checks if a given inventory item is valid.
	 * An item is considered valid if its ID is not None, its amount is greater than 0,
	 * and its definition exists and is valid.
	 * 
	 * @param Item The inventory item to be checked.
	 * @return true if the item is valid, false otherwise.
	 */
	UFUNCTION(BlueprintPure)
	static bool IsValid(FInventoryItem Item);

	/**
	 * Retrieves the definition of an item given its ID. If definition not found editor returns empty structure,
	 * or will crash in runtime.
	 * 
	 * @param ID The identifier of the item whose definition is to be retrieved.
	 * @return The definition of the specified item
	 */
	UFUNCTION(BlueprintPure)
	static FItemDefinition GetItemDefinition(FName ID);

	/**
	 * Retrieves the definition of an item given its ID. If definition not found editor returns empty structure,
	 * or will crash in runtime.
	 * 
	 * @param Item The inventory item whose definition is to be retrieved.
	 * @return The definition of the specified item
	 */
	UFUNCTION(BlueprintPure)
	static FItemDefinition GetItemDefinitionFromItem(FInventoryItem Item);

	/**
	 * Generates an inventory item with random item ID and Amount clamped between 0 and Stack
	 */
	UFUNCTION(BlueprintPure)
	static FInventoryItem GetRandomInventoryItem();

	/**
	 * Retrieves a random item definition from the data table.
	 */
	UFUNCTION(BlueprintPure)
	static FItemDefinition GetRandomItemDefinition();

	UFUNCTION(BlueprintPure)
	static FItemProperties_Food GetFoodProperties(FItemDefinition ItemDefinition);

	UFUNCTION(BlueprintPure)
	static FItemProperties_Prop GetPropProperties(FItemDefinition ItemDefinition);

	UFUNCTION(BlueprintPure)
	static FItemProperties_Weapon GetWeaponProperties(FItemDefinition ItemDefinition);

	UFUNCTION(BlueprintPure)
	static FItemProperties_Armor GetArmorProperties(FItemDefinition ItemDefinition);
	

	UFUNCTION(BlueprintCallable, Category="Item Container")
	static void DebugPrintContainer(UItemContainer* Container);
};
