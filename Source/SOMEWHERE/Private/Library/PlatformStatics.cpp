#include "Library/PlatformStatics.h"

#include "Kismet/GameplayStatics.h"

EOperatingSystem UPlatformStatics::GetOperatingSystem()
{
	if (UGameplayStatics::GetPlatformName() == "Windows") return Window;
	if (UGameplayStatics::GetPlatformName() == "Mac") return Mac;
	if (UGameplayStatics::GetPlatformName() == "IOS") return IOS;
	if (UGameplayStatics::GetPlatformName() == "Android") return Android;
	if (UGameplayStatics::GetPlatformName() == "Linux") return Linux;
	return Unknown;
}

EPlatform UPlatformStatics::GetPlatform(const UObject* WorldContextObject)
{
	if (IsMobilePlatform(WorldContextObject)) return Mobile;
	if (IsDesktopPlatform(WorldContextObject)) return Desktop;
	return Console;
}

bool UPlatformStatics::IsMobilePlatform(const UObject* WorldContextObject)
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

bool UPlatformStatics::IsDesktopPlatform(const UObject* WorldContextObject)
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

bool UPlatformStatics::IsEditor()
{
#if WITH_EDITOR
	return true;
#else
	return false;
#endif
}

FString UPlatformStatics::GetDefaultDeviceProfileName()
{
	return FGenericPlatformMisc::GetDefaultDeviceProfileName();
}

bool UPlatformStatics::EnvironmentVariableExists(const FString& Name)
{
	return !FPlatformMisc::GetEnvironmentVariable(*Name).IsEmpty();
}

FString UPlatformStatics::GetEnvironmentVariable(const FString& Name)
{
	return FPlatformMisc::GetEnvironmentVariable(*Name);
}

FString UPlatformStatics::GetEnvironmentVariableDefault(const FString& Name, const FString& Default)
{
	if(EnvironmentVariableExists(Name))
		return FPlatformMisc::GetEnvironmentVariable(*Name);
	return Default;
}

FName UPlatformStatics::GetDeveloperName()
{
	return FName(*FPaths::GameUserDeveloperFolderName());
}
