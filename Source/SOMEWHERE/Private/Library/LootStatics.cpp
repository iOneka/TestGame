// 


#include "Library/LootStatics.h"

#include "Player/Inventory/InventoryTypes.h"

TArray<FInventoryItem> ULootStatics::CalculateLoot(FLoot Loot)
{
	TArray<FInventoryItem> ItemsToGive;
	
	//Нужно если мы хотим выдать несколько предметов но с определенным шансом, например 100% выпадает один предмет, и 10% на другой
	//Включаем ли доп предметы? Если удачно то не даем доп предметы
	if(FMath::RandRange(0,100)<Loot.ChanceOfMaxCount)
	{
		Loot.Count=Loot.MaxCount;
	}
	//Шанс не выпал, поэтому получаем доп предметы в разбросе от минимума к максимуму (например 1-3 предмета рандомно)
	else
	{
		Loot.Count=FMath::RandRange(Loot.MinCount,Loot.MaxCount);
	}
	
	Loot.Count = FMath::Clamp(Loot.Count, 0, Loot.Items.Num());
	if (Loot.Count == 0)
		return ItemsToGive;
	
	
	// Добавляем в ItemsToGive лаки предметы с рандомным количеством
	int Index = 0;
	while (true) // хули ты мне сделаешь, я в другом городе
	{
		// Набрали нужное количество предметов?
		if (ItemsToGive.Num() == Loot.Count)
			break;

		if(!Loot.Items.IsValidIndex(Index))
			break;
		
		FLootItem LootItem = Loot.Items[Index];

		// Удача-удача или потеря-потеря
		if (LootItem.Chance < FMath::RandRange(0.1f, 100.0f))
		{
			// Потеря-потеря
			Index += 1;
			continue;
		}

		// Добавляем предмет
		const FItemDefinition* ItemDefinition = LootItem.Item.GetRow<FItemDefinition>("");
		const int Amount = FMath::RandRange(LootItem.Min, FMath::Clamp(LootItem.Max, 0, ItemDefinition->Stack));
		ItemsToGive.Add(FInventoryItem(ItemDefinition->ID, Amount));

		//Удаляем предмет из листа чтобы второй раз он нам не выпал
		Loot.Items.RemoveAt(Index);
		
		// Отработали предмет, идём дальше
		//Index += 1;
	}

	if(ItemsToGive.Num() == Loot.Count)
		return ItemsToGive;


	
	// Если удача не удачится, то рандомно вытаскиваем предметы с рандомным количеством
	while (ItemsToGive.Num() < Loot.Count)
	{
		if(Loot.Items.Num()<1)
			break;
		Index=FMath::RandRange(0, Loot.Items.Num() - 1);
		FLootItem LootItem = Loot.Items[Index];
		
		// Добавляем предмет
		const FItemDefinition* ItemDefinition = LootItem.Item.GetRow<FItemDefinition>("");
		const int Amount = FMath::RandRange(LootItem.Min, FMath::Clamp(LootItem.Max, 0, ItemDefinition->Stack));
		ItemsToGive.Add(FInventoryItem(ItemDefinition->ID, Amount));
		//Удаляем предмет из листа чтобы второй раз он нам не выпал
		Loot.Items.RemoveAt(Index);
	}
	
	return ItemsToGive;
}
