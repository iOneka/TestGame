// 


#include "SyncLoadingScreenSubsystem.h"

#include "LoadingScreenWidget.h"
#include "Blueprint/UserWidget.h"
#include "MoviePlayer.h"
#include "STransitionTexture.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Slate/WidgetRenderer.h"

void USyncLoadingScreenSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	// Subscribe for map change event
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &USyncLoadingScreenSubsystem::OnPostLoadMapWithWorld);
}

void USyncLoadingScreenSubsystem::CreateLoadingScreenWidget(TSubclassOf<ULoadingScreenWidget> LoadingScreenClass, const FName LevelName)
{
	checkf(LoadingScreenClass, TEXT("Loading Screen Widget is not valid"))
	
	// Create loading widget
	LoadingWidget = CreateWidget<ULoadingScreenWidget>(GetWorld()->GetFirstPlayerController(), LoadingScreenClass);
	LoadingWidget->AddToViewport(999);

	LoadingWidget->LevelName = LevelName;
	LoadingWidget->OnPreTransition();

	// Save widget class to create widget again after map change
	LoadingWidgetClass = LoadingScreenClass;
	bIsLoading = true;
}

void USyncLoadingScreenSubsystem::PrepareForTransition(bool CustomBool, int CustomInt, const FString& CustomString)
{
	checkf(LoadingWidget, TEXT("LoadingWidget is not valid"))
	checkf(GetMoviePlayer(), TEXT("MoviePlayer is not valid"))
	checkf(FSlateApplication::IsInitialized(), TEXT("SlateApplication is not initialized"))

	// Get screen size based on widget geometry
	const FVector2D CachedSize = LoadingWidget->GetCachedGeometry().GetLocalSize();
	const FVector2D ScreenSize(FMath::FloorToInt(CachedSize.X), FMath::FloorToInt(CachedSize.Y));
	
	// Create WidgetRenderer 
	const TSharedPtr<SWidget> SlateWidget(LoadingWidget->TakeWidget());
	FWidgetRenderer* WidgetRenderer = new FWidgetRenderer();
	UTextureRenderTarget2D* WidgetRT = WidgetRenderer->DrawWidget(SlateWidget.ToSharedRef(), ScreenSize);
	checkf(WidgetRT, TEXT("TextureRenderTarget is not valid"))

	// Get Texture from RT
	TransitionTexture = CreateTextureFromRenderTarget(WidgetRT);
	checkf(TransitionTexture, TEXT("Couldn't get Texture2D from UTextureRenderTarget2D"))
	
	// Настройки для MoviePlayer
	FLoadingScreenAttributes LoadingScreenAttributes;
	LoadingScreenAttributes.bMoviesAreSkippable = false;
	LoadingScreenAttributes.WidgetLoadingScreen = SNew(STransitionTexture, TransitionTexture);
	
	UE_LOG(LogTemp, Warning, TEXT("SetupLoadingScreen"))
	GetMoviePlayer()->SetupLoadingScreen(LoadingScreenAttributes);

	SavedCustomBool = CustomBool;
	SavedCustomInt = CustomInt;
	SavedCustomString = CustomString;
}

void USyncLoadingScreenSubsystem::OnPostLoadMapWithWorld(UWorld* World)
{
	if(!bIsLoading) return;
	
	checkf(LoadingWidgetClass, TEXT("Could not recreate loading screen. Ignore this message if game just opened"));
	
	// Создаём виджет
	ULoadingScreenWidget* LoadingScreen = CreateWidget<ULoadingScreenWidget>(GetWorld()->GetFirstPlayerController(), LoadingWidgetClass);
	LoadingScreen->AddToViewport(999);

	LoadingScreen->OnPostTransition(SavedCustomBool, SavedCustomInt, SavedCustomString);

	// Сбрасываем класс
	LoadingWidgetClass = nullptr;
	bIsLoading = false;
}

UTexture2D* USyncLoadingScreenSubsystem::CreateTextureFromRenderTarget(UTextureRenderTarget2D* RenderTarget)
{
	if(!RenderTarget)
	{
		return nullptr;
	}

	// Создаём текстуру
	UTexture2D* NewTexture = UTexture2D::CreateTransient(RenderTarget->SizeX, RenderTarget->SizeY, PF_B8G8R8A8);

	TArray<FColor> OutBMP;
	RenderTarget->GameThread_GetRenderTargetResource()->ReadPixels(OutBMP);
	
	// Lock the texture so we can read it
	uint8* MipData = static_cast<uint8*>(NewTexture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE));
	FMemory::Memcpy(MipData, OutBMP.GetData(), RenderTarget->SizeX * RenderTarget->SizeY * 4);
	NewTexture->GetPlatformData()->Mips[0].BulkData.Unlock();

	// Update the rendering resource from data.
	NewTexture->UpdateResource();

	return NewTexture;
}
