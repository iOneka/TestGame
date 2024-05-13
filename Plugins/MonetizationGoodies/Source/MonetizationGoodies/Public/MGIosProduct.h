// Copyright (c) 2021 Nineva Studios

#pragma once

#include "MGIosWrapperObject.h"

#include "MGIosProduct.generated.h"

class UMGIosProductDiscount;

/*
* Values representing the duration of an interval, from a day up to a year.
*/
UENUM(BlueprintType)
enum class EMGProductPeriodUnit : uint8
{
	/*
	* An interval lasting one day.
	*/
	Day = 0,
	/*
	* An interval lasting one month.
	*/
	Month = 1,
	/*
	* An interval lasting one week.
	*/
	Week = 2,
	/*
	* An interval lasting one year.
	*/
	Year = 3
};

/*
* An object containing the subscription period duration information.
*/
USTRUCT(BlueprintType)
struct MONETIZATIONGOODIES_API FMGIosSubscriptionPeriod
{
	GENERATED_BODY()
	
	/*
	* The number of units per subscription period.
	*/
	UPROPERTY(BlueprintReadWrite, Category = "Monetization Goodies|IOS")
	int NumberOfUnits = 0;

	/*
	* The increment of time that a subscription period is specified in.
	*/
	UPROPERTY(BlueprintReadWrite, Category = "Monetization Goodies|IOS")
	EMGProductPeriodUnit Unit = EMGProductPeriodUnit::Day;
};

/*
* Values representing the state of a transaction.
*/
UCLASS(BlueprintType)
class MONETIZATIONGOODIES_API UMGIosProduct : public UMGIosWrapperObject
{
	GENERATED_BODY()

public:
	
	/*
	* The string that identifies the product to the Apple App Store.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|IOS")
	FString GetProductIdentifier();
	
	/*
	* A description of the product.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|IOS")
	FString GetLocalizedDescription();
	
	/*
	* The name of the product.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|IOS")
	FString GetLocalizedTitle();
	
	/*
	* A string that identifies the version of the content.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|IOS")
	FString GetContentVersion();
	
	/*
	* A Boolean value that indicates whether the product is available for family sharing in App Store Connect.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|IOS")
	bool IsFamilyShareable();

	/*
	* The cost of the product in the local currency.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|IOS")
	FString GetFormattedPrice();
	
	/*
	* The object containing introductory price information for the product.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|IOS")
	UMGIosProductDiscount* GetIntroductoryPrice();
	
	/*
	* An array of subscription offers available for the auto-renewable subscription.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|IOS")
	TArray<UMGIosProductDiscount*> GetDiscounts();

	/*
	* The identifier of the subscription group to which the subscription belongs.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|IOS")
	FString GetSubscriptionGroupIdentifier();
	
	/*
	* The period details for products that are subscriptions.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|IOS")
	FMGIosSubscriptionPeriod GetSubscriptionPeriod();

	/*
	* A Boolean value that indicates whether the App Store has downloadable content for this product.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|IOS")
	bool IsDownloadable();
	
	/*
	* The lengths of the downloadable files available for this product.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|IOS")
	TArray<int64> GetDownloadContentLengths();
	
	/*
	* A string that identifies which version of the content is available for download.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|IOS")
	FString GetDownloadContentVersion();
};
