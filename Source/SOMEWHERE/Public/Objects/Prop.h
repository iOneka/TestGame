// 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Save/ISavableActor.h"
#include "Prop.generated.h"


class ACupboard;
class APropGhost;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChanged, float, NewHealth);

UCLASS()
class SOMEWHERE_API AProp : public AActor, public ISavableActor
{
	GENERATED_BODY()

	protected:
	AProp();

	/*
	 * AActor overrides
	 */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;


	/*
	 * ISavableActor interface
	 */
	virtual TArray<FInventoryItems> GetItemsToSave_Implementation() override;
	virtual bool LoadSavedItems_Implementation(const TArray<FInventoryItems>& ContainerPayloads) override;

	
	/*
	 * AProp
	 */
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, ReplicatedUsing=OnRep_Health, Category="Prop Defaults")
	float Health;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Prop Defaults")
	float MaxHealth;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Prop Defaults")
	TMap<TSubclassOf<UDamageType>, float> DamageMultipliers;

	UPROPERTY(BlueprintReadWrite)
	bool bRuntimeSpawned;

	UPROPERTY(BlueprintReadWrite,Replicated)
	ACupboard* Cupboard;

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	FItemDefinition ItemGiveFromRemove;

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	int32 ItemGiveFromRemoveCount;
	
	/* Client only event when health changed */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic)
	void OnHealthChanged(float NewHealth);

	/* Client only event when health changed */
	UPROPERTY(BlueprintAssignable, DisplayName="On Health Changed")
	FOnHealthChanged OnHealthChangedDelegate;
	
	/* Multicast event when this prop is damaged */
	UFUNCTION(BlueprintImplementableEvent)
	void OnPropDamaged(float DamageAmount, AActor* DamageCauser);

	/* Multicast event when this prop is killed */
	UFUNCTION(BlueprintImplementableEvent)
	void OnPropKilled(AActor* DamageCauser);


	
protected:
	UFUNCTION()
	void OnRep_Health();
	
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastDamage(float DamageAmount, AActor* DamageCauser);
	
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastKill(AActor* DamageCauser);
};
