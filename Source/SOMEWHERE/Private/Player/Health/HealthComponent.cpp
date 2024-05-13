#include "Player/Health/HealthComponent.h"



UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}


void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UHealthComponent::InitializeHealthComponent(UAbilitySystemComponent* InASC, UHealthAttributeSet* InHealthAttributeSet)
{
	const AActor* Owner = GetOwner();
	check(Owner);

	if (AbilitySystemComponent)
		return;

	AbilitySystemComponent = InASC;
	if (!AbilitySystemComponent)
		return;

	HealthAttributeSet = InHealthAttributeSet;
	if (!HealthAttributeSet)
		return;

	// Register to listen for attribute changes.
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UHealthAttributeSet::GetHealthAttribute()).AddUObject(this, &ThisClass::HandleHealthChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UHealthAttributeSet::GetMaxHealthAttribute()).AddUObject(this, &ThisClass::HandleMaxHealthChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UHealthAttributeSet::GetThirstAttribute()).AddUObject(this, &ThisClass::HandleThirstChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UHealthAttributeSet::GetMaxThirstAttribute()).AddUObject(this, &ThisClass::HandleMaxThirstChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UHealthAttributeSet::GetHungerAttribute()).AddUObject(this, &ThisClass::HandleHungerChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UHealthAttributeSet::GetMaxHungerAttribute()).AddUObject(this, &ThisClass::HandleMaxHungerChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UHealthAttributeSet::GetStaminaAttribute()).AddUObject(this, &ThisClass::HandleStaminaChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UHealthAttributeSet::GetMaxStaminaAttribute()).AddUObject(this, &ThisClass::HandleMaxStaminaChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UHealthAttributeSet::GetArmorAttribute()).AddUObject(this, &ThisClass::HandleArmorChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UHealthAttributeSet::GetMaxArmorAttribute()).AddUObject(this, &ThisClass::HandleMaxArmorChanged);
	
	OnHealthChanged.Broadcast(this, HealthAttributeSet->GetHealth(), HealthAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(this, HealthAttributeSet->GetMaxHealth(), HealthAttributeSet->GetMaxHealth());
	OnThirstChanged.Broadcast(this, HealthAttributeSet->GetThirst(), HealthAttributeSet->GetThirst());
	OnMaxThirstChanged.Broadcast(this, HealthAttributeSet->GetMaxThirst(), HealthAttributeSet->GetMaxThirst());
	OnHungerChanged.Broadcast(this, HealthAttributeSet->GetHunger(), HealthAttributeSet->GetHunger());
	OnMaxHungerChanged.Broadcast(this, HealthAttributeSet->GetMaxHunger(), HealthAttributeSet->GetMaxHunger());
	OnStaminaChanged.Broadcast(this, HealthAttributeSet->GetStamina(), HealthAttributeSet->GetStamina());
	OnMaxStaminaChanged.Broadcast(this, HealthAttributeSet->GetMaxStamina(), HealthAttributeSet->GetMaxStamina());
	OnArmorChanged.Broadcast(this, HealthAttributeSet->GetArmor(), HealthAttributeSet->GetArmor());
	OnMaxArmorChanged.Broadcast(this, HealthAttributeSet->GetMaxArmor(), HealthAttributeSet->GetMaxArmor());
}

float UHealthComponent::GetHealth() const
{
	return AbilitySystemComponent->GetNumericAttributeBase(UHealthAttributeSet::GetHealthAttribute());
}

void UHealthComponent::SetHealth(float NewHealth)
{
	HealthAttributeSet->SetHealth(NewHealth);
}

float UHealthComponent::GetMaxHealth() const
{
	return AbilitySystemComponent->GetNumericAttributeBase(UHealthAttributeSet::GetMaxHealthAttribute());
}

void UHealthComponent::SetMaxHealth(float NewMaxHealth)
{
	HealthAttributeSet->SetMaxHealth(NewMaxHealth);
}

float UHealthComponent::GetHealthNormalized() const
{
	const float Health = GetHealth();
	const float MaxHealth = GetMaxHealth();

	return (MaxHealth > 0.0f ? (Health / MaxHealth) : 0.0f);
}

float UHealthComponent::GetThirst() const
{
	return AbilitySystemComponent->GetNumericAttributeBase(UHealthAttributeSet::GetThirstAttribute());
}

void UHealthComponent::SetThirst(float NewThirst)
{
	HealthAttributeSet->SetThirst(NewThirst);
}

float UHealthComponent::GetMaxThirst() const
{
	return AbilitySystemComponent->GetNumericAttributeBase(UHealthAttributeSet::GetMaxThirstAttribute());
}

void UHealthComponent::SetMaxThirst(float NewMaxThirst)
{
	HealthAttributeSet->SetMaxThirst(NewMaxThirst);
}

float UHealthComponent::GetThirstNormalized() const
{
	const float Thirst = GetThirst();
	const float MaxThirst = GetMaxThirst();

	return (MaxThirst > 0.0f ? (Thirst / MaxThirst) : 0.0f);
}

