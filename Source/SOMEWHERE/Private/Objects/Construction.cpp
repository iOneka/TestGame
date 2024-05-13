// 


#include "Objects/Construction.h"


#include "Kismet/GameplayStatics.h"
#include "Library/SomewhereStatics.h"
#include "Objects/Cupboard.h"
#include "Player/SomewhereCharacter.h"


UConstructionSocket::UConstructionSocket()
{
}

AConstruction::AConstruction(): Health(4000), MaxHealth(4000), SocketName(NAME_None), IsBuilt(true)
{
	PrimaryActorTick.bCanEverTick = false;
	SetReplicates(true);
}

void AConstruction::BeginPlay()
{
	Super::BeginPlay();

	if (IsBuilt)
	{
		LoadSockets();
	}
	else
	{
		// Avoid z-fighting
		SetActorScale3D(FVector(1.001f));

		// Disable mesh collisions
		TArray<UStaticMeshComponent*> Meshes;
		GetComponents(UStaticMeshComponent::StaticClass(), Meshes);
		for (auto Mesh : Meshes)
			Mesh->SetCollisionProfileName(FName("NoCollision"));
	}
}

float AConstruction::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Health -= DamageAmount;

	if(Health <= 0)
		PreDestroy();

	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void AConstruction::PreDestroy()
{
	if(!IsValid(this))
		return;

	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString::Printf(TEXT("Destroying %s. Snapped constructions:"), *GetName()));
	
	auto SnappedConstructions = GetSnappedConstructions();
	Destroy();
	
	// for (const AConstruction* SnappedConstruction : SnappedConstructions)
	// {
	// 	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, FString::Printf(TEXT("         %s"), *SnappedConstruction->GetName()));
	// }
	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Emerald, TEXT(" "));
	
	for (AConstruction* SnappedConstruction : SnappedConstructions)
	{
		if(!SnappedConstruction->HasSupport())
			SnappedConstruction->PreDestroy();
	}
}

void AConstruction::LoadSockets()
{
	SocketTransforms.Empty();

	TArray<UConstructionSocket*> Sockets;
	GetComponents(UConstructionSocket::StaticClass(), Sockets);

	for (const UConstructionSocket* Socket : Sockets)
	{
		if (FTransformArray* ExistingTransformArray = SocketTransforms.Find(Socket->SocketName))
		{
			ExistingTransformArray->Transforms.Add(Socket->GetComponentTransform());
		}
		else
		{
			FTransformArray TransformArray;
			TransformArray.Transforms.Add(Socket->GetComponentTransform());
			SocketTransforms.Add(Socket->SocketName, TransformArray);
		}
	}
}

bool AConstruction::CanBeSnappedToConstruction(AConstruction* Construction)
{
	if (!IsValid(Construction))
		return false;

	if (Construction->SocketTransforms.Contains(SocketName))
		return true;
	return false;
}

FTransform AConstruction::GetNearestSocketToSnap(FName Socket, FVector Location)
{
	if (!SocketTransforms.Contains(Socket))
		return FTransform();

	FTransformArray* TransformArray = SocketTransforms.Find(Socket);
	if (TransformArray->Transforms.IsEmpty())
		return FTransform();

	FTransform ClosestTransform = TransformArray->Transforms[0];
	float MinDistanceSquared = FVector::DistSquared(ClosestTransform.GetLocation(), Location);

	for (FTransform& Transform : TransformArray->Transforms)
	{
		const float DistanceSquared = FVector::DistSquared(Transform.GetLocation(), Location);
		if (DistanceSquared < MinDistanceSquared)
		{
			MinDistanceSquared = DistanceSquared;
			ClosestTransform = Transform;
		}
	}

	return ClosestTransform;
}

TArray<AConstruction*> AConstruction::GetSnappedConstructions()
{
	TArray<AConstruction*> Constructions;
	
	{
		FVector Origin;
		FVector Extents;
		GetActorBounds(false, Origin, Extents);

		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));

		FCollisionQueryParams Params;
		TArray<FHitResult> OutHits;
		GetWorld()->SweepMultiByObjectType(OutHits, Origin, Origin, FQuat(), ObjectTypes, FCollisionShape::MakeBox(Extents + 5), Params);

		for (auto HitResult : OutHits)
		{
			AConstruction* Construction = Cast<AConstruction>(HitResult.GetActor());
			if(IsValid(Construction))
				Constructions.AddUnique(Construction);
		}
	}

	{
		for (auto SocketTransform : SocketTransforms)
		{
			TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
			ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));

			FCollisionQueryParams Params;
			FHitResult OutHit;
			GetWorld()->SweepSingleByObjectType(OutHit, GetActorLocation(), GetActorLocation(), FQuat(), ObjectTypes, FCollisionShape::MakeBox(FVector(5.0f)), Params);

			if(IsValid(Cast<AConstruction>(OutHit.GetActor())))
				Constructions.AddUnique(Cast<AConstruction>(OutHit.GetActor()));
		}
	}

	Constructions.Remove(this);

	return Constructions;
}

