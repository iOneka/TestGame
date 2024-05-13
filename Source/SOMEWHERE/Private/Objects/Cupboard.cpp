// 


#include "Objects/Cupboard.h"

#include "Net/UnrealNetwork.h"


// Sets default values
ACupboard::ACupboard()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ACupboard::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACupboard, AuthorizedPlayers);
}

// Called when the game starts or when spawned
void ACupboard::BeginPlay()
{
	Super::BeginPlay();
	PrivateSphere->GetOverlappingActors(OverlappingActors, AProp::StaticClass());

	for (auto Prop : OverlappingActors)
	{
		Cast<AProp>(Prop)->Cupboard = this;
	}
}

// Called every frame
void ACupboard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool ACupboard::CheckAuthority(FString PFID)
{
	return AuthorizedPlayers.Contains(PFID);
}
