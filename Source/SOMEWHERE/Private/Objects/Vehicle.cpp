// 


#include "Objects/Vehicle.h"


AVehicle::AVehicle()
{
	PrimaryActorTick.bCanEverTick = true;
}

TArray<FInventoryItems> AVehicle::GetItemsToSave_Implementation()
{
	return TArray<FInventoryItems>{};
}

bool AVehicle::LoadSavedItems_Implementation(const TArray<FInventoryItems>& Items)
{
	return true;
}
