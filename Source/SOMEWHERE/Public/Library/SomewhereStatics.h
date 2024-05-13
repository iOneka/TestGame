#pragma once

#include "CoreMinimal.h"
#include "PlatformStatics.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SomewhereStatics.generated.h"


/**
 * Типо override GameplayStatics, но некоторые функции сразу кастят в Somewhere класс
 */
UCLASS(Abstract)
class SOMEWHERE_API USomewhereStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	
	// --- Player functions ------------------------------
	
	/**
	 * Returns the somewhere game instance object
	 */
	UFUNCTION(BlueprintPure, Category="Somewhere Statics", meta=(WorldContext="WorldContextObject"))
	static class USomewhereGameInstance* GetSomewhereGameInstance(const UObject* WorldContextObject);
	
	/** Returns the current SomewhereGameMode or Null if it can't be retrieved, such as on the client */
	UFUNCTION(BlueprintPure, Category="Somewhere Statics", meta=(WorldContext="WorldContextObject"))
	static class ASomewhereGameMode* GetSomewhereGameMode(const UObject* WorldContextObject);
	
	/** 
	 * Returns the somewhere player controller found while iterating through the local and available remote player controllers.
	 * On a network client, this will only include local players as remote player controllers are not available.
	 * The index will be consistent as long as no new players join or leave, but it will not be the same across different clients and servers.
	 *
	 * @param PlayerIndex	Index in the player controller list, starting first with local players and then available remote ones
	 */
	UFUNCTION(BlueprintPure, Category="Somewhere Statics", meta=(WorldContext="WorldContextObject", UnsafeDuringActorConstruction="true"))
	static class ASomewherePlayerController* GetSomewherePlayerController(const UObject* WorldContextObject, int32 PlayerIndex);

	/**
	 * Returns the pawn for the player controller at the specified player index, will return null if the pawn is not a character.
	 * This will not include characters of remote clients with no available player controller, you can iterate the PlayerStates list for that.
	 *
	 * @param PlayerIndex	Index in the player controller list, starting first with local players and then available remote ones
	*/
	UFUNCTION(BlueprintPure, Category="Somewhere Statics", meta=(WorldContext="WorldContextObject", UnsafeDuringActorConstruction="true"))
	static class ASomewhereCharacter* GetSomewherePlayerCharacter(const UObject* WorldContextObject, int32 PlayerIndex);

	
	UFUNCTION(BlueprintPure, Category="Somewhere Statics", meta=(WorldContext="WorldContextObject", UnsafeDuringActorConstruction="true"))
	static class ASomewhereHUD* GetSomewhereHUD(const UObject* WorldContextObject);

	/**
	 * Returns GameplayWidget
	 */
	UFUNCTION(BlueprintPure, Category="Somewhere Statics", meta=(WorldContext="WorldContextObject", UnsafeDuringActorConstruction="true"))
	static class UGameplayWidget* GetGameplayWidget(const UObject* WorldContextObject);
	
	/**
	 * Returns GameplayHUD
	 */
	UFUNCTION(BlueprintPure, DisplayName="Get HUD Widget", Category="Somewhere Statics", meta=(WorldContext="WorldContextObject", UnsafeDuringActorConstruction="true"))
	static class UHUDWidget* GetHUDWidget(const UObject* WorldContextObject);
	
	// --- Platform functions ------------------------------

	/**
	 * 
	 */
	UFUNCTION(BlueprintPure, Category="Somewhere Statics", meta=(WorldContext="WorldContextObject"), meta=(DeprecatedFunction, DeprecationMessage="Use PlatformStatics instead"))
	static EOperatingSystem GetOperatingSystem();
	
	/**
	 * 
	 */
	UFUNCTION(BlueprintPure, Category="Somewhere Statics", meta=(WorldContext="WorldContextObject"), meta=(DeprecatedFunction, DeprecationMessage="Use PlatformStatics instead"))
	static EPlatform GetPlatform(const UObject* WorldContextObject);
	
	/**
	 * 
	 */
	UFUNCTION(BlueprintPure, Category="Somewhere Statics", meta=(WorldContext="WorldContextObject"), meta=(DeprecatedFunction, DeprecationMessage="Use PlatformStatics instead"))
	static bool IsMobilePlatform(const UObject* WorldContextObject);
	
	/**
	 * 
	 */
	UFUNCTION(BlueprintPure, Category="Somewhere Statics", meta=(WorldContext="WorldContextObject"), meta=(DeprecatedFunction, DeprecationMessage="Use PlatformStatics instead"))
	static bool IsDesktopPlatform(const UObject* WorldContextObject);
	
	/**
	 * 
	 */
	UFUNCTION(BlueprintPure, Category="Somewhere Statics", meta=(WorldContext="WorldContextObject"), meta=(DeprecatedFunction, DeprecationMessage="Use PlatformStatics instead"))
	static bool IsEditor();

	/**
	 * 
	 */
	UFUNCTION(BlueprintPure, Category="Somewhere Statics", meta=(WorldContext="WorldContextObject"), meta=(DeprecatedFunction, DeprecationMessage="Use PlatformStatics instead"))
	static FString GetDefaultDeviceProfileName();
	
	
	/**
	 * Retrieve a environment variable from the system
	 *
	 * @param VariableName The name of the variable (ie "Path")
	 * @return Value of the variable, or an empty string if not set.
	 */
	UFUNCTION(BlueprintPure, Category="Somewhere Statics", meta=(DeprecatedFunction, DeprecationMessage="Use PlatformStatics instead"))
	static FString GetEnvironmentVariable(const FString& VariableName);

	UFUNCTION(BlueprintPure, Category="Somewhere Statics", meta=(DeprecatedFunction, DeprecationMessage="Use PlatformStatics instead"))
	static FName GetDeveloperName();
	
	UFUNCTION(BlueprintPure, Category="Somewhere Statics", meta=(WorldContext="WorldContextObject"))
	static bool IsLandscape(const AActor* Actor);
};
