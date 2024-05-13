#include "GameplayAbilitySystem//AttributeSet/HealthAttributeSet.h"
#include "Net/UnrealNetwork.h"


UHealthAttributeSet::UHealthAttributeSet() :
	Health(100.0f),
	MaxHealth(100.0f),
	Thirst(100.0f),
	MaxThirst(100.0f),
	Hunger(100.0f),
	MaxHunger(100.0f),
	Armor(100.0f),
	MaxArmor(100.0f)
{
}

void UHealthAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	if (Attribute == GetHealthAttribute()) NewValue = FMath::Clamp<float>(NewValue, 0, GetMaxHealth());
	if (Attribute == GetArmorAttribute()) NewValue = FMath::Clamp<float>(NewValue, 0, GetMaxArmor());
	if (Attribute == GetThirstAttribute()) NewValue = FMath::Clamp<float>(NewValue, 0, GetMaxThirst());
	if (Attribute == GetHungerAttribute()) NewValue = FMath::Clamp<float>(NewValue, 0, GetMaxHunger());
	if (Attribute == GetStaminaAttribute()) NewValue = FMath::Clamp<float>(NewValue, 0, GetMaxStamina());
	
	Super::PreAttributeChange(Attribute, NewValue);
}

void UHealthAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetHealthAttribute()) NewValue = FMath::Clamp<float>(NewValue, 0, GetMaxHealth());
	if (Attribute == GetArmorAttribute()) NewValue = FMath::Clamp<float>(NewValue, 0, GetMaxArmor());
	if (Attribute == GetThirstAttribute()) NewValue = FMath::Clamp<float>(NewValue, 0, GetMaxThirst());
	if (Attribute == GetHungerAttribute()) NewValue = FMath::Clamp<float>(NewValue, 0, GetMaxHunger());
	if (Attribute == GetStaminaAttribute()) NewValue = FMath::Clamp<float>(NewValue, 0, GetMaxStamina());
	
	Super::PreAttributeBaseChange(Attribute, NewValue);
}

void UHealthAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
}

void UHealthAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UHealthAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHealthAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHealthAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHealthAttributeSet, MaxArmor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHealthAttributeSet, Hunger, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHealthAttributeSet, MaxHunger, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHealthAttributeSet, Thirst, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHealthAttributeSet, MaxThirst, COND_None, REPNOTIFY_Always);
}


void UHealthAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHealthAttributeSet, Health, OldHealth);
}

void UHealthAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHealthAttributeSet, MaxHealth, OldMaxHealth);
}


void UHealthAttributeSet::OnRep_Thirst(const FGameplayAttributeData& OldThirst)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHealthAttributeSet, Thirst, OldThirst);
}

void UHealthAttributeSet::OnRep_MaxThirst(const FGameplayAttributeData& OldMaxThirst)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHealthAttributeSet, MaxThirst, OldMaxThirst);
}


void UHealthAttributeSet::OnRep_Hunger(const FGameplayAttributeData& OldHunger)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHealthAttributeSet, Hunger, OldHunger);
}

void UHealthAttributeSet::OnRep_MaxHunger(const FGameplayAttributeData& OldMaxHunger)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHealthAttributeSet, MaxHunger, OldMaxHunger);
}


void UHealthAttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHealthAttributeSet, Stamina, OldStamina);
}

void UHealthAttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHealthAttributeSet, MaxStamina, OldMaxStamina);
}


void UHealthAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHealthAttributeSet, Armor, OldArmor);
}

void UHealthAttributeSet::OnRep_MaxArmor(const FGameplayAttributeData& OldMaxArmor)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHealthAttributeSet, MaxArmor, OldMaxArmor);
}
