// 

#pragma once

#include "CoreMinimal.h"
#include "InteractiveByVolume.generated.h"


UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class SOMEWHERE_API UInteractiveByVolume : public USceneComponent
{
	GENERATED_BODY()

	UInteractiveByVolume();

	virtual void OnRep_IsActive() override;
};
