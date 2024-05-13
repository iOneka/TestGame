// Copyright (c) 2021 Nineva Studios

#include "MGIosTransactionObserver.h"

#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
#include "IOS/MGTransactionObserverDelegate.h"
#endif

#include "MGIosPaymentTransaction.h"
#include "MGIosPayment.h"
#include "MGIosDownload.h"
#include "MGIosProduct.h"

#include "Async/Async.h"

#include "MonetizationGoodiesLog.h"

bool ShouldAddStorePaymentForProduct(UMGIosPayment* Payment, UMGIosProduct* Product)
{
	// Customize this method if you want to add custom implementation.

	return true;
}

UMGIosTransactionObserver* UMGIosTransactionObserver::CreateTransactionObserver()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosTransactionObserver::CreateTransactionObserver"));
	
	UMGIosTransactionObserver* Result = NewObject<UMGIosTransactionObserver>();
	
#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	MGTransactionObserverDelegate* ObserverDelegate = [MGTransactionObserverDelegate new];
	
	ObserverDelegate.onTransactionsUpdated = ^(NSArray<SKPaymentTransaction*>* transactions) {
		TArray<UMGIosPaymentTransaction*> Transactions;
		
		for (id transaction in transactions) {
			UMGIosPaymentTransaction* Transaction = NewObject<UMGIosPaymentTransaction>();
			Transaction->Init(transaction);
			
			Transactions.Add(Transaction);
		}
		
		AsyncTask(ENamedThreads::GameThread, [=]() {
			Result->OnTransactionsUpdatedDelegate.ExecuteIfBound(Transactions);
		});
	};
	ObserverDelegate.onTransactionsRemoved = ^(NSArray<SKPaymentTransaction*>* transactions) {
		TArray<UMGIosPaymentTransaction*> Transactions;
		
		for (id transaction in transactions) {
			UMGIosPaymentTransaction* Transaction = NewObject<UMGIosPaymentTransaction>();
			Transaction->Init(transaction);
			
			Transactions.Add(Transaction);
		}
		
		AsyncTask(ENamedThreads::GameThread, [=]() {
			Result->OnTransactionsRemovedDelegate.ExecuteIfBound(Transactions);
		});
	};
	ObserverDelegate.onRestoreTransactionsCompleted = ^() {
		AsyncTask(ENamedThreads::GameThread, [=]() {
			Result->OnRestoreTransactionsCompletedDelegate.ExecuteIfBound();
		});
	};
	ObserverDelegate.onRestoreTransactionsError = ^(NSError* error) {
		FString Error = error.localizedDescription;
		Result->OnRestoreTransactionsErrorDelegate.ExecuteIfBound(Error);
	};
	ObserverDelegate.onDownloadsUpdated = ^(NSArray<SKDownload*>* downloads) {
		TArray<UMGIosDownload*> Downloads;
		
		for (id download in downloads) {
			UMGIosDownload* Download = NewObject<UMGIosDownload>();
			Download->Init(download);
			
			Downloads.Add(Download);
		}
		
		AsyncTask(ENamedThreads::GameThread, [=]() {
			Result->OnDownloadsUpdatedDelegate.ExecuteIfBound(Downloads);
		});
	};
	ObserverDelegate.shouldAddStorePaymentForProduct = ^(SKPayment* payment, SKProduct *product) {
		UMGIosPayment* Payment = NewObject<UMGIosPayment>();
		Payment->Init(payment);
		
		UMGIosProduct* Product = NewObject<UMGIosProduct>();
		Product->Init(product);
		
		return ShouldAddStorePaymentForProduct(Payment, Product);
	};
	ObserverDelegate.onEntitlementsRevoked = ^(NSArray<NSString*>* entitlements) {
		TArray<FString> Entitlements;
		
		for (id entitlement in entitlements) {
			FString Entitlement = entitlement;
			
			Entitlements.Add(Entitlement);
		}
		
		AsyncTask(ENamedThreads::GameThread, [=]() {
			Result->OnEntitlementsRevokedDelegate.ExecuteIfBound(Entitlements);
		});
	};
	ObserverDelegate.onStoreFrontChanged = ^() {
		AsyncTask(ENamedThreads::GameThread, [=]() {
			Result->OnStoreFrontChangedDelegate.ExecuteIfBound();
		});
	};
	
	Result->Init(ObserverDelegate);
#endif
	
	return Result;
}
