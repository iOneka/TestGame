// Copyright Saeid Gholizade. All Rights Reserved. 2020

#pragma once

#include "CoreMinimal.h"
#include "Toolkits/BaseToolkit.h"
#include "Engine/StaticMesh.h"
#include "AssetThumbnail.h"
#include "PhysicalLayoutPreset.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Input/SSlider.h"

class FPhysicalLayoutMode;

/** Reference Mesh Struct */
struct FReferenceMesh
{
	UPROPERTY()
	UStaticMesh* StaticMesh = nullptr;
	
	float Chance=100;
	bool bIsAdjusting;
	FVector MinRotate;
	FVector MaxRotate;
	FVector MinScale;
	FVector MaxScale;
};

/** Physical Layout Toolkit */
class FPhysicalLayoutToolkit : public FModeToolkit
{
public:

	/** Constrcutor */
	FPhysicalLayoutToolkit();
	
	/** FModeToolkit interface */
	virtual void Init(const TSharedPtr<IToolkitHost>& InitToolkitHost) override;
	// virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& TabManager) override;
	// virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& TabManager) override;
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual class FEdMode* GetEditorMode() const override;
	virtual TSharedPtr<class SWidget> GetInlineContent() const override { return ToolkitWidget; }

	/** Current layout mode */
	FString CurrentLayoutMode;
	
	/** Returns Previous layout mode */
	FString GetLastLayoutMode() const { return LastLayoutMode; }
	
	/** Returns current layout mode */
	FString GetCurrentLayoutMode() const { return CurrentLayoutMode; }

	/** Returns current layout mode text */
	FText GetCurrentLayoutModeText() const { return FText::FromString(CurrentLayoutMode); }

	/** Binds the toolkit commands */
	void BindCommands();

	/** Returns minimum distance between each placed actor */
	float GetMinDistance();

	/** Returns the current actor to place */
	UStaticMesh* GetRandomMesh();

	/** Changes the layout mode by name */
	void ChangeMode(FString InLayoutMode);
	
	/** Changes the layout mode by index */
	void ChangeMode(int InDirection);
	
	/** Returns true if we are selecting the placed actor  */
	bool IsSelectingPlacedActors() { return bSelectPlacedActors; }
	
	/** Returns true if minimum scale random is lock */
	bool IsMinScaleLock () { return bMinScaleLock; }
	
	/** Returns true if maximum scale random is lock */
	bool IsMaxScaleLock () { return bMaxScaleLock; }
	
	/** Returns true if gravity is enable */
	bool IsEnableGravity () { return bEnableGravity; }
	
	/** Returns true if use selectd is enable */
	bool IsUseSelected () { return bUseSelected; }
	
	/** Returns normal distance for hited polygon */
	float GetNormalDistance () { return fNormalDistance; }
	
	/** Returns position randomness */
	FVector GetPositionRandom() { return PositionRandom; }
	
	/** Returns rotatin randomness */
	FRotator GetRotateRandom() { return RotateRandom; }
	
	/** Returns scale randomness */
	FVector GetScaleRandom() { return ScaleRandom; }
	
	/** Returns normal rotation offset */
	FRotator GetNormalRotation() { return NormalRotation; }

	/** Sets the next actor to place */
	void SetRandomMesh();

	/** Sets the layout mode */
	void SetEdMode(FPhysicalLayoutMode* InEdMode) { EdMode = InEdMode; }

	/** Returns true if damping velocity */
	bool IsDamplingVelocity()
	{ return bDampVelocity; }

	/** Sets Damp Velocity */
	void SetDampVelocity(bool InDampVelocity)
	{ bDampVelocity = InDampVelocity; }

	void LoadPLPreset(const FAssetData& InAsset);
private:

	/** Is Velocity Getting Damp */
	bool bDampVelocity;

	/** Previous layout mode */
	FString LastLayoutMode;

	TSharedPtr<SListView<TSharedPtr<FReferenceMesh>>> ReferenceMeshesListView; 
	/** List of Reference meshes */
	TArray<TSharedPtr<FReferenceMesh>> ReferenceMeshes;
	
