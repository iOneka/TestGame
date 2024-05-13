#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Player/Inventory/InventoryTypes.h"
#include "ItemContainer.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogItemContainer, Log, All);

UCLASS(meta=(BlueprintSpawnableComponent))
class SOMEWHERE_API UItemContainer : public UActorComponent
{
	GENERATED_BODY()

	UItemContainer();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UFUNCTION()
	virtual void OnRep_ItemArray();
	
	UPROPERTY(ReplicatedUsing=OnRep_ItemArray)
	TArray<FInventoryItem> ItemArray;

	
	// ---------------------------------
	
	/** 
	 * Initializes the container with a specified size.
	 * 
	 * @param Size The number of cells in the container.
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Item Container")
	void Initialize(int Size);
	
	/**
	 * Initializes the container with a set of items.
	 *
	 * @param Items An array of items to populate the container with.
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Item Container")
	void InitializeWithItems(UPARAM(ref) TArray<FInventoryItem>& Items);
	
	/**
	 * Initializes the container with a set of items.
	 * If items in save not valid, initializes with a fallback size.
	 *
	 * @param Items An array of items to populate the container with.
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Item Container")
	void ReinitializeFromSave(FInventoryItems Items);

	// ---------------------------------
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Item Container")
	FInventoryItems ReinitializeWithItemsAndCellSize(FInventoryItems Items,int CellSize, UItemContainer* InventoryContainer);
	/**
	 * Checks if a cell index is valid.
	 *
	 * @param Cell The index of the cell.
	 */
	UFUNCTION(BlueprintPure, Category="Item Container")
	bool IsValidCellIndex(int Cell) const;
	
	/**
	 * Retrieves the item in a specified cell.
	 *
	 * @param Cell The index of the cell.
	 * @param Item The item found in the cell, if any.
	 * @return true if a valid item is found, false otherwise.
	 */
	UFUNCTION(BlueprintPure, Category="Item Container")
	bool GetCell(int Cell, FInventoryItem& Item) const;

	/**
	 * Sets a specified cell to contain a specified item or an empty item.
	 *
	 * @param Cell The index of the cell.
	 * @param Item The item to place in the cell.
	 * @return true if the operation is successful, false otherwise.
	 */
	UFUNCTION(BlueprintCallable, Category="Item Container")
	bool SetCell(int Cell, FInventoryItem Item);

	/**
	 * Retrieves the total quantity of a specified item in the container.
	 *
	 * @param ID The identifier of the item.
	 */
	UFUNCTION(BlueprintPure, Category="Item Container")
	int GetItemAmount(FName ID);

	/**
	 * Retrieves the total size of the container.
	 */
	UFUNCTION(BlueprintPure, Category="Item Container")
	int GetSize() const;

	/**
	 * Retrieves the total size of the container minus one.
	 */
	UFUNCTION(BlueprintPure, Category="Item Container")
	int GetSizeMinusOne() const;
	
	/**
	 * Retrieves all items in the container.
	 */
	UFUNCTION(BlueprintPure, Category="Item Container")
	TArray<FInventoryItem> GetItems() const;

	/**
	 * Retrieves all valid items in the container.
	 */
	UFUNCTION(BlueprintPure, Category="Item Container")
	TArray<FInventoryItem> GetValidItems() const;

	/**
	 * Returns true if container has any valid item, false otherwise
	 */
	UFUNCTION(BlueprintPure, Category="Item Container")
	bool IsEmpty() const;

	// ---------------------------------
	
	/**
	 * Deletes all items in container (size preserved)
	 */
	UFUNCTION(BlueprintCallable, Category="Item Container")
	void Empty();
	
	/**
	 * Adds a specified item to the container.
	 *
	 * @param Item The item to be added.
	 * @return true if the item was successfully added, false otherwise.
	 */
	UFUNCTION(BlueprintCallable, Category="Item Container")
	bool AddItem(FInventoryItem Item);

	/**
	 * Adds a specified item to a specified cell in the container.
	 *
	 * @param Item The item to be added.
	 * @param Cell The index of the cell.
	 * @return true if the item was successfully added to the cell, false otherwise.
	 */
	UFUNCTION(BlueprintCallable, Category="Item Container")
	bool AddItemToCell(FInventoryItem Item, int Cell);
	
	/**
	 * Adds a specified amount of an item to a specified cell in the container.
	 *
	 * @param Cell The index of the cell.
	 * @param Amount The amount of the item to be added.
	 * @return true if the amount was successfully added to the cell, false otherwise.
	 */
	UFUNCTION(BlueprintCallable, Category="Item Container")
	bool AddItemAmountToCell(int Cell, int Amount);

	/**
	 * Removes a specified item from the container.
	 *
	 * @param Item The item to be removed.
	 * @return true if the item was successfully removed, false otherwise.
	 */
	UFUNCTION(BlueprintCallable, Category="Item Container")
	bool RemoveItem(FInventoryItem Item);

	/**
	 * Removes a specified item from a specified cell in the container.
	 *
	 * @param Item The item to be removed.
	 * @param Cell The index of the cell.
	 * @return true if the item was successfully removed from the cell, false otherwise.
	 */
	UFUNCTION(BlueprintCallable, Category="Item Container")
	bool RemoveItemFromCell(FInventoryItem Item, int Cell);

