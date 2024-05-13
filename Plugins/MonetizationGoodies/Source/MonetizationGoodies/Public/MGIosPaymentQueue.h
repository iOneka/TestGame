// Copyright (c) 2021 Nineva Studios

#pragma once

#include "MGIosWrapperObject.h"
#include "MGIosPaymentQueue.generated.h"

class UMGIosPaymentTransaction;
class UMGIosStoreFront;
class UMGIosTransactionObserver;
class UMGIosPayment;
class UMGIosDownload;

/*
* A queue of payment transactions to be processed by the App Store.
*/
UCLASS()
class MONETIZATIONGOODIES_API UMGIosPaymentQueue : public UMGIosWrapperObject
{
	GENERATED_BODY()

public:
	virtual ~UMGIosPaymentQueue() override;
	
	/*
	* Returns the default payment queue instance.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|IOS")
	static UMGIosPaymentQueue* DefaultPaymentQueue();
	
	/*
	* Indicates whether the user is allowed to make payments.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|IOS")
	static bool CanMakePurchases();
	
	/*
	* The current App Store storefront for the payment queue.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|IOS")
	UMGIosStoreFront* GetStoreFront();
	
	/*
	* Adds an observer to the payment queue.
	*/
	UFUNCTION(BlueprintCallable, Category = "Monetization Goodies|IOS")
	void AddTransactionObserver(UMGIosTransactionObserver* Observer);
	
	/*
	* Removes an observer from the payment queue.
	*/
	UFUNCTION(BlueprintCallable, Category = "Monetization Goodies|IOS")
	void RemoveTransactionObserver(UMGIosTransactionObserver* Observer);
	
	/*
	* An array of all active payment queue observers.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|IOS")
	TArray<UMGIosTransactionObserver*> GetTransactionObservers();
	
	/*
	* Returns an array of pending transactions.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|IOS")
	TArray<UMGIosPaymentTransaction*> GetPendingTransactions();
	
	/*
	* Adds a payment request to the queue.
	*/
	UFUNCTION(BlueprintCallable, Category = "Monetization Goodies|IOS")
	void AddPayment(UMGIosPayment* Payment);
	
	/*
	* Notifies the App Store that the app finished processing the transaction.
	*/
	UFUNCTION(BlueprintCallable, Category = "Monetization Goodies|IOS")
	void FinishTransaction(UMGIosPaymentTransaction* Transaction);
	
	/*
	* Asks the payment queue to restore previously completed purchases.
	*/
	UFUNCTION(BlueprintCallable, Category = "Monetization Goodies|IOS")
	void RestoreCompletedTransactions();
	
	/*
	* Asks the payment queue to restore previously completed purchases, providing an opaque identifier for the user�s account.
	*/
	UFUNCTION(BlueprintCallable, Category = "Monetization Goodies|IOS")
	void RestoreCompletedTransactionsWithApplicationUsername(const FString& Username);
	
	/*
	* Asks the system to display the price consent sheet if the user has not yet responded to a subscription price increase.
	*/
	UFUNCTION(BlueprintCallable, Category = "Monetization Goodies|IOS")
	void ShowPriceConsentIfNeeded();
	
	/*
	* Displays a sheet that enables users to redeem subscription offer codes that you configure in App Store Connect.
	*/
	UFUNCTION(BlueprintCallable, Category = "Monetization Goodies|IOS")
	void PresentCodeRedemptionSheet();
	
	/*
	* Adds a set of downloads to the download list.
	*/
	UFUNCTION(BlueprintCallable, Category = "Monetization Goodies|IOS")
	void StartDownloads(const TArray<UMGIosDownload*> Downloads);
	
	/*
	* Pauses a set of downloads.
	*/
	UFUNCTION(BlueprintCallable, Category = "Monetization Goodies|IOS")
	void PauseDownloads(const TArray<UMGIosDownload*> Downloads);
	
	/*
	* Resumes a set of downloads.
	*/
	UFUNCTION(BlueprintCallable, Category = "Monetization Goodies|IOS")
	void ResumeDownloads(const TArray<UMGIosDownload*> Downloads);
	
	/*
	* Removes a set of downloads from the download list.
	*/
	UFUNCTION(BlueprintCallable, Category = "Monetization Goodies|IOS")
	void CancelDownloads(const TArray<UMGIosDownload*> Downloads);

	/*
	* Gets an iOS Receipt.
	*/
	UFUNCTION(BlueprintCallable, Category = "Monetization Goodies|IOS")
	static FString GetReceipt();
};
