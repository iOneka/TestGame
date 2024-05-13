// Copyright Saeid Gholizade. All Rights Reserved. 2020

#include "PhysicalLayoutMode.h"
#include "Engine/World.h"
#include "PhysicalLayoutToolkit.h"
#include "Toolkits/ToolkitManager.h"
#include "EditorModeManager.h"
#include "ToolContextInterfaces.h"
#include "Engine/Selection.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "UnrealEdGlobals.h"
#include "Editor/UnrealEdEngine.h"
#include "Kismet/GameplayStatics.h"
#include "Editor.h"
#include "EditorViewportClient.h"
#include "SceneView.h"
#include "Engine/StaticMeshActor.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Selection.h"
#include "Chaos/PBDRigidsEvolutionFwd.h"
#include "PBDRigidsSolver.h"
#include "GeometryCollection/GeometryCollectionActor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"


#define LOCTEXT_NAMESPACE "FPhysicalLayoutMode"

const FEditorModeID FPhysicalLayoutMode::EM_PhysicalLayoutModeId = TEXT("EM_PhysicalLayoutMode");

FPhysicalLayoutMode::FPhysicalLayoutMode()
{
	UMaterial* BrushMaterial = LoadObject<UMaterial>(nullptr, TEXT("/Engine/EditorLandscapeResources/FoliageBrushSphereMaterial.FoliageBrushSphereMaterial"), nullptr, LOAD_None, nullptr);
	BrushMI = UMaterialInstanceDynamic::Create(BrushMaterial, GetTransientPackage());
	BrushMI->SetVectorParameterValue(TEXT("HighlightColor"), FLinearColor::Blue);
	check(BrushMI != nullptr);

	Brush = NewObject<UStaticMeshComponent>(GetTransientPackage(), TEXT("SphereBrushComponent"));
	Brush->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	Brush->SetCollisionObjectType(ECC_WorldDynamic);
	Brush->SetMaterial(0, BrushMI);
	Brush->SetAbsolute(true, true, true);
	Brush->CastShadow = false;
}

void FPhysicalLayoutMode::OnLevelActorsAdded(AActor* InActor)
{
	if (InActor->IsA<AStaticMeshActor>())
	{
		if (InActor->GetLevel() == GetWorld()->GetCurrentLevel())
		{
			if (!SpawnedActors.Contains(InActor))
			{
				UpdatePhysics(InActor, ToolkitPtr->IsEnableGravity());
				SpawnedActors.Add(InActor);

			}
		}
	}
}

void FPhysicalLayoutMode::OnLevelActorsDeleted(AActor* InActor)
{
	if (SpawnedActors.Contains(InActor))
	{
		SpawnedActors.Remove(InActor);
	}
}

void FPhysicalLayoutMode::OnPreBeginPie(bool InStarted)
{
	GetModeManager()->ActivateMode(EM_PhysicalLayoutModeId, true);
}

void FPhysicalLayoutMode::Enter()
{
	FEdMode::Enter();
	GEngine->OnLevelActorAdded().AddSP(this, &FPhysicalLayoutMode::OnLevelActorsAdded);
	GEngine->OnLevelActorDeleted().AddSP(this, &FPhysicalLayoutMode::OnLevelActorsDeleted);
	FEditorDelegates::PreBeginPIE.AddSP(this, &FPhysicalLayoutMode::OnPreBeginPie);

	if (IsValid(Brush))
	{
		Brush->SetVisibility(true);
		if (!Brush->IsRegistered())
		{
			Brush->RegisterComponentWithWorld(GetWorld());
		}
	}

	if (!Toolkit.IsValid() && UsesToolkits())
	{
		ToolkitPtr = MakeShareable(new FPhysicalLayoutToolkit);
		Toolkit = ToolkitPtr;
		ToolkitPtr.Get()->SetEdMode(this);
		Toolkit->Init(Owner->GetToolkitHost());
	}

	UpdateSelectionPhysics();
}