float UHealthComponent::GetHunger() const
{
	return AbilitySystemComponent->GetNumericAttributeBase(UHealthAttributeSet::GetHungerAttribute());
}

void UHealthComponent::SetHunger(float NewHunger)
{
	HealthAttributeSet->SetHunger(NewHunger);
}

float UHealthComponent::GetMaxHunger() const
{
	return AbilitySystemComponent->GetNumericAttributeBase(UHealthAttributeSet::GetMaxHungerAttribute());
}

void UHealthComponent::SetMaxHunger(float NewMaxHunger)
{
	HealthAttributeSet->SetMaxHunger(NewMaxHunger);
}

float UHealthComponent::GetHungerNormalized() const
{
	const float Hunger = GetHunger();
	const float MaxHunger = GetMaxHunger();

	return (MaxHunger > 0.0f ? (Hunger / MaxHunger) : 0.0f);
}

float UHealthComponent::GetStamina() const
{
	return AbilitySystemComponent->GetNumericAttributeBase(UHealthAttributeSet::GetStaminaAttribute());
}

void UHealthComponent::SetStamina(float NewStamina)
{
	HealthAttributeSet->SetStamina(NewStamina);
}

float UHealthComponent::GetMaxStamina() const
{
	return AbilitySystemComponent->GetNumericAttributeBase(UHealthAttributeSet::GetMaxStaminaAttribute());
}

void UHealthComponent::SetMaxStamina(float NewMaxStamina)
{
	HealthAttributeSet->SetMaxStamina(NewMaxStamina);
}

float UHealthComponent::GetStaminaNormalized() const
{
	const float Stamina = GetStamina();
	const float MaxStamina = GetMaxStamina();

	return (MaxStamina > 0.0f ? (Stamina / MaxStamina) : 0.0f);
}

float UHealthComponent::GetArmor() const
{
	return AbilitySystemComponent->GetNumericAttributeBase(UHealthAttributeSet::GetArmorAttribute());
}

void UHealthComponent::SetArmor(float NewArmor)
{
	HealthAttributeSet->SetArmor(NewArmor);
}

float UHealthComponent::GetMaxArmor() const
{
	return AbilitySystemComponent->GetNumericAttributeBase(UHealthAttributeSet::GetMaxArmorAttribute());
}

void UHealthComponent::SetMaxArmor(float NewMaxArmor)
{
	HealthAttributeSet->SetMaxArmor(NewMaxArmor);
}

float UHealthComponent::GetArmorNormalized() const
{
	const float Armor = GetArmor();
	const float MaxArmor = GetMaxArmor();

	return (MaxArmor > 0.0f ? (Armor / MaxArmor) : 0.0f);
}


void UHealthComponent::HandleHealthChanged(const FOnAttributeChangeData& ChangeData)
{
	if(ChangeData.OldValue != ChangeData.NewValue)
		OnHealthChanged.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue);
}

void UHealthComponent::HandleMaxHealthChanged(const FOnAttributeChangeData& ChangeData)
{
	if(ChangeData.OldValue != ChangeData.NewValue)
		OnMaxHealthChanged.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue);
}

void UHealthComponent::HandleThirstChanged(const FOnAttributeChangeData& ChangeData)
{
	if(ChangeData.OldValue != ChangeData.NewValue)
		OnThirstChanged.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue);
}

void UHealthComponent::HandleMaxThirstChanged(const FOnAttributeChangeData& ChangeData)
{
	if(ChangeData.OldValue != ChangeData.NewValue)
		OnMaxThirstChanged.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue);
}

void UHealthComponent::HandleHungerChanged(const FOnAttributeChangeData& ChangeData)
{
	if(ChangeData.OldValue != ChangeData.NewValue)
		OnHungerChanged.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue);
}

void UHealthComponent::HandleMaxHungerChanged(const FOnAttributeChangeData& ChangeData)
{
	if(ChangeData.OldValue != ChangeData.NewValue)
		OnMaxHungerChanged.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue);
}

void UHealthComponent::HandleStaminaChanged(const FOnAttributeChangeData& ChangeData)
{
	if(ChangeData.OldValue != ChangeData.NewValue)
		OnStaminaChanged.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue);
}

void UHealthComponent::HandleMaxStaminaChanged(const FOnAttributeChangeData& ChangeData)
{
	if(ChangeData.OldValue != ChangeData.NewValue)
		OnMaxStaminaChanged.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue);
}

void UHealthComponent::HandleArmorChanged(const FOnAttributeChangeData& ChangeData)
{
	if(ChangeData.OldValue != ChangeData.NewValue)
		OnArmorChanged.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue);
}

void UHealthComponent::HandleMaxArmorChanged(const FOnAttributeChangeData& ChangeData)
{
	if(ChangeData.OldValue != ChangeData.NewValue)
		OnMaxArmorChanged.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue);
}
