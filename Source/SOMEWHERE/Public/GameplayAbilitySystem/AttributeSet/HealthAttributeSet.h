#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "HealthAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)



/**
 * 
 */
UCLASS()
class SOMEWHERE_API UHealthAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

	
public:
	UHealthAttributeSet();

	// AttributeSet Overrides
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	
	// Health
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealthAttributeSet", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, Health)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealthAttributeSet", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, MaxHealth)

	
	// Thirst
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealthAttributeSet", ReplicatedUsing = OnRep_Thirst)
	FGameplayAttributeData Thirst;
	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, Thirst)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealthAttributeSet", ReplicatedUsing = OnRep_MaxThirst)
	FGameplayAttributeData MaxThirst;
	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, MaxThirst)

	
	// Hunger
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealthAttributeSet", ReplicatedUsing = OnRep_Hunger)
	FGameplayAttributeData Hunger;
	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, Hunger)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealthAttributeSet", ReplicatedUsing = OnRep_MaxHunger)
	FGameplayAttributeData MaxHunger;
	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, MaxHunger)

	
	// Stamina
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealthAttributeSet", ReplicatedUsing = OnRep_Stamina)
	FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, Stamina)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealthAttributeSet", ReplicatedUsing = OnRep_MaxStamina)
	FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, MaxStamina)
	

	// Armor
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealthAttributeSet", ReplicatedUsing = OnRep_Armor)
	FGameplayAttributeData Armor;
	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, Armor)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealthAttributeSet", ReplicatedUsing = OnRep_MaxArmor)
	FGameplayAttributeData MaxArmor;
	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, MaxArmor)


protected:
	/*
	 * These OnRep functions exist to make sure that the ability system
	 * internal representations are synchronized properly during replication
	 */
	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);

	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

	UFUNCTION()
	virtual void OnRep_Thirst(const FGameplayAttributeData& OldThirst);

	UFUNCTION()
	virtual void OnRep_MaxThirst(const FGameplayAttributeData& OldMaxThirst);

	UFUNCTION()
	virtual void OnRep_Hunger(const FGameplayAttributeData& OldHunger);

	UFUNCTION()
	virtual void OnRep_MaxHunger(const FGameplayAttributeData& OldMaxHunger);

	UFUNCTION()
	virtual void OnRep_Stamina(const FGameplayAttributeData& OldStamina);

	UFUNCTION()
	virtual void OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina);

	UFUNCTION()
	virtual void OnRep_Armor(const FGameplayAttributeData& OldArmor);

	UFUNCTION()
	virtual void OnRep_MaxArmor(const FGameplayAttributeData& OldMaxArmor);

};
