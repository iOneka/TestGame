#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintPlatformLibrary.h"
#include "SomewhereGameInstance.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, Config=Engine)
class SOMEWHERE_API USomewhereGameInstance : public UPlatformGameInstance
{
	GENERATED_BODY()

	USomewhereGameInstance();

	virtual void Init() override;
	virtual void Shutdown() override;

public:
    UPROPERTY(BlueprintReadOnly, Config)
	FString BuildVersion;
};
