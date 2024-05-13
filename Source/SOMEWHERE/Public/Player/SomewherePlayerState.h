// 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SomewherePlayerState.generated.h"


/**
 * 
 */
UCLASS()
class SOMEWHERE_API ASomewherePlayerState : public APlayerState
{
	GENERATED_BODY()

	ASomewherePlayerState();

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(BlueprintReadOnly, Replicated)
	FString PlayFabID;
};
