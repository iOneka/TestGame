// 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Player/Inventory/InventoryTypes.h"
#include "Construction.generated.h"


USTRUCT(BlueprintType)
struct FTransformArray
{
	GENERATED_BODY()

	UPROPERTY(BlueprintType, EditAnywhere)
	TArray<FTransform> Transforms;
};


UCLASS(Blueprintable, BlueprintType, meta=(BlueprintSpawnableComponent))
class SOMEWHERE_API UConstructionSocket : public USceneComponent
{
	GENERATED_BODY()

	UConstructionSocket();

public:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName SocketName;
};


/*
 *
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class SOMEWHERE_API AConstruction : public AActor
{
	GENERATED_BODY()

public:
	AConstruction();

	virtual void BeginPlay() override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual void PreDestroy();
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Health;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MaxHealth;

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	FString ItemGiveFromRemove;

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	int32 ItemGiveFromRemoveCount;

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	int32 ResourcePrice;
	
	/*  */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FName SocketName;

	/*  */
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, meta=(ExposeOnSpawn=true))
	bool IsBuilt;

	/*  */
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	TMap<FName, FTransformArray> SocketTransforms;

	UFUNCTION()
	void LoadSockets();

	UFUNCTION(BlueprintPure)
	bool CanBeSnappedToConstruction(AConstruction* Construction);

	UFUNCTION(BlueprintPure)
	FTransform GetNearestSocketToSnap(FName Socket, FVector Location);

	UFUNCTION(BlueprintPure)
	TArray<AConstruction*> GetSnappedConstructions();

	UFUNCTION(BlueprintPure)
	bool IsInSocket(AConstruction* Construction);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool CanBeBuilt(AActor* TargetActor);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool HasSupport();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool HasNoBlocks();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool HasCupboardAuthority();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void UpgradeConstruction(FName Action,ACharacter* Character);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool AdjustPosition(AConstruction* LookConstruction);
};


USTRUCT(BlueprintType)
struct FConstructionData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName ID;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FText Name;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSoftClassPtr<AConstruction> ConstructionClass;
};


UCLASS(Blueprintable, BlueprintType)
class SOMEWHERE_API AConstruction_Ramp : public AConstruction
{
	GENERATED_BODY()

public:
	virtual bool HasNoBlocks_Implementation() override;
};
