// Copyright (c) 2021 Nineva Studios

#pragma once

#include "MGIosWrapperObject.h"

#include "Misc/Guid.h"
#include "Misc/DateTime.h"

#include "MGIosPaymentDiscount.generated.h"

/*
* The signed discount applied to a payment.
*/
UCLASS(BlueprintType)
class MONETIZATIONGOODIES_API UMGIosPaymentDiscount : public UMGIosWrapperObject
{
	GENERATED_BODY()

public:
	
	/*
	* Initializes the payment discount with a signature and the parameters used by the signature.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|IOS")
	static UMGIosPaymentDiscount* CreatePaymentDiscount(const FString& Identifier, const FString& KeyIdentifier, const FGuid& Nonce, const FString& Signature, const FDateTime& Timestamp);
	
	/*
	* A string used to uniquely identify a discount offer for a product.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|IOS")
	FString GetIdentifier();
	
	/*
	* A string that identifies the key used to generate the signature.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|IOS")
	FString GetKeyIdentifier();
	
	/*
	* A universally unique ID (UUID) value that you define.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|IOS")
	FGuid GetNonce();
	
	/*
	* A string representing the properties of a specific promotional offer, cryptographically signed.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|IOS")
	FString GetSignature();
	
	/*
	* The date and time of the signature's creation in milliseconds, formatted in Unix epoch time.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|IOS")
	FDateTime GetTimestamp();
};
