// 

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "CommonUserWidget.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintAsyncActionBase.h"

#include "InterfaceLayout.generated.h"

struct FStreamableManager;
struct FStreamableHandle;
class UInterfaceLayoutSubsystem;
class UCommonActivatableWidget;
class UCommonActivatableWidgetContainerBase;


USTRUCT(BlueprintType)
struct FPlatformWidgets
{
	GENERATED_BODY()

	TSoftClassPtr<UCommonActivatableWidget> DesktopWidgetClass;
	TSoftClassPtr<UCommonActivatableWidget> MobileWidgetClass;
};


/**
 * 
 */
UCLASS(Abstract, meta = (DisableNativeTick))
class SOMEWHERE_API UInterfaceLayout : public UCommonUserWidget
{
	GENERATED_BODY()

	UInterfaceLayout();

public:
	/* Returns current interface layout */
	UFUNCTION(BlueprintPure, Category="Interface Layout", meta=(WorldContext="WorldContextObject"))
	static UInterfaceLayout* GetInterfaceLayout(const UObject* WorldContextObject);

	/*  */
	UFUNCTION(BlueprintCallable, Category="Interface Layout")
	UCommonActivatableWidget* PushWidgetToLayer(UPARAM(meta=(Categories="UI.Layer")) FGameplayTag LayerTag,
	                                            TSubclassOf<UCommonActivatableWidget> MobileWidgetClass,
	                                            TSubclassOf<UCommonActivatableWidget> DesktopWidgetClass);

	// Find the widget if it exists on any of the layers and remove it from the layer.
	UFUNCTION(BlueprintCallable, Category="Interface Layout")
	void RemoveWidgetFromLayer(UCommonActivatableWidget* ActivatableWidget);

	// Get the layer widget for the given layer tag.
	UFUNCTION(BlueprintPure, Category="Interface Layout")
	UCommonActivatableWidgetContainerBase* GetLayerByTag(UPARAM(meta=(Categories="UI.Layer")) FGameplayTag LayerTag) const;


	void PushWidgetToLayerAsync(FGameplayTag LayerTag, TSoftClassPtr<UCommonActivatableWidget> MobileWidgetClass, TSoftClassPtr<UCommonActivatableWidget> DesktopWidgetClass,
	                            TFunction<void(UCommonActivatableWidget*)> Callback);

protected:
	/** Register a layer that widgets can be pushed onto. */
	UFUNCTION(BlueprintCallable, Category="Interface Layout")
	void RegisterLayer(UPARAM(meta=(Categories="UI.Layer")) FGameplayTag LayerTag, UCommonActivatableWidgetContainerBase* LayerWidget);

private:
	// The registered layers for the layout.
	UPROPERTY(Transient, meta=(Categories="UI.Layer"))
	TMap<FGameplayTag, TObjectPtr<UCommonActivatableWidgetContainerBase>> Layers;
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWidgetPushedToLayerSignature, UCommonActivatableWidget*, ActivatableWidget);

/**
 * 
 */
UCLASS(BlueprintType)
class SOMEWHERE_API UAsyncAction_PushWidgetToLayer : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

	virtual void Activate() override;

public:
	UFUNCTION(BlueprintCallable, Category="Game Layout", meta=(WorldContext = "WorldContextObject", BlueprintInternalUseOnly="true"))
	static UAsyncAction_PushWidgetToLayer* PushWidgetToLayerAsync(UPARAM(meta = (Categories = "UI.Layer")) FGameplayTag LayerTag,
																  TSoftClassPtr<UCommonActivatableWidget> MobileWidgetClass,
																  TSoftClassPtr<UCommonActivatableWidget> DesktopWidgetClass,
																  UObject* WorldContextObject);

	UPROPERTY(BlueprintAssignable)
	FOnWidgetPushedToLayerSignature Completed;

private:
	FGameplayTag LayerTag;
	TSoftClassPtr<UCommonActivatableWidget> MobileWidgetClass;
	TSoftClassPtr<UCommonActivatableWidget> DesktopWidgetClass;

	UPROPERTY()
	UObject* WorldContextObject;

	TSharedPtr<FStreamableHandle> StreamingHandle;
};
