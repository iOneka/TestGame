#include "GameplayAbilitySystem/AbilityTask/AbilityTask_Tick.h"

UAbilityTask_Tick::UAbilityTask_Tick(const FObjectInitializer& ObjectInitializer)
{
	bTickingTask = true;
}

UAbilityTask_Tick* UAbilityTask_Tick::Tick(UGameplayAbility* OwningAbility)
{
	UAbilityTask_Tick* MyObj = NewAbilityTask<UAbilityTask_Tick>(OwningAbility);
	return MyObj;
}

void UAbilityTask_Tick::Activate()
{
	Super::Activate();
}

void UAbilityTask_Tick::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);
	OnTick.Broadcast(DeltaTime);
}