void FPhysicalLayoutMode::Exit()
{
	GEngine->OnLevelActorAdded().RemoveAll(this);
	GEngine->OnLevelActorDeleted().RemoveAll(this);
	FEditorDelegates::PreBeginPIE.RemoveAll(this);

	ResetPhysics();

	Mobilities.Reset();
	Gravities.Reset();
	Physics.Reset();
	LevelActors.Reset();
	LastSpawnedActors.Reset();
	SpawnedActors.Reset();

	Brush->UnregisterComponent();
	Brush->SetStaticMesh(nullptr);
	GetWorld()->FinishPhysicsSim();

	if (Toolkit.IsValid())
	{
		FToolkitManager::Get().CloseToolkit(Toolkit.ToSharedRef());
		Toolkit.Reset();
	}

	// Call base Exit method to ensure proper cleanup
	FEdMode::Exit();
}

void FPhysicalLayoutMode::ActorSelectionChangeNotify()
{
	FEdMode::ActorSelectionChangeNotify();
	
	if (ToolkitPtr.IsValid())
	{
		if (ToolkitPtr.Get()->IsSelectingPlacedActors())
		{
			TArray<AActor*> Actors;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), Actors);
			for (auto& Actor : Actors)
			{
				if (!SpawnedActors.Contains(Actor))
				{
					GEditor->SelectActor(Actor, false, false);
				}
			}
		}
	}

	TArray<AActor*> Actors = GetSelectedActors();
	bool SelectionChanged = false;
	for (auto& Actor : Actors)
	{
		if (!LastSelectedActors.Contains(Actor))
		{
			SelectionChanged = true;
			break;
		}
	}
	
	if (SelectionChanged)
	{
		LastSelectedActors = GetSelectedActors();
		UpdateSelectionPhysics();
	}
}

