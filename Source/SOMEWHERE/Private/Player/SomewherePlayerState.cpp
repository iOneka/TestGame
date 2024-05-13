// 


#include "Player/SomewherePlayerState.h"

#include "Net/UnrealNetwork.h"

ASomewherePlayerState::ASomewherePlayerState()
{
}

void ASomewherePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ASomewherePlayerState, PlayFabID)
}
