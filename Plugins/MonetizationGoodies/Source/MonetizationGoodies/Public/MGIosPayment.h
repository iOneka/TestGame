// Copyright (c) 2021 Nineva Studios

#pragma once

#include "MGIosWrapperObject.h"

#include "MGIosPayment.generated.h"

class UMGIosPaymentDiscount;
class UMGIosProduct;

/*
* Structure for payment creation.
*/
USTRUCT(BlueprintType)
struct MONETIZATIONGOODIES_API FMGIosPaymentData
{
	GENERATED_BODY()
	
	/*
	* A string that identifies a product that can be purchased from within your app.
	*/
	UPROPERTY(BlueprintReadWrite, Category = "Monetization Goodies|IOS")
	FString ProductIdentifier = "";
	
	/*
	* The number of items the user wants to purchase.
	*/
	UPROPERTY(BlueprintReadWrite, Category = "Monetization Goodies|IOS")
	int Quantity = 1;
	
	/*
	* A string that associates the payment transaction with a user on your own service.
	*/
	UPROPERTY(BlueprintReadWrite, Category = "Monetization Goodies|IOS")
	FString ApplicationUsername = "";
	
	/*
	* A Boolean value that produces an 'ask to buy' flow for this payment in the sandbox.
	*/
	UPROPERTY(BlueprintReadWrite, Category = "Monetization Goodies|IOS")
	bool SimulatesAskToBuyInSandbox = false;
	
	/*
	* The details of the discount offer to apply to the payment.
	*/
	UPROPERTY(BlueprintReadWrite, Category = "Monetization Goodies|IOS")
	UMGIosPaymentDiscount* PaymentDiscount = nullptr;
};

/*
* A request to the App Store to process payment for additional functionality offered by your app.
*/
UCLASS(BlueprintType)
class MONETIZATIONGOODIES_API UMGIosPayment : public UMGIosWrapperObject
{
	GENERATED_BODY()

public:
	
	/*
	* Create a payment request for a specific product.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|IOS")
	static UMGIosPayment* CreatePayment(UMGIosProduct* Product, const FMGIosPaymentData& PaymentData);
	
	/*
	* A string that identifies a product that can be purchased from within your app.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|IOS")
	FString GetProductIdentifier();
	
	/*
	* The number of items the user wants to purchase.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|IOS")
	int GetQuantity();
	
	/*
	* A string that associates the payment transaction with a user on your own service.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|IOS")
	FString GetApplicationUsername();
	
	/*
	* A Boolean value that produces an 'ask to buy' flow for this payment in the sandbox.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|IOS")
	bool GetSimulatesAskToBuyInSandbox();
	
	/*
	* The details of the discount offer to apply to the payment.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|IOS")
	UMGIosPaymentDiscount* GetPaymentDiscount();
};