void FPhysicalLayoutMode::Tick(FEditorViewportClient* ViewportClient, float DeltaTime)
{
	if (GEditor->IsSimulateInEditorInProgress() || GEditor->IsPlaySessionInProgress()
		|| GEditor->IsPlaySessionRequestQueued())
	{
		GetModeManager()->ActivateMode(EM_PhysicalLayoutModeId, true);
		return;
	}

	if (ToolkitPtr.IsValid())
	{
		FHitResult Hit;
		bool Hited = Trace(Hit, ViewportClient);

		if (ToolkitPtr.Get()->GetCurrentLayoutMode() == ELayoutMode::PaintSelect)
		{
			if (Hited && bIsPainting)
			{
				if (ToolkitPtr.Get()->IsSelectingPlacedActors())
				{
					if (SpawnedActors.Contains(Hit.GetActor()))
					{
						GEditor->SelectActor(Hit.GetActor(), !bIsCtrlDown, false);
					}
				}
				else
				{
					GEditor->SelectActor(Hit.GetActor(), !bIsCtrlDown, false);
				}
			}
		}
		else if (ToolkitPtr.Get()->GetCurrentLayoutMode() == ELayoutMode::Paint)
		{
			if (Hited)
			{
				auto Rotation = ToolkitPtr.Get()->GetRotateRandom();
				auto PickedMesh = ToolkitPtr.Get()->GetRandomMesh();

				if (IsValid(PickedMesh))
				{
					if (IsValid(Brush))
					{
						if (!Brush->IsRegistered())
						{
							Brush->RegisterComponentWithWorld(GetWorld());
						}

						Brush->SetStaticMesh(PickedMesh);
						Brush->SetWorldLocation(GetPosition());
						Brush->SetWorldScale3D(ToolkitPtr.Get()->GetScaleRandom());

						if (bIsShiftDown && bIsCtrlDown)
						{
							Rotation = UKismetMathLibrary::FindLookAtRotation(
								BrushPosition,
								BrushPosition + BrushDirection) + ToolkitPtr.Get()->GetNormalRotation();
						}
						else if (bIsShiftDown)
						{
							Rotation = UKismetMathLibrary::FindLookAtRotation(
								BrushPosition,
								BrushPosition + BrushNormal) + ToolkitPtr.Get()->GetNormalRotation();
						}

						Brush->SetWorldRotation(Rotation);
					}
					else
					{
						Brush->SetVisibility(false);
					}

					if (bIsPainting)
					{
						if (bIsCtrlDown && !bIsShiftDown && !bIsQDown)
						{
							if (SpawnedActors.Contains(Hit.GetActor()))
							{
								GetWorld()->DestroyActor(Hit.GetActor());
							}
						}
						else
						{
							auto ReferenceMesh = ToolkitPtr.Get()->GetRandomMesh();
							if (IsValid(ReferenceMesh))
							{
								float dist = FVector::Dist(GetPosition(), LastSpawnedPosition);
								if (dist > ToolkitPtr.Get()->GetMinDistance())
								{
									LastSpawnedPosition = GetPosition();

									FActorSpawnParameters Params = FActorSpawnParameters();
									FString name = FString::Format(TEXT("Actor_{0}"), { ReferenceMesh->GetFName().ToString() });
									FName fname = MakeUniqueObjectName(nullptr, AStaticMeshActor::StaticClass(), FName(*name));
									Params.Name = fname;
									Params.NameMode = FActorSpawnParameters::ESpawnActorNameMode::Requested;

									AStaticMeshActor* actor = GetWorld()->SpawnActor<AStaticMeshActor>(GetPosition(), Rotation, Params);

									actor->SetActorLabel(fname.ToString());
									actor->SetActorScale3D(ToolkitPtr.Get()->GetScaleRandom());
									LastSpawnedActors.Add(actor);
									actor->GetStaticMeshComponent()->SetStaticMesh(ReferenceMesh);

									UpdatePhysics(actor, ToolkitPtr.Get()->IsEnableGravity());
									ToolkitPtr.Get()->SetRandomMesh();
								}
							}
						}

						if (IsValid(Brush))
						{
							if (!Brush->IsRegistered())
							{
								Brush->RegisterComponentWithWorld(GetWorld());
							}
							Brush->SetStaticMesh(ToolkitPtr.Get()->GetRandomMesh());
						}
					}
				}
			}
		}
		auto World = GetWorld();
		auto Solver = World->GetPhysicsScene()->GetSolver();

		if (ToolkitPtr.Get()->GetCurrentLayoutMode() == ELayoutMode::Transform ||
			ToolkitPtr.Get()->GetCurrentLayoutMode() == ELayoutMode::Paint)
		{
			TArray<AActor*> Actors = GetSelectedActors();
			// GetWorld()->GetPhysicsScene()->StartFrame();
			Solver->StartingSceneSimulation();

			for (auto& SelectedActor : Actors)
            {

            	bool DampVelocity = ToolkitPtr->IsDamplingVelocity();
            	if (!ToolkitPtr->IsDamplingVelocity())
            	{
            		DampVelocity = GetCurrentWidgetAxis() != EAxisList::None && GetWidgetLocation() == SelectedActor->GetActorLocation();
            	}
            	if (DampVelocity)
            	{
            		TArray<UPrimitiveComponent*> Prims = GetPrimitives(SelectedActor);
            		for (auto& Prim : Prims)
            		{
            			Prim->SetPhysicsLinearVelocity(FVector::ZeroVector);
            			Prim->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
            		}
            	}
            }

            UGameplayStatics::GetAllActorsOfClass(World, AActor::StaticClass(), LevelActors);
            for (auto& LevelActor : LevelActors)
            {
				if (AGeometryCollectionActor* CollectionActor = Cast<AGeometryCollectionActor>(LevelActor))
				{
					auto CollectionComponent = CollectionActor->GetGeometryCollectionComponent();
					
//					World->GetPhysicsScene()->RemoveObject(CollectionComponent->GetPhysicsProxy());
				}
				else
				{

					// World->GetPhysicsScene()->RemoveObject

					bool DampVelocity = ToolkitPtr->IsDamplingVelocity();
					if (!ToolkitPtr->IsDamplingVelocity())
					{
						DampVelocity = GetCurrentWidgetAxis() != EAxisList::None && GetWidgetLocation() == LevelActor->GetActorLocation();
					}

					if (DampVelocity)
					{
						auto Prims = GetPrimitives(LevelActor);
						for (auto& Prim : Prims)
						{
							Prim->SetPhysicsLinearVelocity(FVector::ZeroVector);
							Prim->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
						}
					}
				}
            }
			Solver->AdvanceAndDispatch_External(DeltaTime);
			/*World->Tick(ELevelTick::LEVELTICK_All, DeltaTime);
			if (World->GetPhysicsScene()->IsCompletionEventComplete())
			{
				World->bShouldSimulatePhysics = false;
				World->FinishPhysicsSim();
			}*/
			// Solver->CompleteSceneSimulation();
			// GetWorld()->FinishPhysicsSim();

		}

		if (ToolkitPtr.Get()->GetCurrentLayoutMode() != ELayoutMode::Transform)
		{
			DrawDebugLine(GetWorld(), BrushPosition, (BrushPosition + BrushDirection), BrushColor.ToFColor(false), false, -1, 0, 5);
		}
	}
}

