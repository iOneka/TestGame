// 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "WheeledVehiclePawn.h"
#include "Save/ISavableActor.h"
#include "Vehicle.generated.h"

UCLASS()
class SOMEWHERE_API AVehicle : public AWheeledVehiclePawn, public ISavableActor
{
	GENERATED_BODY()

	AVehicle();

	/*
	 * ISavableActor
	 */
public:
	virtual TArray<FInventoryItems> GetItemsToSave_Implementation() override;
	virtual bool LoadSavedItems_Implementation(const TArray<FInventoryItems>& Items) override;

	/*
	 * AProp
	 */
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Vehicle Defaults")
	float Health;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Vehicle Defaults")
	float MaxHealth;
};
