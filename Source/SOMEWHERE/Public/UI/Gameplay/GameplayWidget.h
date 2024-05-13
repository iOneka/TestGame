#pragma once

#include "CoreMinimal.h"
#include "HUD/HUDWidget.h"
#include "Blueprint/UserWidget.h"
#include "Views/Inventory/InventoryView.h"
#include "Engine/DataTable.h"
#include "GameplayWidget.generated.h"
 
/**
 * 
 */
UCLASS(Abstract)
class SOMEWHERE_API UGameplayWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;


	UPROPERTY(BlueprintReadWrite, Category="Gameplay Widget")
	UHUDWidget* HUD;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Gameplay Widget",
		meta=(RequiredAssetDataTags = "RowStructure=/Script/SOMEWHERE.TagToView"))
	UDataTable* ViewsTable;

	/**
	 * Show HUD widget using cool animations
	 */
	UFUNCTION(BlueprintCallable)
	void ShowHUD() const;

	/**
	 * Hide HUD widget using cool animations
	 */
	UFUNCTION(BlueprintCallable)
	void HideHUD() const;

	/**
	 * 
	 */
	UFUNCTION(BlueprintCallable)
	UUserWidget* OpenView(FGameplayTag ViewTag);

	/**
	 * 
	 */
	UFUNCTION(BlueprintCallable)
	void CloseView(UView* View);

	UPROPERTY(BlueprintReadWrite)
	UView* CurrentView;
};