bool FPhysicalLayoutMode::InputDelta(FEditorViewportClient* InViewportClient, FViewport* InViewport, FVector& InDrag, FRotator& InRot, FVector& InScale)
{
	bool handled = FEdMode::InputDelta(InViewportClient, InViewport, InDrag, InRot, InScale);
	
	if (!InScale.IsZero())
	{
		auto Prims = GetSelectedPrimitives();
		for (auto& Prim : Prims)
		{
			FVector Pivot = GetWidgetLocation();
			FVector dir = (Prim->GetComponentLocation() - Pivot) * 100;
			float sign = InScale.X + InScale.Y + InScale.Z;
			Prim->AddForce(sign * dir, NAME_None, true);
		}

		handled = true;
	}

	return handled;
}

bool FPhysicalLayoutMode::MouseMove(FEditorViewportClient* ViewportClient, FViewport* Viewport, int32 x, int32 y)
{
	CursorPosition.Set(x, y);
	return FEdMode::MouseMove(ViewportClient, Viewport, x, y);
}

bool FPhysicalLayoutMode::StartTracking(FEditorViewportClient* InViewportClient, FViewport* InViewport)
{
	GEditor->EndTransaction();
	GEditor->BeginTransaction(LOCTEXT("PhysicalLayoutMode_Transformation", "Transformation"));
	
	return FEdMode::StartTracking(InViewportClient, InViewport);
}

bool FPhysicalLayoutMode::EndTracking(FEditorViewportClient* InViewportClient, FViewport* InViewport)
{
	GEditor->EndTransaction();
	GEditor->NoteSelectionChange();
	
	return 	FEdMode::EndTracking(InViewportClient, InViewport);
}

