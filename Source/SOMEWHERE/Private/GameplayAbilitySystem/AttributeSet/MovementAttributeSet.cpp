#include "GameplayAbilitySystem/AttributeSet/MovementAttributeSet.h"

#include "Net/UnrealNetwork.h"
#include "Math/NumericLimits.h"


UMovementAttributeSet::UMovementAttributeSet() :
	LookSensitivityScale(1.0f)
{
}

void UMovementAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	if (Attribute == GetLookSensitivityScaleAttribute()) NewValue = FMath::Clamp<float>(NewValue, 0, MAX_flt);

	Super::PreAttributeChange(Attribute, NewValue);
}

void UMovementAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetLookSensitivityScaleAttribute()) NewValue = FMath::Clamp<float>(NewValue, 0, MAX_flt);

	Super::PreAttributeBaseChange(Attribute, NewValue);
}

void UMovementAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UMovementAttributeSet, LookSensitivityScale, COND_None, REPNOTIFY_Always);
}

void UMovementAttributeSet::OnRep_LookSensitivityScale(const FGameplayAttributeData& OldLookSensitivityScale)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMovementAttributeSet, LookSensitivityScale, OldLookSensitivityScale);
}
