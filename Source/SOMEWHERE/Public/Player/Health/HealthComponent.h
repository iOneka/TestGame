#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySystem//AttributeSet//HealthAttributeSet.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

class UAbilitySystemComponent;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FAttributeChangeEvent,
                                               UHealthComponent*, HealthComponent,
                                               float, OldValue,
                                               float, NewValue);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttributeZeroEvent,
                                            UHealthComponent*, HealthComponent);


/*
 * 
 */
UCLASS(Blueprintable, Abstract, meta=(BlueprintSpawnableComponent))
class SOMEWHERE_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHealthComponent();


	/*
	 * UActorComponent
	 */
protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/*
	 * Gameplay Ability System
	 */
public:
protected:
	// Ability system used by this component.
	UPROPERTY(BlueprintReadOnly)
	UAbilitySystemComponent* AbilitySystemComponent;

	// Attribute set used by this component.
	UPROPERTY()
	UHealthAttributeSet* HealthAttributeSet;


	/*
	 * UHealthComponent
	 */
public:
	
	// Initialize the component using an ability system component.
	void InitializeHealthComponent(UAbilitySystemComponent* InASC, UHealthAttributeSet* InHealthAttributeSet);


	/*
	 * Health functions
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="HealthComponent|Health")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category="HealthComponent|Health")
	void SetHealth(float NewHealth);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="HealthComponent|Health")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category="HealthComponent|Health")
	void SetMaxHealth(float NewMaxHealth);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="HealthComponent|Health")
	float GetHealthNormalized() const;

	/*
	 * Thirst functions
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="HealthComponent|Thirst")
	float GetThirst() const;

	UFUNCTION(BlueprintCallable, Category="HealthComponent|Thirst")
	void SetThirst(float NewThirst);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="HealthComponent|Thirst")
	float GetMaxThirst() const;

	UFUNCTION(BlueprintCallable, Category="HealthComponent|Thirst")
	void SetMaxThirst(float NewMaxThirst);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="HealthComponent|Thirst")
	float GetThirstNormalized() const;

	/*
	 * Hunger functions
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="HealthComponent|Hunger")
	float GetHunger() const;

	UFUNCTION(BlueprintCallable, Category="HealthComponent|Hunger")
	void SetHunger(float NewHunger);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="HealthComponent|Hunger")
	float GetMaxHunger() const;

	UFUNCTION(BlueprintCallable, Category="HealthComponent|Hunger")
	void SetMaxHunger(float NewMaxHunger);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="HealthComponent|Hunger")
	float GetHungerNormalized() const;

	/*
	 * Stamina functions
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="HealthComponent|Stamina")
	float GetStamina() const;

	UFUNCTION(BlueprintCallable, Category="HealthComponent|Stamina")
	void SetStamina(float NewStamina);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="HealthComponent|Stamina")
	float GetMaxStamina() const;

	UFUNCTION(BlueprintCallable, Category="HealthComponent|Stamina")
	void SetMaxStamina(float NewMaxStamina);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="HealthComponent|Stamina")
	float GetStaminaNormalized() const;

	/*
	 * Armor functions
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="HealthComponent|Armor")
	float GetArmor() const;

	UFUNCTION(BlueprintCallable, Category="HealthComponent|Armor")
	void SetArmor(float NewArmor);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="HealthComponent|Armor")
	float GetMaxArmor() const;

	UFUNCTION(BlueprintCallable, Category="HealthComponent|Armor")
	void SetMaxArmor(float NewMaxArmor);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="HealthComponent|Armor")
	float GetArmorNormalized() const;


	// Delegates for blueprints to use
	UPROPERTY(BlueprintAssignable, Category="HealthComponent")
	FAttributeChangeEvent OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category="HealthComponent")
	FAttributeChangeEvent OnMaxHealthChanged;

	UPROPERTY(BlueprintAssignable, Category="HealthComponent")
	FAttributeChangeEvent OnThirstChanged;

	UPROPERTY(BlueprintAssignable, Category="HealthComponent")
	FAttributeChangeEvent OnMaxThirstChanged;

	UPROPERTY(BlueprintAssignable, Category="HealthComponent")
	FAttributeChangeEvent OnHungerChanged;

	UPROPERTY(BlueprintAssignable, Category="HealthComponent")
	FAttributeChangeEvent OnMaxHungerChanged;

	UPROPERTY(BlueprintAssignable, Category="HealthComponent")
	FAttributeChangeEvent OnStaminaChanged;

	UPROPERTY(BlueprintAssignable, Category="HealthComponent")
	FAttributeChangeEvent OnMaxStaminaChanged;

	UPROPERTY(BlueprintAssignable, Category="HealthComponent")
	FAttributeChangeEvent OnArmorChanged;

	UPROPERTY(BlueprintAssignable, Category="HealthComponent")
	FAttributeChangeEvent OnMaxArmorChanged;

protected:
	virtual void HandleHealthChanged(const FOnAttributeChangeData& ChangeData);
	virtual void HandleMaxHealthChanged(const FOnAttributeChangeData& ChangeData);
	virtual void HandleThirstChanged(const FOnAttributeChangeData& ChangeData);
	virtual void HandleMaxThirstChanged(const FOnAttributeChangeData& ChangeData);
	virtual void HandleHungerChanged(const FOnAttributeChangeData& ChangeData);
	virtual void HandleMaxHungerChanged(const FOnAttributeChangeData& ChangeData);
	virtual void HandleStaminaChanged(const FOnAttributeChangeData& ChangeData);
	virtual void HandleMaxStaminaChanged(const FOnAttributeChangeData& ChangeData);
	virtual void HandleArmorChanged(const FOnAttributeChangeData& ChangeData);
	virtual void HandleMaxArmorChanged(const FOnAttributeChangeData& ChangeData);
};
