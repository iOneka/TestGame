#include "Player/Inventory/ItemContainer.h"

#include "Net/UnrealNetwork.h"
#include "Player/Inventory/InventoryStatics.h"

DEFINE_LOG_CATEGORY(LogItemContainer);

UItemContainer::UItemContainer()
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UItemContainer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UItemContainer, ItemArray);
}

void UItemContainer::OnRep_ItemArray()
{
	OnContainerUpdated.Broadcast(this);
}

void UItemContainer::Initialize(const int Size)
{
	ItemArray.Init(FInventoryItem(), Size);
	
	if (GetNetMode() == NM_Standalone)
		OnContainerUpdated.Broadcast(this);
}

void UItemContainer::InitializeWithItems(TArray<FInventoryItem>& Items)
{
	ItemArray.Empty();
	ItemArray = Items;
	
	if (GetNetMode() == NM_Standalone)
		OnContainerUpdated.Broadcast(this);
}

void UItemContainer::ReinitializeFromSave(FInventoryItems Items)
{
	if(Items.Items.IsEmpty())
		Initialize(GetSize());

	InitializeWithItems(Items.Items);
}

FInventoryItems UItemContainer::ReinitializeWithItemsAndCellSize(FInventoryItems Items,int CellsSize, UItemContainer* InventoryContainer) {
	if (Items.Items.IsEmpty())
		Initialize(CellsSize);
	FInventoryItems ReturnItems;
	if (Items.Items.Num() > CellsSize) {

		ItemArray.Init(FInventoryItem(), CellsSize);
		for (size_t i = 0; i < CellsSize; i++)
		{
			ItemArray[i] = Items.Items[i];
		}
		for (size_t i = CellsSize;i<Items.Items.Num();i++)
		{
			if (!InventoryContainer->AddItem(Items.Items[i]))
			{
				ReturnItems.Items.Add(Items.Items[i]);
			}
		}
		
	}
	else {
		ItemArray.Init(FInventoryItem(), CellsSize);
		for (size_t i = 0; i < Items.Items.Num(); i++)
		{
			ItemArray[i] = Items.Items[i];
		}
	}

	if (GetNetMode() == NM_Standalone)
		OnContainerUpdated.Broadcast(this);
	return ReturnItems;
}

bool UItemContainer::IsValidCellIndex(const int Cell) const
{
	return ItemArray.IsValidIndex(Cell);
}

bool UItemContainer::GetCell(const int Cell, FInventoryItem& Item) const
{
	Item = FInventoryItem();

	if (!ItemArray.IsValidIndex(Cell))
		return false;

	const FInventoryItem FoundItem = ItemArray[Cell];

	if (!UInventoryStatics::IsValid(FoundItem))
		return false;

	Item = FoundItem;
	return true;
}

bool UItemContainer::SetCell(const int Cell, const FInventoryItem Item)
{
	if (!ItemArray.IsValidIndex(Cell))
		return false;

	if (UInventoryStatics::IsValid(Item))
	{
		if (Item.Amount > UInventoryStatics::GetItemDefinition(Item.ID).Stack)
			return false;

		ItemArray[Cell] = Item;
		OnContainerUpdated.Broadcast(this);
		return true;
	}

	ItemArray[Cell] = FInventoryItem();
	OnContainerUpdated.Broadcast(this);
	return true;
}

int UItemContainer::GetItemAmount(const FName ID)
{
	int Amount = 0;

	for (int i = 0; i < ItemArray.Num(); ++i)
	{
		if (const FInventoryItem& Item = ItemArray[i]; Item.ID == ID)
		{
			Amount += Item.Amount;
		}
	}
	return Amount;
}

int UItemContainer::GetSize() const
{
	return ItemArray.Num();
}

int UItemContainer::GetSizeMinusOne() const
{
	return ItemArray.Num() - 1;
}

TArray<FInventoryItem> UItemContainer::GetItems() const
{
	return ItemArray;
}

TArray<FInventoryItem> UItemContainer::GetValidItems() const
{
	TArray<FInventoryItem> ValidItems;
	for (auto& Item : ItemArray)
	{
		if (UInventoryStatics::IsValid(Item))
			ValidItems.Add(Item);
	}
	return ValidItems;
}

