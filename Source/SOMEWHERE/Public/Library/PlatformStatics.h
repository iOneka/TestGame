#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PlatformStatics.generated.h"


UENUM(BlueprintType)
enum EPlatform
{
	Mobile	UMETA(DisplayName = "Mobile"),
	Desktop UMETA(DisplayName = "Desktop"),
	Console UMETA(DisplayName = "Console")
};

UENUM(BlueprintType)
enum EOperatingSystem
{
	Window	UMETA(DisplayName = "Windows"),
	Mac		UMETA(DisplayName = "Mac"),
	IOS		UMETA(DisplayName = "IOS"),
	Android UMETA(DisplayName = "Android"),
	Linux	UMETA(DisplayName = "Linux"),
	Unknown UMETA(DisplayName = "Unknown"),
};

/**
 * 
 */
UCLASS(Abstract)
class SOMEWHERE_API UPlatformStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	
	/**
	 * 
	 */
	UFUNCTION(BlueprintPure, Category="Platform Statics", meta=(WorldContext="WorldContextObject"))
	static EOperatingSystem GetOperatingSystem();
	
	/**
	 * 
	 */
	UFUNCTION(BlueprintPure, Category="Platform Statics", meta=(WorldContext="WorldContextObject"))
	static EPlatform GetPlatform(const UObject* WorldContextObject);
	
	/**
	 * 
	 */
	UFUNCTION(BlueprintPure, Category="Platform Statics", meta=(WorldContext="WorldContextObject"))
	static bool IsMobilePlatform(const UObject* WorldContextObject);
	
	/**
	 * 
	 */
	UFUNCTION(BlueprintPure, Category="Platform Statics", meta=(WorldContext="WorldContextObject"))
	static bool IsDesktopPlatform(const UObject* WorldContextObject);
	
	/**
	 * 
	 */
	UFUNCTION(BlueprintPure, Category="Platform Statics", meta=(WorldContext="WorldContextObject"))
	static bool IsEditor();

	/**
	 * 
	 */
	UFUNCTION(BlueprintPure, Category="Platform Statics", meta=(WorldContext="WorldContextObject"))
	static FString GetDefaultDeviceProfileName();

	
	UFUNCTION(BlueprintPure, Category="Platform Statics")
	static bool EnvironmentVariableExists(const FString& Name);

	UFUNCTION(BlueprintPure, Category="Platform Statics")
	static FString GetEnvironmentVariable(const FString& Name);
	
	UFUNCTION(BlueprintPure, Category="Platform Statics")
	static FString GetEnvironmentVariableDefault(const FString& Name, const FString& Default);
	

	UFUNCTION(BlueprintPure, Category="Platform Statics")
	static FName GetDeveloperName();
};
