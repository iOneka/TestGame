// 


#include "Objects/Prop.h"

#include "Net/UnrealNetwork.h"
#include "Player/Inventory/InventoryTypes.h"


AProp::AProp(): Health(1000), MaxHealth(1000), bRuntimeSpawned(false)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AProp::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AProp, Health);
	DOREPLIFETIME(AProp, Cupboard);
}

float AProp::TakeDamage(const float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	// Undamagable prop
	if(!CanBeDamaged())
		return 0;

	// Already killed
	if(Health <= 0)
		return 0;
	
	Health = FMath::Clamp(Health - DamageAmount, 0, MaxHealth);
	
	if(Health > 0)
		MulticastDamage(DamageAmount, DamageCauser);
	else
		MulticastKill(DamageCauser);
	
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	return DamageAmount;
}

TArray<FInventoryItems> AProp::GetItemsToSave_Implementation()
{
	return TArray<FInventoryItems>{};
}

bool AProp::LoadSavedItems_Implementation(const TArray<FInventoryItems>& ContainerPayloads)
{
	return true;
}

void AProp::OnRep_Health()
{
	OnHealthChanged(Health);
	OnHealthChangedDelegate.Broadcast(Health);
}

void AProp::MulticastDamage_Implementation(const float DamageAmount, AActor* DamageCauser)
{
	OnPropDamaged(DamageAmount, DamageCauser);
}

void AProp::MulticastKill_Implementation(AActor* DamageCauser)
{
	OnPropKilled(DamageCauser);
}
