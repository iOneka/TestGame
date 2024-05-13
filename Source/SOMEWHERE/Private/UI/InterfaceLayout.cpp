// 


#include "UI/InterfaceLayout.h"

#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Library/PlatformStatics.h"
#include "UI/InterfaceLayoutSubsystem.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

UInterfaceLayout::UInterfaceLayout()
{
}

UInterfaceLayout* UInterfaceLayout::GetInterfaceLayout(const UObject* WorldContextObject)
{
	const UWorld* World = WorldContextObject->GetWorld();
	if (!IsValid(World))
		return nullptr;

	const ULocalPlayer* LocalPlayer = World->GetFirstLocalPlayerFromController();
	if (!IsValid(LocalPlayer))
		return nullptr;

	const UInterfaceLayoutSubsystem* GameLayoutSubsystem = LocalPlayer->GetSubsystem<UInterfaceLayoutSubsystem>();
	if (!IsValid(GameLayoutSubsystem))
		return nullptr;

	return GameLayoutSubsystem->CurrentInterfaceLayout;
}

UCommonActivatableWidget* UInterfaceLayout::PushWidgetToLayer(FGameplayTag LayerTag,
                                                              TSubclassOf<UCommonActivatableWidget> MobileWidgetClass,
                                                              TSubclassOf<UCommonActivatableWidget> DesktopWidgetClass)
{
	UCommonActivatableWidgetContainerBase* Layer = GetLayerByTag(LayerTag);
	if (!Layer)
		return nullptr;

	switch (UPlatformStatics::GetPlatform(this))
	{
	case Mobile:
		return Layer->AddWidget(MobileWidgetClass);
	case Desktop:
		return Layer->AddWidget(DesktopWidgetClass);
	case Console:
		return Layer->AddWidget(DesktopWidgetClass);
	default:
		return nullptr;
	}
}

void UInterfaceLayout::RemoveWidgetFromLayer(UCommonActivatableWidget* ActivatableWidget)
{
	for (const auto& LayerKVP : Layers)
	{
		LayerKVP.Value->RemoveWidget(*ActivatableWidget);
	}
}

UCommonActivatableWidgetContainerBase* UInterfaceLayout::GetLayerByTag(const FGameplayTag LayerTag) const
{
	return Layers.FindRef(LayerTag);
}


void UInterfaceLayout::PushWidgetToLayerAsync(FGameplayTag LayerTag, TSoftClassPtr<UCommonActivatableWidget> MobileWidgetClass, TSoftClassPtr<UCommonActivatableWidget> DesktopWidgetClass,
                                              TFunction<void(UCommonActivatableWidget*)> Callback)
{
	TSoftClassPtr<UCommonActivatableWidget> SoftClassToLoad = nullptr;

	switch (UPlatformStatics::GetPlatform(this))
	{
	case Mobile:
		SoftClassToLoad = MobileWidgetClass;
		break;
	case Desktop:
		SoftClassToLoad = DesktopWidgetClass;
		break;
	case Console:
		SoftClassToLoad = DesktopWidgetClass;
		break;
	default:
		break;
	}

	if(!SoftClassToLoad.IsValid())
		Callback(PushWidgetToLayer(LayerTag, nullptr, nullptr));

	// Подгруджаем класс
	FStreamableManager& StreamableManager = UAssetManager::Get().GetStreamableManager();
	TSharedPtr<FStreamableHandle> StreamingHandle = StreamableManager.RequestAsyncLoad(
		SoftClassToLoad.ToSoftObjectPath(), FStreamableDelegate::CreateWeakLambda(
			this, [this, LayerTag, MobileWidgetClass, DesktopWidgetClass, Callback]()
			{
				Callback(PushWidgetToLayer(LayerTag, MobileWidgetClass.Get(), DesktopWidgetClass.Get()));
			})
	);
	//
	// return StreamingHandle;
}

void UInterfaceLayout::RegisterLayer(const FGameplayTag LayerTag, UCommonActivatableWidgetContainerBase* LayerWidget)
{
	if (!IsDesignTime())
	{
		Layers.Add(LayerTag, LayerWidget);
	}
}

void UAsyncAction_PushWidgetToLayer::Activate()
{
	UInterfaceLayout* GameLayout = UInterfaceLayout::GetInterfaceLayout(WorldContextObject);
	if (!GameLayout)
	{
		SetReadyToDestroy();
		return;
	}

	TWeakObjectPtr<UAsyncAction_PushWidgetToLayer> WeakThis = this;

	GameLayout->PushWidgetToLayerAsync(
		LayerTag, MobileWidgetClass, DesktopWidgetClass, [this, WeakThis](UCommonActivatableWidget* Widget)
		{
			if (WeakThis.IsValid())
				Completed.Broadcast(Widget);
			SetReadyToDestroy();
		}
	);
}

UAsyncAction_PushWidgetToLayer* UAsyncAction_PushWidgetToLayer::PushWidgetToLayerAsync(FGameplayTag LayerTag,
																					   TSoftClassPtr<UCommonActivatableWidget> MobileWidgetClass,
																					   TSoftClassPtr<UCommonActivatableWidget> DesktopWidgetClass,
																					   UObject* WorldContextObject)
{
	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(World))
		return nullptr;

	UAsyncAction_PushWidgetToLayer* Action = NewObject<UAsyncAction_PushWidgetToLayer>();
	Action->LayerTag = LayerTag;
	Action->MobileWidgetClass = MobileWidgetClass;
	Action->DesktopWidgetClass = DesktopWidgetClass;
	Action->WorldContextObject = WorldContextObject;
	Action->RegisterWithGameInstance(World);

	return Action;
}