bool FPhysicalLayoutMode::InputKey(FEditorViewportClient* ViewportClient, FViewport* Viewport, FKey Key, EInputEvent Event)
{
	bIsShiftDown = IsShiftDown(Viewport);
	
	bool bHandled = false;
	
	if (bIsCtrlDown != IsCtrlDown(Viewport))
	{
		bIsCtrlDown = IsCtrlDown(Viewport);
	}

	if (ToolkitPtr.IsValid())
	{
		if (Key == EKeys::Tab && Event == EInputEvent::IE_Pressed)
		{
			ToolkitPtr.Get()->ChangeMode(bIsShiftDown ? -1 : 1);
			bHandled = true;
		}

		if (Key == EKeys::Q && Event == EInputEvent::IE_Pressed)
		{
			bHandled = true;
			bIsQDown = true;

			ToolkitPtr.Get()->ChangeMode(ELayoutMode::PaintSelect);
		}
		else if (Key == EKeys::Q && Event == EInputEvent::IE_Released)
		{
			bIsQDown = false;
			ToolkitPtr.Get()->ChangeMode(ToolkitPtr.Get()->GetLastLayoutMode());
		}

		if (ToolkitPtr.Get()->GetCurrentLayoutMode() == ELayoutMode::Paint)
		{
			if (!bIsCtrlDown || (bIsCtrlDown && bIsShiftDown))
			{
				BrushColor = ELayoutModeColor::Add;
				Brush->SetVisibility(true);
			}
			else
			{
				Brush->SetVisibility(false);
				BrushColor = ELayoutModeColor::Remove;

			}
		}
		else if (ToolkitPtr.Get()->GetCurrentLayoutMode() == ELayoutMode::PaintSelect ||
				 ToolkitPtr.Get()->GetCurrentLayoutMode() == ELayoutMode::Select)
		{
			Brush->SetVisibility(false);

			if (!bIsCtrlDown)
			{
				BrushColor = ELayoutModeColor::Select;
			}
			else
			{
				BrushColor = ELayoutModeColor::Deselect;
			}
		}
		else
		{
			Brush->SetVisibility(false);
		}

		BrushMI->SetVectorParameterValue(TEXT("HighlightColor"), BrushColor);

		if (Key == EKeys::LeftMouseButton && Event == IE_Pressed && !IsAltDown(Viewport))
		{

			if (ToolkitPtr && ToolkitPtr.Get()->GetCurrentLayoutMode() == ELayoutMode::Paint ||
				ToolkitPtr && ToolkitPtr.Get()->GetCurrentLayoutMode() == ELayoutMode::PaintSelect)
			{
				bHandled = true;
			}

			bIsPainting = true;

			if (bIsQDown)
			{
				GEditor->EndTransaction();
				GEditor->BeginTransaction(LOCTEXT("PhysicalLayoutMode_Paint", "Select Objects"));
			}
			else
			{
				GEditor->EndTransaction();
				GEditor->BeginTransaction(LOCTEXT("PhysicalLayoutMode_Paint", "Add Objects"));
			}
		}

		if (Key == EKeys::LeftMouseButton && Event == IE_Released && !IsAltDown(Viewport))
		{
			if (bIsPainting)
			{
				GEditor->EndTransaction();
			}

			bIsPainting = false;
			LastSpawnedActors.Reset();
		}
	}

	return bHandled? bHandled : FEdMode::InputKey(ViewportClient, Viewport, Key, Event);;
}

bool FPhysicalLayoutMode::MouseEnter(FEditorViewportClient* ViewportClient, FViewport* Viewport, int32 x, int32 y)
{
	return FEdMode::MouseEnter(ViewportClient, Viewport, x, y);;
}

bool FPhysicalLayoutMode::ProcessCapturedMouseMoves(FEditorViewportClient* InViewportClient, FViewport* InViewport, const TArrayView<FIntPoint>& CapturedMouseMoves)
{
	bool bHandled = FEdMode::ProcessCapturedMouseMoves(InViewportClient, InViewport, CapturedMouseMoves);

	for (auto& move : CapturedMouseMoves)
	{
		CursorPosition.Set(move.X, move.Y);
	}

	return bHandled;
}

