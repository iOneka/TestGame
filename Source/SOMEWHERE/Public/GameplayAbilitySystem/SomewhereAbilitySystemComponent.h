

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "SomewhereAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class SOMEWHERE_API USomewhereAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:

	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor);
};