bool UItemContainer::IsEmpty() const
{
	for (const FInventoryItem& Item : ItemArray)
	{
		if(UInventoryStatics::IsValid(Item))
			return false;
	}
	return true;
}

void UItemContainer::Empty()
{
	for (int i = 0; i < ItemArray.Num(); ++i)
	{
		ItemArray[i] = FInventoryItem();
	}
}

bool UItemContainer::AddItem(FInventoryItem Item)
{
	if (!UInventoryStatics::IsValid(Item))
		return false;

	const FItemDefinition ItemDefinition = UInventoryStatics::GetItemDefinition(Item.ID);
	const int InitialItemAmount = Item.Amount;

	// Предмет делимый?
	if (ItemDefinition.Stack == 1)
	{
		// Нет, не делимый
		// Ищем первую пустую ячейку
		int FreeCell;
		if (!FindFirstEmptyCell(FreeCell))
			return false;

		ItemArray[FreeCell] = Item;
		OnContainerUpdated.Broadcast(this);
		OnItemAdded.Broadcast(this, Item, TArray{FreeCell});
		return true;
	}

	// Да, делимый
	// Поместится ли вообще такое количество предмета? Достаточно ли места?
	TArray<int> FoundEmptyCells;
	TArray<int> FoundCellsWithItem;
	{
		int TotalFreeSpace = 0;

		// Сколько предметов поместится в пустые ячейки
		if (FindAllEmptyCells(FoundEmptyCells))
			TotalFreeSpace = ItemDefinition.Stack * FoundEmptyCells.Num();

		// Сколько предметов поместится в неполные ячейки
		if (FindAllCellsWithItem(Item.ID, FoundCellsWithItem))
			for (const auto CellWithItem : FoundCellsWithItem)
			{
				TotalFreeSpace += ItemDefinition.Stack - ItemArray[CellWithItem].Amount;
			}

		if (Item.Amount > TotalFreeSpace)
			return false;
	}

	// 1. Сначала докладываем предмет в те ячейки, в которых он уже есть
	TArray<int> UpdatedCellsWithItem;
	{
		for (const auto CellWithItem : FoundCellsWithItem)
		{
			if (Item.Amount <= 0)
				break;

			const int StoredAmount = ItemArray[CellWithItem].Amount;
			if (StoredAmount >= ItemDefinition.Stack)
				continue;

			if (StoredAmount + Item.Amount >= ItemDefinition.Stack)
			{
				ItemArray[CellWithItem] = Item;
				ItemArray[CellWithItem].Amount = ItemDefinition.Stack;

				UpdatedCellsWithItem.Add(CellWithItem);

				Item.Amount -= ItemDefinition.Stack - StoredAmount;
				continue;
			}

			ItemArray[CellWithItem] = Item;
			ItemArray[CellWithItem].Amount = StoredAmount + Item.Amount;

			UpdatedCellsWithItem.Add(CellWithItem);

			Item.Amount = 0;
		}
	}

	// 2. Если все ячейки в с предметами уже заполнены, но очередь осталась, то выделяем пустые ячейки
	TArray<int> UpdatedEmptyCells;
	if (Item.Amount > 0)
	{
		for (const auto EmptyCell : FoundEmptyCells)
		{
			if (Item.Amount <= 0)
				break;

			if (Item.Amount >= ItemDefinition.Stack)
			{
				ItemArray[EmptyCell] = Item;
				ItemArray[EmptyCell].Amount = ItemDefinition.Stack;

				UpdatedEmptyCells.Add(EmptyCell);

				Item.Amount -= ItemDefinition.Stack;
				continue;
			}

			ItemArray[EmptyCell] = Item;

			UpdatedEmptyCells.Add(EmptyCell);

			Item.Amount = 0;
		}
	}

	Item.Amount = InitialItemAmount;

	OnContainerUpdated.Broadcast(this);
	UpdatedCellsWithItem.Append(UpdatedEmptyCells);
	OnItemAdded.Broadcast(this, Item, UpdatedCellsWithItem);

	return true;
}