bool FPhysicalLayoutMode::Trace(FHitResult& outHits, FEditorViewportClient *InViewportClient)
{
	FSceneViewFamilyContext ViewContext(FSceneViewFamilyContext::ConstructionValues(
				InViewportClient->Viewport, 
				InViewportClient->GetScene(), 
				InViewportClient->EngineShowFlags)
				.SetRealtimeUpdate(InViewportClient->IsRealtime()));

	FSceneView* View = InViewportClient->CalcSceneView(&ViewContext);
	FViewportCursorLocation Cursor(View, InViewportClient, CursorPosition.X, CursorPosition.Y);

	FCollisionQueryParams Params = FCollisionQueryParams::DefaultQueryParam;
	Params.AddIgnoredActors(LastSpawnedActors);

	bool Hited = GetWorld()->LineTraceSingleByChannel(
		outHits, 
		Cursor.GetOrigin(), 
		Cursor.GetOrigin() + Cursor.GetDirection() * HALF_WORLD_MAX, 
		ECollisionChannel::ECC_Visibility, Params);

	if (Hited)
	{
		BrushLastPosition = BrushPosition;
		BrushPosition = outHits.ImpactPoint;
		BrushDirection = BrushLastPosition - BrushPosition;
		BrushNormal = outHits.ImpactNormal;
	}

	return Hited;
}

void FPhysicalLayoutMode::AddReferencedObjects(FReferenceCollector& Collector)
{
	FEdMode::AddReferencedObjects(Collector);
	Collector.AddReferencedObject(Brush);
}

bool FPhysicalLayoutMode::ShowModeWidgets() const
{
	if (ToolkitPtr.IsValid())
	{
		if (ToolkitPtr.Get()->GetCurrentLayoutMode() == ELayoutMode::Transform)
		{
			return true;
		}
	}

	return false;
}

bool FPhysicalLayoutMode::UsesTransformWidget() const
{
	if (ToolkitPtr.IsValid())
	{
		if (ToolkitPtr.Get()->GetCurrentLayoutMode() == ELayoutMode::Transform)
		{
			return true;
		}
	}

	return false;
}

FVector FPhysicalLayoutMode::GetWidgetLocation() const
{
	if (LastSelectedActors.Num() > 0)
	{
		if (LastSelectedActors.Last())
		{
			return LastSelectedActors.Last()->GetActorLocation();
		}
	}
	return FEdMode::GetWidgetLocation();
}

bool FPhysicalLayoutMode::UsesToolkits() const
{
	return true;
}

void FPhysicalLayoutMode::RegisterBrush()
{
	if (ToolkitPtr.IsValid())
	{
		if (Brush && !Brush->IsRegistered())
		{
			Brush->RegisterComponentWithWorld(GetWorld());
			auto PickedMesh = ToolkitPtr.Get()->GetRandomMesh();

			if (IsValid(PickedMesh))
			{
				Brush->SetStaticMesh(PickedMesh);
				Brush->SetVisibility(true);
			}
		}
	}
}

void FPhysicalLayoutMode::OnLayoutModeChange(FString InMode)
{
	if (InMode == ELayoutMode::Transform)
	{
		GEditor->NoteSelectionChange();
	}
}

FVector FPhysicalLayoutMode::GetPosition()
{
	if (ToolkitPtr.IsValid())
	{
		return BrushPosition + (BrushNormal * ToolkitPtr.Get()->GetNormalDistance()) + ToolkitPtr.Get()->GetPositionRandom();
	}

	return BrushPosition;
}

TArray<UPrimitiveComponent*> FPhysicalLayoutMode::GetPrimitives(const AActor* InActor)
{
	TArray<UPrimitiveComponent*> Comps;
	InActor->GetComponents<UPrimitiveComponent>(Comps, false);
	TArray<UPrimitiveComponent*> prims;

	for (auto Comp : Comps)
	{
		if (Comp->IsA<UPrimitiveComponent>())
		{
			UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(Comp);
			if (Prim)
			{
				prims.Add(Prim);
			}
		}
	}

	return prims;
}

