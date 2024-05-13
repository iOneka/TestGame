// 


#include "Player/Inventory/InventoryStatics.h"

#include "Player/Inventory/ItemContainer.h"


UDataTable* UInventoryStatics::GetItemsDataTable()
{
	const FSoftObjectPath SoftObjPath = FSoftObjectPath(TEXT("/Game/Somewhere/Core/Data/Items/CDT_Items.CDT_Items"));
	
	// План A. Возвращаем табличку, которая уже есть в памяти
	UDataTable* CDT_Items = Cast<UDataTable>(SoftObjPath.ResolveObject());
	if (::IsValid(CDT_Items)) return CDT_Items;

	// План B. Подгружаем табличку с диска и возвращаем её
	CDT_Items = Cast<UDataTable>(SoftObjPath.TryLoad());
	if (::IsValid(CDT_Items)) return CDT_Items;

	// План D. Сдаёмся
	return nullptr;
}

bool UInventoryStatics::IsValid(const FInventoryItem Item)
{
	if (Item.ID == EName::None || Item.Amount <= 0)
		return false;

	const FItemDefinition ItemDefinition = GetItemDefinition(Item.ID);

	if (ItemDefinition.ID == EName::None || ItemDefinition.Stack <= 0)
		return false;
	
	return true;
}

FItemDefinition UInventoryStatics::GetItemDefinition(const FName ID)
{
	// return GetRandomItemDefinition();
	if(ID == EName::None) return FItemDefinition();
	
	const UDataTable* CDT_Items = GetItemsDataTable();
	if(!::IsValid(CDT_Items)) return FItemDefinition();
	
	TArray<FName> RowNames = CDT_Items->GetRowNames();
	checkf(!RowNames.IsEmpty(), TEXT("CDT_Items has no rows"))
	
	checkf(RowNames.Contains(ID), TEXT("CDT_Items has no row with ID '%s'"), ID)
	
	const FItemDefinition* Row = CDT_Items->FindRow<FItemDefinition>(ID, "");
	return *Row;
}

FItemDefinition UInventoryStatics::GetItemDefinitionFromItem(FInventoryItem Item)
{
	return GetItemDefinition(Item.ID);
}

FInventoryItem UInventoryStatics::GetRandomInventoryItem()
{
	const FItemDefinition ItemDefinition = GetRandomItemDefinition();
	return FInventoryItem(ItemDefinition.ID, FMath::RandRange(0, ItemDefinition.Stack));
}

FItemDefinition UInventoryStatics::GetRandomItemDefinition()
{
	const UDataTable* CDT_Items = GetItemsDataTable();
	if(!::IsValid(CDT_Items)) return FItemDefinition();
	
	TArray<FItemDefinition*> ItemDefinitions;
	CDT_Items->GetAllRows<FItemDefinition>("", ItemDefinitions);
	
	if (!ItemDefinitions.IsEmpty())
		return *ItemDefinitions[FMath::RandRange(0, ItemDefinitions.Num() - 1)];
	return FItemDefinition();
}

FItemProperties_Food UInventoryStatics::GetFoodProperties(FItemDefinition ItemDefinition)
{
	return ItemDefinition.Properties.Food;
}

FItemProperties_Prop UInventoryStatics::GetPropProperties(FItemDefinition ItemDefinition)
{
	return ItemDefinition.Properties.Prop;
}

FItemProperties_Weapon UInventoryStatics::GetWeaponProperties(FItemDefinition ItemDefinition)
{
	return ItemDefinition.Properties.Weapon;
}

FItemProperties_Armor UInventoryStatics::GetArmorProperties(FItemDefinition ItemDefinition)
{
	return ItemDefinition.Properties.Armor;
}

void UInventoryStatics::DebugPrintContainer(UItemContainer* Container)
{
	if(!::IsValid(Container))
		return;
	
	UE_LOG(LogItemContainer, Log, TEXT("=============== DEBUG PRINT CONTAINER ==============="))

	const TArray<FInventoryItem> Items = Container->GetItems();

	for (int i = 0; i < Items.Num(); ++i)
	{
		FInventoryItem Item = Items[i];
		
		UE_LOG(LogItemContainer, Log, TEXT("Cell #%d\t %s\t x%d"), i, *Item.ID.ToString(), Item.Amount);
	}
	
	UE_LOG(LogItemContainer, Log, TEXT("====================================================="))
}
