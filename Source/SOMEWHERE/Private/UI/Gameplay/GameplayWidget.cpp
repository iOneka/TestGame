#include "UI/Gameplay/GameplayWidget.h"

#include "GameplayTagContainer.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Library/SomewhereStatics.h"
#include "Animation/UMGSequencePlayer.h"
#include "Animation/WidgetAnimation.h"
#include "UI/Gameplay/Views/FTagToView.h"

void UGameplayWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UGameplayWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UGameplayWidget::ShowHUD() const
{
	HUD->PlayAnimation(HUD->ShowAnimation);
	HUD->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UGameplayWidget::HideHUD() const
{
	HUD->PlayAnimation(HUD->HideAnimation);
	HUD->SetVisibility(ESlateVisibility::HitTestInvisible);
}

UUserWidget* UGameplayWidget::OpenView(FGameplayTag ViewTag)
{
	if (!ViewsTable)
		return nullptr;

	// Ищем вьюху в табличке
	const FTagToView* TagToView = ViewsTable->FindRow<FTagToView>(ViewTag.GetTagName(), "");

	// Подгружаем класс вьюхи
	TSoftClassPtr<UView> ViewSoftClassRef = TagToView->View;
	const TSubclassOf<UView> ViewClassRef = TagToView->View.LoadSynchronous();

	// Красиво прячем худ
	HideHUD();

	// TODO: https://forums.unrealengine.com/t/c-how-to-get-uclass-from-tsubclassof/379067
	// Создаём вьюху
	UView* CreatedView = CreateWidget<UView>(this, ViewClassRef);
	CreatedView->AddToViewport();
	CurrentView = CreatedView;

	// Настраиваем инпут для разных платформ
	if (USomewhereStatics::IsDesktopPlatform(this))
	{
		UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(GetOwningPlayer(), CreatedView, EMouseLockMode::DoNotLock, true);
		GetOwningPlayer()->SetShowMouseCursor(true);
		// CreatedView->bIsFocusable = true;
		CreatedView->SetKeyboardFocus();
	}
	else if (USomewhereStatics::IsMobilePlatform(this))
	{
		GetOwningPlayer()->SetShowMouseCursor(true);
	}

	// Запускаем анимацию открытия вьюхи
	const UUMGSequencePlayer* ViewOpenAnimation = CreatedView->PlayAnimation(CreatedView->ViewOpenAnimation);
	const float AnimationDuration = ViewOpenAnimation->GetAnimation()->GetEndTime();

	// Ждём завершения анимации и раним ивент OnViewOpened
	FTimerHandle TimerHandle;
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindLambda([&, CreatedView]
	{
		CreatedView->OnViewOpened();
	});
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, AnimationDuration, false);

	return CreatedView;
}

void UGameplayWidget::CloseView(UView* View)
{
	// Раним ивент
	View->OnViewClosed();

	// Настраиваем инпут для разных платформ
	if (USomewhereStatics::IsDesktopPlatform(this))
	{
		UWidgetBlueprintLibrary::SetInputMode_GameOnly(GetOwningPlayer(), true);
		GetOwningPlayer()->SetShowMouseCursor(false);
		// View->bIsFocusable = false;
	}
	else if (USomewhereStatics::IsMobilePlatform(this))
	{
		GetOwningPlayer()->SetShowMouseCursor(true);
	}

	// Играем анимацию закрытия
	const UUMGSequencePlayer* ViewCloseAnimation = View->PlayAnimation(View->ViewCloseAnimation);
	const float AnimationDuration = ViewCloseAnimation->GetAnimation()->GetEndTime();

	// Ждём завершения анимации
	FTimerHandle TimerHandle;
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindLambda([&, View]
	{
		// Покзываем худ
		USomewhereStatics::GetGameplayWidget(View)->ShowHUD();

		// Удаляем вьюху
		View->RemoveFromParent();
	});
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, AnimationDuration, false);
}
