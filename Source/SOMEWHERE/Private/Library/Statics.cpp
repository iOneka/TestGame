#include "Library/SomewhereStatics.h"

#include "Landscape.h"
#include "Game/SomewhereGameInstance.h"
#include "Game/SomewhereGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Player/SomewhereCharacter.h"
#include "Player/SomewhereHUD.h"
#include "Player/SomewherePlayerController.h"

USomewhereGameInstance* USomewhereStatics::GetSomewhereGameInstance(const UObject* WorldContextObject)
{
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject);
	return GameInstance ? Cast<USomewhereGameInstance>(GameInstance) : nullptr;
}

ASomewhereGameMode* USomewhereStatics::GetSomewhereGameMode(const UObject* WorldContextObject)
{
	AGameModeBase* GameModeBase = UGameplayStatics::GetGameMode(WorldContextObject);
	return GameModeBase ? Cast<ASomewhereGameMode>(GameModeBase) : nullptr;
}

ASomewherePlayerController* USomewhereStatics::GetSomewherePlayerController(const UObject* WorldContextObject, int32 PlayerIndex)
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, PlayerIndex);
	return PC ? Cast<ASomewherePlayerController>(PC) : nullptr;
}

ASomewhereCharacter* USomewhereStatics::GetSomewherePlayerCharacter(const UObject* WorldContextObject, int32 PlayerIndex)
{
	ACharacter* Character = UGameplayStatics::GetPlayerCharacter(WorldContextObject, PlayerIndex);
	return Character ? Cast<ASomewhereCharacter>(Character) : nullptr;
}

ASomewhereHUD* USomewhereStatics::GetSomewhereHUD(const UObject* WorldContextObject)
{
	AHUD* HUD = UGameplayStatics::GetPlayerController(WorldContextObject, 0)->GetHUD();
	return HUD ? Cast<ASomewhereHUD>(HUD) : nullptr;
}

UGameplayWidget* USomewhereStatics::GetGameplayWidget(const UObject* WorldContextObject)
{
	const ASomewherePlayerController* SPC = GetSomewherePlayerController(WorldContextObject, 0);
	return SPC ? SPC->GameplayWidget : nullptr;
}

UHUDWidget* USomewhereStatics::GetHUDWidget(const UObject* WorldContextObject)
{
	const ASomewherePlayerController* SPC = GetSomewherePlayerController(WorldContextObject, 0);
	return SPC ? SPC->HUDWidget : nullptr;
}

EOperatingSystem USomewhereStatics::GetOperatingSystem()
{
	if (UGameplayStatics::GetPlatformName() == "Windows") return Window;
	if (UGameplayStatics::GetPlatformName() == "Mac") return Mac;
	if (UGameplayStatics::GetPlatformName() == "IOS") return IOS;
	if (UGameplayStatics::GetPlatformName() == "Android") return Android;
	if (UGameplayStatics::GetPlatformName() == "Linux") return Linux;
	return Unknown;
}

EPlatform USomewhereStatics::GetPlatform(const UObject* WorldContextObject)
{
	if (IsMobilePlatform(WorldContextObject)) return Mobile;
	if (IsDesktopPlatform(WorldContextObject)) return Desktop;
	return Console;
}

bool USomewhereStatics::IsMobilePlatform(const UObject* WorldContextObject)
{
#if WITH_EDITOR
	if (WorldContextObject->GetWorld()->GetFeatureLevel() == ERHIFeatureLevel::ES3_1)
	{
		return true;
	}
#endif

#if PLATFORM_IOS || PLATFORM_ANDROID
	return true;
#endif
	
	return false;
}

bool USomewhereStatics::IsDesktopPlatform(const UObject* WorldContextObject)
{
#if WITH_EDITOR
	if (WorldContextObject->GetWorld()->GetFeatureLevel() == ERHIFeatureLevel::ES3_1)
	{
		return false;
	}
#endif

#if PLATFORM_WINDOWS
	return true;
#endif
	
	return false;
}

bool USomewhereStatics::IsEditor()
{
#if WITH_EDITOR
	return true;
#else
	return false;
#endif
}

FString USomewhereStatics::GetDefaultDeviceProfileName()
{
	return FGenericPlatformMisc::GetDefaultDeviceProfileName();
}

FString USomewhereStatics::GetEnvironmentVariable(const FString& VariableName)
{
	return FPlatformMisc::GetEnvironmentVariable(*VariableName);
}

FName USomewhereStatics::GetDeveloperName()
{
	return FName(*FPaths::GameUserDeveloperFolderName());
}

bool USomewhereStatics::IsLandscape(const AActor* Actor)
{
	if(Actor->IsA(ALandscape::StaticClass()) || Actor->ActorHasTag(FName("IsLandscape")))
		return true;
	return false;
}
