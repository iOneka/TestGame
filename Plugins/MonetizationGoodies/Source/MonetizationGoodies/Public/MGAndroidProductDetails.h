#pragma once

#include "MGAndroidWrapperObject.h"
#include "MGAndroidProductDetails.generated.h"

USTRUCT(BlueprintType)
struct MONETIZATIONGOODIES_API FOneTimePurchaseOfferDetails
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Monetization Goodies|Android")
	bool IsValid = false;

	UPROPERTY(BlueprintReadWrite, Category = "Monetization Goodies|Android")
	int64 PriceAmountMicros = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Monetization Goodies|Android")
	FString FormattedPrice = "";

	UPROPERTY(BlueprintReadWrite, Category = "Monetization Goodies|Android")
	FString PriceCurrencyCode = "";
};

USTRUCT(BlueprintType)
struct MONETIZATIONGOODIES_API FSubscriptionPricingPhase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Monetization Goodies|Android")
	int32 BillingCycleCount = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Monetization Goodies|Android")
	int32 RecurrenceMode = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Monetization Goodies|Android")
	int64 PriceAmountMicros = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Monetization Goodies|Android")
	FString BillingPeriod = "";

	UPROPERTY(BlueprintReadWrite, Category = "Monetization Goodies|Android")
	FString FormattedPrice = "";

	UPROPERTY(BlueprintReadWrite, Category = "Monetization Goodies|Android")
	FString PriceCurrencyCode = "";
};

USTRUCT(BlueprintType)
struct MONETIZATIONGOODIES_API FSubscriptionOfferDetails
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Monetization Goodies|Android")
	TArray<FSubscriptionPricingPhase> PricingPhases;

	UPROPERTY(BlueprintReadWrite, Category = "Monetization Goodies|Android")
	TArray<FString> OfferTags;

	UPROPERTY(BlueprintReadWrite, Category = "Monetization Goodies|Android")
	FString OfferToken = "";
};

UCLASS(BlueprintType)
class MONETIZATIONGOODIES_API UMGAndroidProductDetails : public UMGAndroidWrapperObject
{
	GENERATED_BODY()

public:
	/*
	* Returns the description of the product.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|Android")
	FString GetDescription();

	/*
	* Returns the name of the product being sold.
	* This method is similar to GetTitle() but does not include the name of the app which owns the product. Example: 100 Gold Coins
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|Android")
	FString GetName();

	/*
	* Returns the product's Id.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|Android")
	FString GetProductId();

	/*
	* Returns the type of the product. ("inapp" or "subs")
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|Android")
	FString GetProductType();

	/*
	* Returns the product title.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|Android")
	FString GetTitle();

	/*
	* Returns optional one time purchase offer details. Check IsValid on the returned struct to see if you can use the data.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|Android")
	FOneTimePurchaseOfferDetails GetOneTimePurchaseOfferDetails();

	/*
	* Returns optional subscription offer details. Check IsValid on the returned struct to see if you can use the data.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|Android")
	TArray<FSubscriptionOfferDetails> GetSubscriptionOfferDetails();
};