	/**
	 * Removes a specified amount of an item from a specified cell in the container.
	 *
	 * @param Cell The index of the cell.
	 * @param Amount The amount of the item to be removed.
	 * @return true if the amount was successfully removed from the cell, false otherwise.
	 */
	UFUNCTION(BlueprintCallable, Category="Item Container")
	bool RemoveItemAmountFromCell(int Cell, int Amount);

	/**
	 * Swaps items between two specified cells in the container.
	 *
	 * @param FromCell The index of the source cell.
	 * @param FromContainer The container of the source cell.
	 * @param ToCell The index of the target cell.
	 * @param ToContainer The container of the target cell.
	 * @return true if the items were successfully swapped, false otherwise.
	 */
	UFUNCTION(BlueprintCallable, Category="Item Container")
	bool SwapItems(int FromCell, UItemContainer* FromContainer, int ToCell, UItemContainer* ToContainer);

	// ---------------------------------
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Server, Reliable, Category="Item Container")
	void ServerAddItem(FInventoryItem Item);
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Server, Reliable, Category="Item Container")
	void ServerSwapItems(int FromCell, UItemContainer* FromContainer, int ToCell, UItemContainer* ToContainer);
	
	// ---------------------------------

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnContainerUpdated, UItemContainer*, Container);
	UPROPERTY(BlueprintAssignable, Category="Item Container")
	FOnContainerUpdated OnContainerUpdated;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnItemAdded, UItemContainer*, Container, FInventoryItem, Item, const TArray<int>&, ToCells);
	UPROPERTY(BlueprintAssignable, Category="Item Container")
	FOnItemAdded OnItemAdded;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnItemRemoved, UItemContainer*, Container, FInventoryItem, Item, const TArray<int>&, FromCells);
	UPROPERTY(BlueprintAssignable, Category="Item Container")
	FOnItemRemoved OnItemRemoved;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnItemsSwaped, int, FromCell, UItemContainer*, FromContainer, int, ToCell, UItemContainer*, ToContainer);
	UPROPERTY(BlueprintAssignable, Category="Item Container")
	FOnItemsSwaped OnItemsSwapped;
	
	// ---------------------------------
	
	/**
	 * Searches for the first cell containing a specified item.
	 *
	 * @param ID The identifier of the item to be searched for.
	 * @param Cell Found cell with specified item
	 * @return true if a cell containing the specified item is found, false otherwise.
	 */
	UFUNCTION(BlueprintPure, Category="Item Container")
	bool FindFirstCellWithItem(FName ID, int& Cell);

	/**
	 * Searches for all cells containing a specified item.
	 *
	 * @param ID The identifier of the item to be searched for.
	 * @param Cells Found cells with specified item
	 * @return true if any cells containing the specified item are found, false otherwise.
	 */
	UFUNCTION(BlueprintPure, Category="Item Container")
	bool FindAllCellsWithItem(FName ID, TArray<int>& Cells);

	/**
	 * Searches for the first empty cell in the container.
	 *
	 * @param EmptyCell Found empty cell.
	 * @return true if an empty cell is found, false otherwise.
	 */
	UFUNCTION(BlueprintPure, Category="Item Container")
	bool FindFirstEmptyCell(int& EmptyCell);

	/**
	 * Searches for the first empty cell in the container excluding specified cells.
	 *
	 * @param ExcludeCells An array of cells to be excluded from the search.
	 * @param EmptyCell Found empty cell.
	 * @return true if an empty cell is found, false otherwise.
	 */
	UFUNCTION(BlueprintPure, Category="Item Container")
	bool FindFirstEmptyCellExclude(UPARAM(ref) const TArray<int>& ExcludeCells, int& EmptyCell);

	/**
	 * Searches for all empty cells in the container.
	 *
	 * @param EmptyCells Found empty cells.
	 * @return true if any empty cells are found, false otherwise.
	 */
	UFUNCTION(BlueprintPure, Category="Item Container")
	bool FindAllEmptyCells(TArray<int>& EmptyCells);

	/**
	 * Searches for all empty cells in the container excluding specified cells.
	 *
	 * @param ExcludeCells An array of cells to be excluded from the search.
	 * @param EmptyCells Found empty cells.
	 * @return true if any empty cells are found, false otherwise.
	 */
	UFUNCTION(BlueprintPure, Category="Item Container")
	bool FindAllEmptyCellsExclude(UPARAM(ref) const TArray<int>& ExcludeCells, TArray<int>& EmptyCells);

	/**
	 * Searches for free space available for a specified item in the container.
	 *
	 * @param ID The identifier of the item to search free space for.
	 * @param FreeSpace Amount of specified item that can fit in container.
	 */
	UFUNCTION(BlueprintPure, Category="Item Container")
	void FindFreeSpaceForItem(FName ID, int& FreeSpace);

	/**
	 * Searches for free space available for a specified item in the container excluding specified cells.
	 *
	 * @param ID The identifier of the item to search free space for.
	 * @param ExcludeCells An array of cells to be excluded from the search.
	 * @param FreeSpace Amount of specified item that can fit in container.
	 */
	UFUNCTION(BlueprintPure, Category="Item Container")
	void FindFreeSpaceForItemExclude(FName ID, UPARAM(ref) const TArray<int>& ExcludeCells, int& FreeSpace);
};
