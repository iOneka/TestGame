#include "Player/Interaction/InteractionComponent.h"


UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.bCanEverTick = true;
}