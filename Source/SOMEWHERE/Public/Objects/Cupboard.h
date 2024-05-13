// 

#pragma once

#include "CoreMinimal.h"
#include "Prop.h"
#include "Components/SphereComponent.h"
#include "Cupboard.generated.h"

UCLASS()
class SOMEWHERE_API ACupboard : public AProp
{
	GENERATED_BODY()


	ACupboard();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<AActor*> OverlappingActors;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated)
	TArray<FString> AuthorizedPlayers;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USphereComponent* PrivateSphere;

	UFUNCTION(BlueprintCallable)
	bool CheckAuthority(FString PFID);
};