bool UItemContainer::AddItemToCell(const FInventoryItem Item, const int Cell)
{
	if (!ItemArray.IsValidIndex(Cell) || !UInventoryStatics::IsValid(Item))
		return false;

	FInventoryItem StoredItem = ItemArray[Cell];

	if (StoredItem.ID != EName::None && StoredItem.ID != Item.ID)
		return false;

	const FItemDefinition ItemDefinition = UInventoryStatics::GetItemDefinition(Item.ID);

	if (Item.Amount + StoredItem.Amount > ItemDefinition.Stack)
		return false;

	StoredItem.ID = Item.ID;
	StoredItem.Amount += Item.Amount;
	ItemArray[Cell] = StoredItem;

	OnContainerUpdated.Broadcast(this);
	OnItemAdded.Broadcast(this, Item, TArray{Cell});

	return true;
}

bool UItemContainer::AddItemAmountToCell(const int Cell, const int Amount)
{
	if (!ItemArray.IsValidIndex(Cell) || Amount <= 0)
		return false;

	FInventoryItem StoredItem = ItemArray[Cell];

	if (!UInventoryStatics::IsValid(StoredItem))
		return false;

	const FItemDefinition ItemDefinition = UInventoryStatics::GetItemDefinition(StoredItem.ID);

	if (StoredItem.Amount + Amount > ItemDefinition.Stack)
		return false;

	StoredItem.Amount += Amount;
	ItemArray[Cell] = StoredItem;

	OnContainerUpdated.Broadcast(this);
	OnItemAdded.Broadcast(this, StoredItem, TArray{Cell});

	return true;
}


bool UItemContainer::RemoveItem(FInventoryItem Item)
{
	if (!UInventoryStatics::IsValid(Item))
		return false;

	if (GetItemAmount(Item.ID) < Item.Amount)
		return false;

	TArray<int> CellsWithItem;
	if (!FindAllCellsWithItem(Item.ID, CellsWithItem))
		return false;

	TArray<int> UpdatedCells;
	for (const int Cell : CellsWithItem)
	{
		if (Item.Amount <= 0)
			break;

		FInventoryItem StoredItem = ItemArray[Cell];

		if (Item.Amount < StoredItem.Amount)
		{
			StoredItem.Amount -= Item.Amount;
			ItemArray[Cell] = StoredItem;
			UpdatedCells.Add(Cell);
			
			OnContainerUpdated.Broadcast(this);
			OnItemRemoved.Broadcast(this, Item, UpdatedCells);
			
			return true;
		}

		Item.Amount -= StoredItem.Amount;
		ItemArray[Cell] = FInventoryItem();
		UpdatedCells.Add(Cell);
	}

	OnContainerUpdated.Broadcast(this);
	OnItemRemoved.Broadcast(this, Item, UpdatedCells);

	return true;
}


bool UItemContainer::RemoveItemFromCell(const FInventoryItem Item, const int Cell)
{
	if (!ItemArray.IsValidIndex(Cell) || !UInventoryStatics::IsValid(Item))
		return false;

	FInventoryItem StoredItem = ItemArray[Cell];

	if (StoredItem.ID != Item.ID)
		return false;

	if (StoredItem.Amount < Item.Amount)
		return false;

	if (Item.Amount < StoredItem.Amount)
	{
		StoredItem.Amount -= Item.Amount;
		ItemArray[Cell] = StoredItem;

		OnContainerUpdated.Broadcast(this);
		OnItemRemoved.Broadcast(this, Item, TArray{Cell});

		return true;
	}

	ItemArray[Cell] = FInventoryItem();

	OnContainerUpdated.Broadcast(this);
	OnItemRemoved.Broadcast(this, Item, TArray{Cell});

	return true;
}

bool UItemContainer::RemoveItemAmountFromCell(const int Cell, const int Amount)
{
	if (!ItemArray.IsValidIndex(Cell) || Amount <= 0)
		return false;

	FInventoryItem StoredItem = ItemArray[Cell];

	if (StoredItem.Amount < Amount)
		return false;

	if (Amount < StoredItem.Amount)
	{
		StoredItem.Amount -= Amount;
		ItemArray[Cell] = StoredItem;

		OnContainerUpdated.Broadcast(this);
		StoredItem.Amount = Amount;
		OnItemRemoved.Broadcast(this, StoredItem, TArray{Cell});

		return true;
	}

	ItemArray[Cell] = FInventoryItem();
	
	OnContainerUpdated.Broadcast(this);
	StoredItem.Amount = Amount;
	OnItemRemoved.Broadcast(this, StoredItem, TArray{Cell});

	return true;
}