bool AConstruction::IsInSocket(AConstruction* Construction)
{
	FTransformArray* TransformArray = SocketTransforms.Find(Construction->SocketName);
	
	if(!TransformArray)
		return false;

	const FVector ConstructionLocation = Construction->GetActorLocation();
	
	for (FTransform SocketTransform : TransformArray->Transforms)
	{
		if(ConstructionLocation.Equals(SocketTransform.GetLocation(), 5.0))
			return true;
	}
	return false;
}

void AConstruction::UpgradeConstruction_Implementation(FName Action,ACharacter* Character)
{

}

bool AConstruction::CanBeBuilt_Implementation(AActor* TargetActor)
{
	if (HasSupport() && HasNoBlocks())
		return true;
	return false;
}

bool AConstruction::HasSupport_Implementation()
{
	TArray<AConstruction*> SnappedConstructions = GetSnappedConstructions();

	for (const auto SnappedConstruction : SnappedConstructions)
	{
		if(SnappedConstruction->IsInSocket(this))
			return true;
	}
	return false;
}

bool AConstruction::HasCupboardAuthority_Implementation()
{
	TArray<UActorComponent*> BlockingCollisions = GetComponentsByTag(UPrimitiveComponent::StaticClass(), FName("BlockChecker"));
	for (const auto BlockingCollision : BlockingCollisions)
	{
		TArray<AActor*> OverlappingActors;
		Cast<UPrimitiveComponent>(BlockingCollision)->GetOverlappingActors(OverlappingActors);

		for (const auto OverlappingActor : OverlappingActors)
		{
			if (IsValid(Cast<ACupboard>(OverlappingActor)))
			{
				if (!Cast<ACupboard>(OverlappingActor)->CheckAuthority(Cast<ASomewhereCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))->PlayFabId))
				{
					return false;
				}
			}
		}
	}
	return true;
}

bool AConstruction::HasNoBlocks_Implementation()
{
	TArray<UActorComponent*> BlockingCollisions = GetComponentsByTag(UPrimitiveComponent::StaticClass(), FName("BlockChecker"));
	for (const auto BlockingCollision : BlockingCollisions)
	{
		TArray<AActor*> OverlappingActors;
		Cast<UPrimitiveComponent>(BlockingCollision)->GetOverlappingActors(OverlappingActors);
		
		for (const auto OverlappingActor : OverlappingActors)
		{
			// GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Red, *OverlappingActor->GetName());
			if (IsValid(Cast<ACupboard>(OverlappingActor)))
			{
				if (!Cast<ACupboard>(OverlappingActor)->CheckAuthority(Cast<ASomewhereCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))->PlayFabId))
				{
					return false;
				}
			}
			else if (OverlappingActor != this)
			{
				return false;
			}
		}
	}
	return true;
}

bool AConstruction::AdjustPosition_Implementation(AConstruction* LookConstruction)
{
	return true;
}

bool AConstruction_Ramp::HasNoBlocks_Implementation()
{
	TArray<UActorComponent*> BlockingCollisions = GetComponentsByTag(UPrimitiveComponent::StaticClass(), FName("BlockChecker"));
	for (const auto BlockingCollision : BlockingCollisions)
	{
		TArray<AActor*> OverlappingActors;
		Cast<UPrimitiveComponent>(BlockingCollision)->GetOverlappingActors(OverlappingActors);

		for (const auto OverlappingActor : OverlappingActors)
		{
			if (IsValid(Cast<ACupboard>(OverlappingActor)))
			{
				if (!Cast<ACupboard>(OverlappingActor)->CheckAuthority(Cast<ASomewhereCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))->PlayFabId))
				{
					return false;
				}
			}
			else if (OverlappingActor != this && !USomewhereStatics::IsLandscape(OverlappingActor))
			{
				return false;
			}
		}
	}
	return true;
}
