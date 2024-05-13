// 


#include "Player/Interaction/InteractiveByLook.h"


UInteractiveByLook::UInteractiveByLook()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UInteractiveByLook::OnRep_IsActive()
{
	Super::OnRep_IsActive();

	if(IsActive())
		OnComponentActivated.Broadcast(this, true);
	else
		OnComponentDeactivated.Broadcast(this);
}