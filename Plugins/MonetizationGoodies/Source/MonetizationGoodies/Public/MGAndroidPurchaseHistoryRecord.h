// Copyright (c) 2021 Nineva Studios

#pragma once

#include "MGAndroidWrapperObject.h"
#include "MGAndroidPurchaseHistoryRecord.generated.h"

/*
* Represents an in-app billing purchase history record. This class includes a subset of fields in Purchase.
*/
UCLASS(BlueprintType)
class MONETIZATIONGOODIES_API UMGAndroidPurchaseHistoryRecord : public UMGAndroidWrapperObject
{
	GENERATED_BODY()

public:

	/*
	* Returns the payload specified when the purchase was acknowledged or consumed.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|Android")
	FString GetDeveloperPayload();

	/*
	* Returns a String in JSON format that contains details about the purchase order.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|Android")
	FString GetOriginalJson();

	/*
	* Returns the time the product was purchased (UTC).
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|Android")
	FDateTime GetPurchaseTime();

	/*
	* Returns a token that uniquely identifies a purchase for a given item and user pair.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|Android")
	FString GetPurchaseToken();

	/*
	* Returns the quantity of the purchased product.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|Android")
	int GetQuantity();

	/*
	* Returns String containing the signature of the purchase data that was signed with the private key of the developer.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|Android")
	FString GetSignature();

	/*
	* Returns the product Ids.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|Android")
	TArray<FString> GetSkus();
};