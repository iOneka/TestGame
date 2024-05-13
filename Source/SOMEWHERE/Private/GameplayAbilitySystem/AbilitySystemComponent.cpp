


#include "GameplayAbilitySystem/SomewhereAbilitySystemComponent.h"
#include "Player/Animation/SomewhereAnimInstance.h"

void USomewhereAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	FGameplayAbilityActorInfo* ActorInfo = AbilityActorInfo.Get();
	check(ActorInfo);

	const bool bHasNewPawnAvatar = Cast<APawn>(InAvatarActor) && (InAvatarActor != ActorInfo->AvatarActor);
	if (bHasNewPawnAvatar) {
		Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);
		if (USomewhereAnimInstance* SomewhereAnimInstance = Cast<USomewhereAnimInstance>(ActorInfo->GetAnimInstance()))
		{
			SomewhereAnimInstance->InitializeWithAbilitySystem(this);
		}
	}
}