	/** Minimum position random */
	FVector MinPositionRandom = FVector::ZeroVector;
	
	/** Maximum position random */
	FVector MaxPositionRandom = FVector::ZeroVector;
	
	/** Position random */
	FVector PositionRandom = FVector::ZeroVector;

	/** Minimum rotation random */
	FRotator MinRotateRandom = FRotator::ZeroRotator;
	
	/** Maximum rotation random */
	FRotator MaxRotateRandom = FRotator::ZeroRotator;
	
	/** Rotation random */
	FRotator RotateRandom = FRotator::ZeroRotator;

	/** Minimum scale random */
	FVector MinScaleRandom = FVector::OneVector;
	
	/** Maximum scale random */
	FVector MaxScaleRandom = FVector::OneVector;
	
	/** Scale random */
	FVector ScaleRandom = FVector::OneVector;

	/** Nornaml rotation offset */
	FRotator NormalRotation = FRotator::ZeroRotator;

	/** Layout mode */
	FPhysicalLayoutMode* EdMode = nullptr;

	/** Normal distance for hited polygon */
	float fNormalDistance = 0;

	/** Selected reference mesh index */
	int SelectedMeshIndex = -1;
	
	/** Minimum distance to last placed actor */
	float fMinDistance = 2;
	
	/** Is select placed actor enable */
	bool bSelectPlacedActors = true;
	
	/** Is minimum scale lock */
	bool bMinScaleLock = false;
	
	/** Is maximum scale lock */
	bool bMaxScaleLock = false;
	
	/** Is gravity enable */
	bool bEnableGravity = false;
	
	/** Is use selected enable */
	bool bUseSelected = false;

	/** Layout mode name */
	FString LayoutMode;

	/** Selected reference mesh */
	UStaticMesh* PickedMesh = nullptr;
	
	/** Is percents are relative */
	bool bIsPercentRelative;
	
	/** SSlider references for reference mesh chance */
	TMap<TSharedPtr<FReferenceMesh>, TSharedPtr<SSlider>> Sliders;
	
	/** Layout mode SCombobox reference */
	TSharedPtr<SComboBox<TSharedPtr<FString>>> LayoutModeComboBox;
	
	/** Layout mode names */
	TArray<TSharedPtr<FString>> LayoutModes;
	
	/** Toolkit's SWidget */
	TSharedPtr<SWidget> ToolkitWidget;
	
	/** Current layout mode index */
	int LayoutModeIndex = 0;
	
	/** Reference mesh thumbnail pool */
	TSharedPtr<FAssetThumbnailPool> ThumbnailPool;
	
	/** Generates the toolkit widget */
	TSharedRef<SWidget> GenerateWidget(TSharedPtr<FString> InItem);
	
	/** Layout mode change event */
	void ModeChanged(TSharedPtr<FString> Item, ESelectInfo::Type SelectInfo);
	
	/** Bakes the places actors into instance mesh */
	void BakeToInstanceMesh(bool BakeSelected);
	
	/** Returns reference meshes */
	TArray<TSharedPtr<FReferenceMesh>> GetReferenceMeshes() const;
	
	/** Reference mesh widget delegate */
	TSharedRef<ITableRow> GetReferenceMeshWidget(TSharedPtr<FReferenceMesh> InItem, const TSharedRef<STableViewBase>& OwnerTable);
	
	/** Creates select mode widget */
	TSharedPtr<SWidget> CreateSelectModeWidget();
	
	/** Creates Paint select mode widget */
	TSharedPtr<SWidget> CreatePaintSelectModeWidget();
	
	/** Creates transfrom mode widget */
	TSharedPtr<SWidget> CreateTransformModeWidget();

	FReply SavePLPreset();
	/** Creates paint mode widget */
	TSharedPtr<SWidget> CreatePaintModeWidget();
	
	/** Creates shared widgets */
	TSharedPtr<SWidget> CreateSharedWidget();

	UPhysicalLayoutPreset* PLPreset = nullptr;
};
