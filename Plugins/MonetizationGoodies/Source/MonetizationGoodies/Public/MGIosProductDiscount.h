// Copyright (c) 2021 Nineva Studios

#pragma once

#include "MGIosWrapperObject.h"

#include "MGIosProduct.h"

#include "MGIosProductDiscount.generated.h"

/*
* Values representing the types of discount offers an app can present.
*/
UENUM(BlueprintType)
enum class EMGProductDiscountType : uint8
{
	/*
	* A constant indicating the discount type is an introductory offer.
	*/
	Introductory = 0,
	/*
	* A constant indicating the discount type is a promotional offer.
	*/
	Subscription = 1
};

/*
* Values representing the payment modes for a product discount.
*/
UENUM(BlueprintType)
enum class EMGProductPaymentType : uint8
{
	/*
	* A constant indicating that the payment mode of a product discount is billed over a single or multiple billing periods.
	*/
	PayAsYouGo = 0,
	/*
	* A constant indicating that the payment mode of a product discount is paid up front.
	*/
	PayUpfront = 1,
	/*
	* A constant that indicates that the payment mode is a free trial.
	*/
	FreeTrial = 2
};

/*
* The details of an introductory offer or a promotional offer for an auto-renewable subscription.
*/
UCLASS(BlueprintType)
class MONETIZATIONGOODIES_API UMGIosProductDiscount : public UMGIosWrapperObject
{
	GENERATED_BODY()

public:
	
	/*
	* A string used to uniquely identify a discount offer for a product.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|IOS")
	FString GetIdentifier();
	
	/*
	* The type of discount offer.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|IOS")
	EMGProductDiscountType GetType();
	
	/*
	* The discount price of the product in the local currency.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|IOS")
	FString GetFormattedPrice();
	
	/*
	* The payment mode for this product discount.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|IOS")
	EMGProductPaymentType GetPaymentType();
	
	/*
	* An object that defines the period for the product discount.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|IOS")
	FMGIosSubscriptionPeriod GetSubscriptionPeriod();
	
	/*
	* An integer that indicates the number of periods the product discount is available.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|IOS")
	int GetNumberOfPeriods();
};
