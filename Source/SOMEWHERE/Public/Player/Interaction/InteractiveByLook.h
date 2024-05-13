// 

#pragma once

#include "CoreMinimal.h"
#include "InteractiveByLook.generated.h"


UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class SOMEWHERE_API UInteractiveByLook : public USceneComponent
{
	GENERATED_BODY()

	UInteractiveByLook();

	virtual void OnRep_IsActive() override;
};
