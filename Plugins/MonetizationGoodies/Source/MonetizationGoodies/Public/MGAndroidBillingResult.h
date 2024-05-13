// Copyright (c) 2021 Nineva Studios

#pragma once

#include "MGAndroidWrapperObject.h"
#include "MGAndroidBillingResult.generated.h"

/*
* Params containing the response code and the debug message from In-app Billing API response.
*/
UENUM(BlueprintType)
enum class EMGAndroidBillingResponseCode : uint8
{
	/*
	* Success.
	*/
	Ok = 0,
	/*
	* User pressed back or canceled a dialog.
	*/
	UserCancelled = 1,
	/*
	* Network connection is down.
	*/
	ServiceUnavailable = 2,
	/*
	* Billing API version is not supported for the type requested.
	*/
	BillingUnavailable = 3,
	/*
	* Requested product is not available for purchase.
	*/
	ItemUnavailable = 4,
	/*
	* Invalid arguments provided to the API.
	*/
	DeveloperError = 5,
	/*
	* Fatal error during the API action.
	*/
	Error = 6,
	/*
	* Failure to purchase since item is already owned.
	*/
	ItemAlreadyOwned = 7,
	/*
	* Failure to consume since item is not owned.
	*/
	ItemNotOwned = 8,
	/*
	* The request has reached the maximum timeout before Google Play responds.
	*/
	ServiceTimeout = 97,
	/*
	* Requested feature is not supported by Play Store on the current device.
	*/
	FeatureNotSupported = 98,
	/*
	* Play Store service is not connected now - potentially transient state.
	*/
	ServiceDisconnected = 99
};

/*
* Params containing the response code and the debug message from In-app Billing API response.
*/
UCLASS(BlueprintType)
class MONETIZATIONGOODIES_API UMGAndroidBillingResult : public UMGAndroidWrapperObject
{
	GENERATED_BODY()

public:

	/*
	* Debug message returned in In-app Billing API calls.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|Android")
	FString GetDebugMessage();

	/*
	* Response code returned in In-app Billing API calls.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|Android")
	EMGAndroidBillingResponseCode GetResponseCode();
};