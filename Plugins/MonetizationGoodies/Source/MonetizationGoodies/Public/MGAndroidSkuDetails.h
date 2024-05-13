// Copyright (c) 2021 Nineva Studios

#pragma once

#include "MGAndroidWrapperObject.h"
#include "MGAndroidSkuDetails.generated.h"

/*
* Represents an in-app product's or subscription's listing details.
*/
UCLASS(BlueprintType)
class MONETIZATIONGOODIES_API UMGAndroidSkuDetails : public UMGAndroidWrapperObject
{
	GENERATED_BODY()

public:

	/*
	* Returns the description of the product.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|Android")
	FString GetDescription();

	/*
	* Trial period configured in Google Play Console, specified in ISO 8601 format.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|Android")
	FString GetFreeTrialPeriod();

	/*
	* Returns the icon of the product if present.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|Android")
	FString GetIconUrl();

	/*
	* Formatted introductory price of a subscription, including its currency sign, such as ˆ3.99.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|Android")
	FString GetIntroductoryPrice();

	/*
	* Introductory price in micro-units.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|Android")
	int64 GetIntroductoryPriceAmountMicros();

	/*
	* The number of subscription billing periods for which the user will be given the introductory price, such as 3.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|Android")
	int GetIntroductoryPriceCycles();

	/*
	* The billing period of the introductory price, specified in ISO 8601 format.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|Android")
	FString GetIntroductoryPricePeriod();

	/*
	* Returns a String in JSON format that contains SKU details.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|Android")
	FString GetOriginalJson();

	/*
	* Returns formatted original price of the item, including its currency sign.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|Android")
	FString GetOriginalPrice();

	/*
	* Returns the original price in micro-units, where 1,000,000 micro-units equal one unit of the currency.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|Android")
	int64 GetOriginalPriceAmountMicros();

	/*
	* Returns formatted price of the item, including its currency sign.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|Android")
	FString GetPrice();

	/*
	* Returns price in micro-units, where 1,000,000 micro-units equal one unit of the currency.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|Android")
	int64 GetPriceAmountMicros();

	/*
	* Returns ISO 4217 currency code for price and original price.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|Android")
	FString GetPriceCurrencyCode();

	/*
	* Returns the product Id.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|Android")
	FString GetSku();

	/*
	* Subscription period, specified in ISO 8601 format.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|Android")
	FString GetSubscriptionPeriod();

	/*
	* Returns the title of the product being sold.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|Android")
	FString GetTitle();

	/*
	* Returns the type of SKU, either MGSkuTypeSubscription or MGSkuTypeInAppPurchase, defined in the MGAndroidBillingLibrary.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|Android")
	FString GetType();
};