void FPhysicalLayoutMode::DestroyActors(bool InSelected)
{
	TArray<AActor*> Actors;

	if (InSelected)
	{
		Actors = GetSelectedActors();
	}
	else
	{
		Actors = GetSpawnedActors();
	}

	for (auto& Actor : Actors)
	{
		SpawnedActors.Remove(Actor);
		LevelActors.Remove(Actor);
		Actor->Destroy();
	}
}

TArray<UPrimitiveComponent*> FPhysicalLayoutMode::GetSelectedPrimitives()
{
	TArray<UPrimitiveComponent*> Prims;
	USelection* SelectedActors = GEditor->GetSelectedActors();

	// For each selected actor
	for (FSelectionIterator Iter(*SelectedActors); Iter; ++Iter)
	{
		if (AActor* LevelActor = Cast<AActor>(*Iter))
		{

			auto comps = LevelActor->GetComponents();
			TArray<UPrimitiveComponent*> prims;

			for (auto& comp : comps)
			{
				if (!comp->IsA<UGeometryCollectionComponent>())
				{
					if (comp->IsA<UPrimitiveComponent>())
					{
						UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(comp);
						if (Prim)
						{
							Prims.Add(Prim);
						}
					}
				}
			}
		}
	}

	return Prims;
}

void FPhysicalLayoutMode::UnregisterBrush()
{
	if (Brush && Brush->IsRegistered())
	{
		Brush->SetVisibility(false);
		Brush->UnregisterComponent();

	}
}

TArray<AActor*> FPhysicalLayoutMode::GetSelectedActors()
{
	TArray<AActor*> Actors;
	USelection* SelectedActors = GEditor->GetSelectedActors();

	// For each selected actor
	for (FSelectionIterator Iter(*SelectedActors); Iter; ++Iter)
	{
		if (AActor* LevelActor = Cast<AActor>(*Iter))
		{
			Actors.Add(LevelActor);
		}
	}

	return Actors;
}

void FPhysicalLayoutMode::UpdatePhysics(AActor* InActor, bool bInEnableGravity)
{
	auto Prims = GetPrimitives(InActor);
	for (auto& Prim : Prims)
	{
		if (!Mobilities.Contains(Prim))
		{
			Mobilities.Add(Prim, Prim->Mobility.GetValue());
		}
		if (!Physics.Contains(Prim))
		{
			Physics.Add(Prim, Prim->IsSimulatingPhysics());
		}
		if (!Gravities.Contains(Prim))
		{
			Gravities.Add(Prim, Prim->IsGravityEnabled());
		}
		if (!Positions.Contains(Prim))
		{
			Positions.Add(Prim, Prim->GetComponentLocation());
		}
		if (!Rotations.Contains(Prim))
		{
			Rotations.Add(Prim, Prim->GetComponentRotation());
		}

		Prim->SetSimulatePhysics(true);
		Prim->SetEnableGravity(bInEnableGravity);
		Prim->SetMobility(EComponentMobility::Movable);

	}
}

void FPhysicalLayoutMode::MakeSelectedStatic()
{
	auto Actors = GetSelectedActors();
	for (auto& Actor : Actors)
	{
		GEditor->SelectActor(Actor, false, true);

		auto Prims = GetPrimitives(Actor);
		for (auto& Prim : Prims)
		{
			ResetPrimitivePhysics(Prim, false, true);
		}
		
		SpawnedActors.Remove(Actor);
		LastSpawnedActors.Remove(Actor);
	}
}

void FPhysicalLayoutMode::SelectPlacedActors(UStaticMesh* InStaticMesh)
{
	GEditor->SelectNone(false, true, false);
	if (InStaticMesh)
	{
		auto Components = GetSpawnedComponents();
		for (auto& Component : Components)
		{
			UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(Component);
			if (StaticMeshComponent)
			{
				if (StaticMeshComponent->GetStaticMesh() == InStaticMesh)
				{
					GEditor->SelectActor(Component->GetOwner(),true, false);
				}
			}
		}
	}
	else
	{
		for (auto& Actor : SpawnedActors)
		{
			GEditor->SelectActor(Actor,true, false);
		}
	}
}

