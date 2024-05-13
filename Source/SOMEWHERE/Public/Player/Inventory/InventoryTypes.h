// 

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "InventoryTypes.generated.h"


class AProp;
class APropGhost;

UENUM(BlueprintType)
enum EItemType
{
	Food UMETA(DisplayName="Food"),
	Clothing UMETA(DisplayName="Clothing"),
	Weapons UMETA(DisplayName="Weapons"),
	Misc UMETA(DisplayName="Misc"),
	Medicals UMETA(DisplayName="Medicals"),
	Components UMETA(DisplayName="Components"),
	Ammunition UMETA(DisplayName="Ammunition"),
	Resources UMETA(DisplayName="Resources"),
	Tools UMETA(DisplayName="Tools"),
	Construction UMETA(DisplayName="Construction"),
	Props UMETA(DisplayName="Props"),
	Traps UMETA(DisplayName="Traps")
};

UENUM(BlueprintType)
enum EArmorType
{
	Head UMETA(DisplayName="Head"),
	Chest UMETA(DisplayName="Chest"),
	Legs UMETA(DisplayName="Legs"),
	Feet UMETA(DisplayName="Feet"),
	Backpack UMETA(DisplayName="Backpack")
};

USTRUCT(BlueprintType)
struct FItemProperties_Food
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	double Hunger;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	double Thirst;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<USoundBase> ConsumeSound;
};

USTRUCT(BlueprintType)
struct FItemProperties_Armor
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TEnumAsByte<EArmorType> Type;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<USkeletalMesh> Mesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<UClass*, double> Protection;
};

USTRUCT(BlueprintType)
struct FItemProperties_Weapon
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSoftClassPtr<AActor> HoldableClass;
};

USTRUCT(BlueprintType)
struct FItemProperties_Prop
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSoftClassPtr<AProp> PropClass;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSoftClassPtr<APropGhost> PropGhostClass;
};


USTRUCT(BlueprintType)
struct FItemProperties
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FItemProperties_Food Food;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FItemProperties_Armor Armor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FItemProperties_Weapon Weapon;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FItemProperties_Prop Prop;
};


USTRUCT(BlueprintType)
struct FItemDefinition : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName ID;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FText Label;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FText Description;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Stack;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSoftObjectPtr<UStaticMesh> Mesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TEnumAsByte<EItemType> Type;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FItemProperties Properties;
};


USTRUCT(BlueprintType)
struct FInventoryItem
{
	GENERATED_BODY()

	FInventoryItem(): Amount(0), Cell(0), CustomInteger(0), Durability(0)
	{
	}

	FInventoryItem(const FName ItemID, const int ItemAmount): Cell(0), CustomInteger(0), Durability(0)
	{
		ID = ItemID;
		Amount = ItemAmount;
	}

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName ID;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int Amount;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int Cell;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int CustomInteger;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(MakeStructureDefaultValue="1.0"))
	float Durability;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<uint8> AdditionalData;
};


USTRUCT(BlueprintType)
struct FInventoryItems : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FInventoryItem> Items;
};
