// Copyright (c) 2021 Nineva Studios

#pragma once

#include "MGIosWrapperObject.h"

#include "MGIosTransactionObserver.generated.h"

class UMGIosPaymentTransaction;
class UMGIosPayment;
class UMGIosDownload;
class UMGIosProduct;

DECLARE_DYNAMIC_DELEGATE_OneParam(FTransactionsDelegate, const TArray<UMGIosPaymentTransaction*>&, Transactions);
DECLARE_DYNAMIC_DELEGATE(FMGVoidDelegate);
DECLARE_DYNAMIC_DELEGATE_OneParam(FMGStringDelegate, const FString&, String);
DECLARE_DYNAMIC_DELEGATE_OneParam(FMGStringsDelegate, const TArray<FString>&, Strings);
DECLARE_DYNAMIC_DELEGATE_OneParam(FMGDownloadsDelegate, const TArray<UMGIosDownload*>&, Downloads);

/*
* A set of methods that process transactions, unlock purchased functionality, and continue promoted in-app purchases.
*/
UCLASS(BlueprintType)
class MONETIZATIONGOODIES_API UMGIosTransactionObserver : public UMGIosWrapperObject
{
	GENERATED_BODY()
	
public:
	
	/*
	* Create a new transaction observer to add to the payment queue.
	*/
	UFUNCTION(BlueprintCallable, Category = "Monetization Goodies|IOS")
	static UMGIosTransactionObserver* CreateTransactionObserver();
	
	/*
	* Bind a callback that is invoked when one or more transactions have been updated.
	*/
	UFUNCTION(BlueprintCallable, Category = "Monetization Goodies|IOS")
	void BindOnTransactionsUpdatedDelegate(const FTransactionsDelegate& Delegate) {
		OnTransactionsUpdatedDelegate = Delegate;
	}
	
	/*
	* Bind a callback that is invoked when one or more transactions have been removed from the queue.
	*/
	UFUNCTION(BlueprintCallable, Category = "Monetization Goodies|IOS")
	void BindOnTransactionsRemovedDelegate(const FTransactionsDelegate& Delegate) {
		OnTransactionsRemovedDelegate = Delegate;
	}
	
	/*
	* Bind a callback that is invoked when the payment queue has finished sending restored transactions.
	*/
	UFUNCTION(BlueprintCallable, Category = "Monetization Goodies|IOS")
	void BindOnRestoreTransactionsCompletedDelegate(const FMGVoidDelegate& Delegate) {
		OnRestoreTransactionsCompletedDelegate = Delegate;
	}
	
	/*
	* Bind a callback that is invoked when an error occurred while restoring transactions.
	*/
	UFUNCTION(BlueprintCallable, Category = "Monetization Goodies|IOS")
	void BindOnRestoreTransactionsErrorDelegate(const FMGStringDelegate& Delegate) {
		OnRestoreTransactionsErrorDelegate = Delegate;
	}
	
	/*
	* Bind a callback that is invoked when the payment queue has updated one or more download objects.
	*/
	UFUNCTION(BlueprintCallable, Category = "Monetization Goodies|IOS")
	void BindOnDownloadsUpdatedDelegate(const FMGDownloadsDelegate& Delegate) {
		OnDownloadsUpdatedDelegate = Delegate;
	}
	
	/*
	* Bind a callback that is invoked when the user is no longer entitled to one or more family-shared purchases.
	*/
	UFUNCTION(BlueprintCallable, Category = "Monetization Goodies|IOS")
	void BindOnEntitlementsRevokedDelegate(const FMGStringsDelegate& Delegate) {
		OnEntitlementsRevokedDelegate = Delegate;
	}
	
	/*
	* Bind a callback that is invoked when the storefront for the payment queue has changed.
	*/
	UFUNCTION(BlueprintCallable, Category = "Monetization Goodies|IOS")
	void BindOnStoreFrontChangedDelegate(const FMGVoidDelegate& Delegate) {
		OnStoreFrontChangedDelegate = Delegate;
	}
	
	FTransactionsDelegate OnTransactionsUpdatedDelegate;
	FTransactionsDelegate OnTransactionsRemovedDelegate;
	FMGVoidDelegate OnRestoreTransactionsCompletedDelegate;
	FMGStringDelegate OnRestoreTransactionsErrorDelegate;
	FMGDownloadsDelegate OnDownloadsUpdatedDelegate;
	FMGStringsDelegate OnEntitlementsRevokedDelegate;
	FMGVoidDelegate OnStoreFrontChangedDelegate;
};
