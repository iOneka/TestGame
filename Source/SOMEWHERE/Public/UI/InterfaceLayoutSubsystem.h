// 

#pragma once

#include "CoreMinimal.h"
#include "InterfaceLayoutSubsystem.generated.h"

class UInterfaceLayout;


UCLASS(Config=Game, DefaultConfig, meta = (DisplayName="Somewhere"))
class SOMEWHERE_API USomewhereSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UInterfaceLayout> InterfaceLayoutClass;
};


/**
 * 
 */
UCLASS()
class SOMEWHERE_API UInterfaceLayoutSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void PlayerControllerChanged(APlayerController* NewPlayerController) override;
	
public:
	UPROPERTY()
	UInterfaceLayout* CurrentInterfaceLayout;
};
