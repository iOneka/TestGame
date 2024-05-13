#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "MovementAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)


/**
 * 
 */
UCLASS()
class SOMEWHERE_API UMovementAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UMovementAttributeSet();

	// AttributeSet Overrides
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	// Look Sensitivity Scale
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_LookSensitivityScale)
	FGameplayAttributeData LookSensitivityScale;
	ATTRIBUTE_ACCESSORS(UMovementAttributeSet, LookSensitivityScale)

protected:
	/*
	 * These OnRep functions exist to make sure that the ability system
	 * internal representations are synchronized properly during replication
	 */
	UFUNCTION()
	virtual void OnRep_LookSensitivityScale(const FGameplayAttributeData& OldLookSensitivityScale);
};
