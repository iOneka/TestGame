// Copyright (c) 2021 Nineva Studios

#pragma once

#include "MGAndroidWrapperObject.h"
#include "MGAndroidPurchase.generated.h"

/*
* Account identifiers that were specified when the purchase was made.
*/
USTRUCT(BlueprintType)
struct MONETIZATIONGOODIES_API FMGAndroidAccountIdentifiers
{
	GENERATED_BODY()

	/*
	* The obfuscated account id specified.
	*/
	UPROPERTY(BlueprintReadWrite, Category = "Monetization Goodies|Android")
	FString ObfuscatedAccountId;

	/*
	* The obfuscated account id specified.
	*/
	UPROPERTY(BlueprintReadWrite, Category = "Monetization Goodies|Android")
	FString ObfuscatedProfileId;
};

/*
* Possible purchase states.
*/
UENUM(BlueprintType)
enum class EMGAndroidPurchaseState : uint8
{
	Unspecified = 0,
	Purchased = 1,
	Pending = 2
};

/*
* Possible purchase states.
*/
UCLASS(BlueprintType)
class MONETIZATIONGOODIES_API UMGAndroidPurchase : public UMGAndroidWrapperObject
{
	GENERATED_BODY()

public:

	/*
	* Returns account identifiers that were provided when the purchase was made.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|Android")
	FMGAndroidAccountIdentifiers GetAccountIdentifiers();

	/*
	* Returns the payload specified when the purchase was acknowledged or consumed.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|Android")
	FString GetDeveloperPayload();

	/*
	* Returns a unique order identifier for the transaction.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|Android")
	FString GetOrderId();

	/*
	* Returns a String in JSON format that contains details about the purchase order.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|Android")
	FString GetOriginalJson();

	/*
	* Returns the application package from which the purchase originated.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|Android")
	FString GetPackageName();

	/*
	* Returns one of PurchaseState indicating the state of the purchase.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|Android")
	EMGAndroidPurchaseState GetPurchaseState();

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

	/*
	* Indicates whether the purchase has been acknowledged.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|Android")
	bool IsAcknowledged();

	/*
	* Indicates whether the subscription renews automatically.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|Android")
	bool IsAutoRenewing();
};