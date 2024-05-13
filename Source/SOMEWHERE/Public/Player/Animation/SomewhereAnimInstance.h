

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"			
#include "GameplayEffectTypes.h"
#include "SomewhereAnimInstance.generated.h"
/**
 * 
 */
class UAbilitySystemComponent;


UCLASS()
class SOMEWHERE_API USomewhereAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	virtual void InitializeWithAbilitySystem(UAbilitySystemComponent* ASC);

protected:

	virtual void NativeInitializeAnimation() override;

	UPROPERTY(EditDefaultsOnly, Category = "GameplayTags")
	FGameplayTagBlueprintPropertyMap GameplayTagPropertyMap;
};
