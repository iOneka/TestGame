// 

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SyncLoadingScreenSubsystem.generated.h"

class ULoadingScreenWidget;
/**
 * 
 */
UCLASS()
class SYNCLOADINGSCREEN_API USyncLoadingScreenSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable)
	void CreateLoadingScreenWidget(TSubclassOf<ULoadingScreenWidget> LoadingScreenClass, const FName LevelName);

	UFUNCTION(BlueprintCallable)
	void PrepareForTransition(bool CustomBool, int CustomInt, const FString& CustomString);
	
	UPROPERTY(BlueprintReadOnly)
	UTexture2D* TransitionTexture;

	UPROPERTY()
	bool SavedCustomBool;

	UPROPERTY()
	int SavedCustomInt;

	UPROPERTY()
	FString SavedCustomString;

protected:
	UPROPERTY()
	bool bIsLoading;
	
	UPROPERTY()
	ULoadingScreenWidget* LoadingWidget;
	
	UPROPERTY()
	TSubclassOf<ULoadingScreenWidget> LoadingWidgetClass;
	
	void OnPostLoadMapWithWorld(UWorld* World);

	static UTexture2D* CreateTextureFromRenderTarget(UTextureRenderTarget2D* RenderTarget);
};