void FPhysicalLayoutMode::AddSelectedActor(AActor* InActor)
{
	OnLevelActorsAdded(InActor);
}

void FPhysicalLayoutMode::UpdateSelectionPhysics()
{
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), LevelActors);
	
	for (auto& actor : LevelActors)
	{
		if (!actor->IsA<AGeometryCollectionActor>())
		{

			auto Prims = GetPrimitives(actor);
			for (auto& Prim : Prims)
			{
				if (!Mobilities.Contains(Prim))
				{
					Mobilities.Add(Prim, Prim->Mobility.GetValue());
				}

				if (!Physics.Contains(Prim))
				{
					Physics.Add(Prim, Prim->IsSimulatingPhysics());
				}

				if (!Gravities.Contains(Prim))
				{
					Gravities.Add(Prim, Prim->IsGravityEnabled());
				}

				if (!Positions.Contains(Prim))
				{
					Positions.Add(Prim, Prim->GetComponentLocation());
				}

				if (!Rotations.Contains(Prim))
				{
					Rotations.Add(Prim, Prim->GetComponentRotation());
				}

				Prim->SetMobility(EComponentMobility::Static);
			}
		}
	}

	auto Prims = GetSelectedPrimitives();
	for (auto& Prim : Prims)
	{
		if (!Mobilities.Contains(Prim))
		{
			Mobilities.Add(Prim, Prim->Mobility.GetValue());
		}

		if (!Physics.Contains(Prim))
		{
			Physics.Add(Prim, Prim->IsSimulatingPhysics());
		}

		if (!Gravities.Contains(Prim))
		{
			Gravities.Add(Prim, Prim->IsGravityEnabled());
		}

		Prim->SetSimulatePhysics(true);
		Prim->SetEnableGravity(false);
		Prim->SetMobility(EComponentMobility::Movable);
	}
}

void FPhysicalLayoutMode::ResetTransform()
{
	USelection* SelectedActors = GEditor->GetSelectedActors();

	auto Prims = GetSelectedPrimitives();
	for (auto& Prim : Prims)
	{
		ResetPrimitivePhysics(Prim, true);
	}

	SelectedActors->DeselectAll();
}

TArray<UPrimitiveComponent*> FPhysicalLayoutMode::GetSpawnedComponents()
{ 
	TArray<UPrimitiveComponent*> Components;
	for (auto& Actor : SpawnedActors)
	{
		Components.Append(GetPrimitives(Actor));
	}
	return Components;
}

void FPhysicalLayoutMode::ResetPhysics()
{
	TArray<UPrimitiveComponent*> Prims;
	Mobilities.GetKeys(Prims);

	for (auto& Prim : Prims)
	{
		ResetPrimitivePhysics(Prim, false);
	}
}

void FPhysicalLayoutMode::ResetPrimitivePhysics(UPrimitiveComponent* InPrim, bool bResetTransform, bool bForceStatic)
{
	if (InPrim && IsValid(InPrim))
	{
		if (Physics.Contains(InPrim))
		{
			InPrim->SetSimulatePhysics(bForceStatic ? false : Physics[InPrim]);
		}

		if (Gravities.Contains(InPrim))
		{
			InPrim->SetEnableGravity(bForceStatic ? false : Gravities[InPrim]);
		}

		if (Mobilities.Contains(InPrim))
		{
			InPrim->SetMobility(bForceStatic ? EComponentMobility::Static : Mobilities[InPrim]);
		}

		if (bResetTransform)
		{
			if (Positions.Contains(InPrim))
			{
				InPrim->SetWorldLocation(Positions[InPrim]);
			}

			if (Rotations.Contains(InPrim))
			{
				InPrim->SetWorldRotation(Rotations[InPrim]);
			}
		}
	}

}


#undef LOCTEXT_NAMESPACE
