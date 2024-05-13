// Copyright (c) 2021 Nineva Studios

#pragma once

#include "MGAndroidWrapperObject.h"

#include "MGAndroidBillingFlowParameters.generated.h"

class UMGAndroidSkuDetails;
class UMGAndroidProductDetails;

/*
* Replace SKU ProrationMode.
*/
UENUM(BlueprintType)
enum class EMGProrationMode : uint8
{
	/*
	* Not for use.
	*/
	UnknownSubscriptionPolicy = 0,
	/*
	* Replacement takes effect immediately, and the remaining time will be prorated and credited to the user. This is the current default behavior.
	*/
	ImmediateWithTimeProration = 1,
	/*
	* Replacement takes effect immediately, and the billing cycle remains the same. The price for the remaining period will be charged. This option is only available for subscription upgrade.
	*/
	ImmediateAndChargeProratedPrice = 2,
	/*
	* Replacement takes effect immediately, and the new price will be charged on next recurrence time. The billing cycle stays the same.
	*/
	ImmediateWithoutProration = 3,
	/*
	* Replacement takes effect when the old plan expires, and the new price will be charged at the same time.
	*/
	Deferred = 4,
	/*
	* Replacement takes effect immediately, and the user is charged full price of new plan and is given a full billing cycle of subscription, plus remaining prorated time from the old plan.
	*/
	ImmediateAndChargeFullPrice = 5
};

/*
* Parameters to initiate a purchase flow.
*/
UCLASS(BlueprintType)
class MONETIZATIONGOODIES_API UMGAndroidBillingFlowParameters : public UMGAndroidWrapperObject
{
	GENERATED_BODY()

public:

	/*
	* Specifies an optional obfuscated string that is uniquely associated with the user's account in your app.
	* 
	* @return the same instance for chaining.
	*/
	UFUNCTION(BlueprintCallable, Category = "Monetization Goodies|Android")
	UMGAndroidBillingFlowParameters* SetObfuscatedAccountId(const FString& ObfuscatedAccountId);

	/*
	* Specifies an optional obfuscated string that is uniquely associated with the user's profile in your app.
	*
	* @return the same instance for chaining.
	*/
	UFUNCTION(BlueprintCallable, Category = "Monetization Goodies|Android")
	UMGAndroidBillingFlowParameters* SetObfuscatedProfileId(const FString& ObfuscatedProfileId);

	/*
	* Specifies the SkuDetails of the item being purchased.
	*
	* @return the same instance for chaining.
	*/
	UFUNCTION(BlueprintCallable, Category = "Monetization Goodies|Android", meta = (DeprecatedFunction, DeprecationMessage = "Use SetProductDetailsParams Instead."))
	UMGAndroidBillingFlowParameters* SetSkuDetails(UMGAndroidSkuDetails* SkuDetails);

	/*
	* Specifies the parameters used to upgrade or downgrade a subscription.
	*
	* @return the same instance for chaining.
	*/
	UFUNCTION(BlueprintCallable, Category = "Monetization Goodies|Android")
	UMGAndroidBillingFlowParameters* SetSubscriptionUpdateParams(const FString& OldSkuPurchaseToken, EMGProrationMode ProrationMode = EMGProrationMode::ImmediateWithTimeProration);

	/*
	* Specifies the parameters used to purchase product. OfferToken is required only for subscriptions.
	*
	* @return the same instance for chaining.
	*/
	UFUNCTION(BlueprintCallable, Category = "Monetization Goodies|Android")
	UMGAndroidBillingFlowParameters* SetProductDetailsParams(UMGAndroidProductDetails* ProductDetails, const FString& OfferToken = "");
};
