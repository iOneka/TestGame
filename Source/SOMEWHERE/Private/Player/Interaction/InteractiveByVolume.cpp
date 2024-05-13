// 


#include "Player/Interaction/InteractiveByVolume.h"


UInteractiveByVolume::UInteractiveByVolume()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UInteractiveByVolume::OnRep_IsActive()
{
	Super::OnRep_IsActive();

	if(IsActive())
		OnComponentActivated.Broadcast(this, true);
	else
		OnComponentDeactivated.Broadcast(this);
}