bool UItemContainer::SwapItems(const int FromCell, UItemContainer* FromContainer, const int ToCell,
                               UItemContainer* ToContainer)
{
	// Проверочки
	if (!IsValid(FromContainer) || !IsValid(ToContainer))
		return false;

	if (!FromContainer->IsValidCellIndex(FromCell) || !ToContainer->IsValidCellIndex(ToCell))
		return false;

	// Нет смысла класть в одну и ту же ячейку
	if (FromContainer == ToContainer && FromCell == ToCell)
		return true;

	// Откуда перетаскиваем предмет\первая ячейка
	FInventoryItem ItemFrom;
	FromContainer->GetCell(FromCell, ItemFrom);

	// Куда перетаскиваем предмет\вторая ячейка
	FInventoryItem ItemTo;
	ToContainer->GetCell(ToCell, ItemTo);

	// Меняем местами разные предметы или одинаковые?
	if (ItemFrom.ID != ItemTo.ID)
	{
		// Разные предметы меняем местами
		ToContainer->ItemArray[ToCell] = ItemFrom;
		FromContainer->ItemArray[FromCell] = ItemTo;

		ToContainer->OnContainerUpdated.Broadcast(ToContainer);
		ToContainer->OnItemsSwapped.Broadcast(FromCell, FromContainer, ToCell, ToContainer);

		if (ToContainer != FromContainer)
		{
			FromContainer->OnContainerUpdated.Broadcast(FromContainer);
			FromContainer->OnItemsSwapped.Broadcast(FromCell, FromContainer, ToCell, ToContainer);
		}

		return true;
	}

	// Одинаковые предметы складываем вместе.
	const FItemDefinition ItemToDefinition = UInventoryStatics::GetItemDefinition(ItemTo.ID);

	const int FreeSpace = ItemToDefinition.Stack - ItemTo.Amount;

	// Есть ли место во второй ячейке?
	if (FreeSpace <= 0)
	{
		// Нет места, меняем местами
		ToContainer->ItemArray[ToCell] = ItemFrom;
		FromContainer->ItemArray[FromCell] = ItemTo;

		ToContainer->OnContainerUpdated.Broadcast(ToContainer);
		ToContainer->OnItemsSwapped.Broadcast(FromCell, FromContainer, ToCell, ToContainer);

		if (ToContainer != FromContainer)
		{
			FromContainer->OnContainerUpdated.Broadcast(FromContainer);
			FromContainer->OnItemsSwapped.Broadcast(FromCell, FromContainer, ToCell, ToContainer);
		}

		return true;
	}

	// Есть место

	// Поместится ли предмет полностью во вторую ячейку?
	if (ItemFrom.Amount <= FreeSpace)
	{
		// Поместится. Дополняем вторую ячейку, первую ячейку оставляем пустой
		ItemTo.Amount += ItemFrom.Amount;
		ToContainer->ItemArray[ToCell] = ItemTo;
		FromContainer->ItemArray[FromCell] = FInventoryItem();

		ToContainer->OnContainerUpdated.Broadcast(ToContainer);
		ToContainer->OnItemsSwapped.Broadcast(FromCell, FromContainer, ToCell, ToContainer);

		if (ToContainer != FromContainer)
		{
			FromContainer->OnContainerUpdated.Broadcast(FromContainer);
			FromContainer->OnItemsSwapped.Broadcast(FromCell, FromContainer, ToCell, ToContainer);
		}

		return true;
	}

	// Поместится. Заполняем вторую ячейку полностью, в первой ячейке оставляем разницу
	ItemTo.Amount += FreeSpace;
	ToContainer->ItemArray[ToCell] = ItemTo;

	ItemFrom.Amount -= FreeSpace;
	FromContainer->ItemArray[FromCell] = ItemFrom;

	ToContainer->OnContainerUpdated.Broadcast(ToContainer);
	ToContainer->OnItemsSwapped.Broadcast(FromCell, FromContainer, ToCell, ToContainer);
	
	if (ToContainer != FromContainer)
	{
		FromContainer->OnContainerUpdated.Broadcast(FromContainer);
		FromContainer->OnItemsSwapped.Broadcast(FromCell, FromContainer, ToCell, ToContainer);
	}
	return true;
}

void UItemContainer::ServerAddItem_Implementation(FInventoryItem Item)
{
	if(GetNetMode() == NM_DedicatedServer)
		AddItem(Item);
}

