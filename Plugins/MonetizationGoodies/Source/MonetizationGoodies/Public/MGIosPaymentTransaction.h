// Copyright (c) 2021 Nineva Studios

#pragma once

#include "MGIosWrapperObject.h"

#include "MGIosPaymentTransaction.generated.h"

class UMGIosPayment;
class UMGIosDownload;

/*
* Values representing the state of a transaction.
*/
UENUM(BlueprintType)
enum class EMGTransactionState : uint8
{
	/*
	* A transaction that is being processed by the App Store.
	*/
	Purchasing = 0,
	/*
	* A successfully processed transaction.
	*/
	Purchased = 1,
	/*
	* A failed transaction.
	*/
	Failed = 2,
	/*
	* A transaction that restores content previously purchased by the user.
	*/
	Restored = 3,
	/*
	* A transaction that is in the queue, but its final status is pending external action such as Ask to Buy.
	*/
	Deferred = 4
};

/*
* An object in the payment queue.
*/
UCLASS(BlueprintType)
class MONETIZATIONGOODIES_API UMGIosPaymentTransaction : public UMGIosWrapperObject
{
	GENERATED_BODY()

public:
	
	/*
	* The payment for the transaction.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|IOS")
	UMGIosPayment* GetPayment();
	
	/*
	* A string that uniquely identifies a successful payment transaction.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|IOS")
	FString GetTransactionIdentifier();
	
	/*
	* The date the transaction was added to the App Store’s payment queue.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|IOS")
	FDateTime GetTransactionDate();
	
	/*
	* The transaction that was restored by the App Store.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|IOS")
	UMGIosPaymentTransaction* GetOriginalTransaction();
	
	/*
	* An object describing the error that occurred while processing the transaction.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|IOS")
	FString GetError();
	
	/*
	* An array of download objects representing the downloadable content associated with the transaction.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|IOS")
	TArray<UMGIosDownload*> GetDownloads();
	
	/*
	* The current state of the transaction.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|IOS")
	EMGTransactionState GetState();
};