void UItemContainer::ServerSwapItems_Implementation(const int FromCell, UItemContainer* FromContainer, const int ToCell,
                                                    UItemContainer* ToContainer)
{
	if(GetNetMode() == NM_DedicatedServer)
		SwapItems(FromCell, FromContainer, ToCell, ToContainer);
}

bool UItemContainer::FindFirstCellWithItem(const FName ID, int& Cell)
{
	Cell = -1;

	for (int i = 0; i < ItemArray.Num(); ++i)
	{
		const FInventoryItem& Item = ItemArray[i];

		if (UInventoryStatics::IsValid(Item) && Item.ID == ID)
		{
			Cell = i;
			return true;
		}
	}
	return false;
}

bool UItemContainer::FindAllCellsWithItem(const FName ID, TArray<int>& Cells)
{
	Cells = {};

	for (int i = 0; i < ItemArray.Num(); ++i)
	{
		const FInventoryItem& Item = ItemArray[i];
		UInventoryStatics::IsValid(Item);

		if (UInventoryStatics::IsValid(Item) && Item.ID == ID)
		{
			Cells.Add(i);
		}
	}
	if (!Cells.IsEmpty())
		return true;
	return false;
}

bool UItemContainer::FindFirstEmptyCell(int& EmptyCell)
{
	EmptyCell = -1;

	for (int i = 0; i < ItemArray.Num(); ++i)
	{
		if (!UInventoryStatics::IsValid(ItemArray[i]))
		{
			EmptyCell = i;
			return true;
		}
	}
	return false;
}

bool UItemContainer::FindFirstEmptyCellExclude(const TArray<int>& ExcludeCells, int& EmptyCell)
{
	EmptyCell = -1;

	for (int i = 0; i < ItemArray.Num(); ++i)
	{
		if (!UInventoryStatics::IsValid(ItemArray[i]))
		{
			if (!ExcludeCells.Contains(i))
			{
				EmptyCell = i;
				return true;
			}
		}
	}
	return false;
}

bool UItemContainer::FindAllEmptyCells(TArray<int>& EmptyCells)
{
	EmptyCells = {};

	for (int i = 0; i < ItemArray.Num(); ++i)
	{
		if (ItemArray[i].ID == EName::None)
		{
			EmptyCells.Add(i);
		}
	}

	if (!EmptyCells.IsEmpty())
		return true;
	return false;
}

bool UItemContainer::FindAllEmptyCellsExclude(const TArray<int>& ExcludeCells, TArray<int>& EmptyCells)
{
	EmptyCells = {};

	for (int i = 0; i < ItemArray.Num(); ++i)
	{
		if (ItemArray[i].ID == EName::None)
		{
			if (!ExcludeCells.Contains(i))
			{
				EmptyCells.Add(i);
			}
		}
	}

	if (!EmptyCells.IsEmpty())
		return true;
	return false;
}

void UItemContainer::FindFreeSpaceForItem(const FName ID, int& FreeSpace)
{
	FreeSpace = -1;

	if (ID == EName::None)
		return;

	const FItemDefinition ItemDefinition = UInventoryStatics::GetItemDefinition(ID);

	for (int i = 0; i < ItemArray.Num(); ++i)
	{
		FInventoryItem Item = ItemArray[i];

		if (Item.ID == ID)
		{
			FreeSpace += ItemDefinition.Stack - Item.Amount;
		}
		else if (!UInventoryStatics::IsValid(Item))
		{
			FreeSpace += ItemDefinition.Stack;
		}
	}
}

void UItemContainer::FindFreeSpaceForItemExclude(const FName ID, const TArray<int>& ExcludeCells, int& FreeSpace)
{
	FreeSpace = -1;

	if (ID == EName::None)
		return;

	const FItemDefinition ItemDefinition = UInventoryStatics::GetItemDefinition(ID);

	for (int i = 0; i < ItemArray.Num(); ++i)
	{
		if (ExcludeCells.Contains(i))
			continue;

		FInventoryItem Item = ItemArray[i];

		if (Item.ID == ID)
		{
			FreeSpace += ItemDefinition.Stack - Item.Amount;
		}
		else if (!UInventoryStatics::IsValid(Item))
		{
			FreeSpace += ItemDefinition.Stack;
		}
	}
}
