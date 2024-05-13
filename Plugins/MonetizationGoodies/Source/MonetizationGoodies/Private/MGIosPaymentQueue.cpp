// Copyright (c) 2021 Nineva Studios

#include "MGIosPaymentQueue.h"

#include "MGIosPaymentTransaction.h"
#include "MGIosStoreFront.h"
#include "MGIosTransactionObserver.h"
#include "MGIosPayment.h"
#include "MGIosDownload.h"

#include "MonetizationGoodiesLog.h"

#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
#import <StoreKit/StoreKit.h>

#include "IOS/MGPaymentQueueDelegate.h"

MGPaymentQueueDelegate* PaymentQueueDelegateGlobalRef;
#endif

UMGIosPaymentQueue::~UMGIosPaymentQueue()
{
#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	if (PaymentQueueDelegateGlobalRef != nil) {
		[PaymentQueueDelegateGlobalRef release];
		PaymentQueueDelegateGlobalRef = nil;
	}
#endif
}

bool ShouldContinueTransaction(UMGIosPaymentTransaction* Transaction, UMGIosStoreFront* StoreFront)
{
	// Customize this method if you want to add custom implementation.

	return true;
}

bool ShouldShowPriceConsent()
{
	// Customize this method if you want to add custom implementation.

	return true;
}

UMGIosPaymentQueue* UMGIosPaymentQueue::DefaultPaymentQueue()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosPaymentQueue::DefaultPaymentQueue"));

	UMGIosPaymentQueue* Queue = NewObject<UMGIosPaymentQueue>();

#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	Queue->Init([SKPaymentQueue defaultQueue]);

	if (@available(iOS 13, *)) {
		MGPaymentQueueDelegate *PaymentQueueDelegate = [[MGPaymentQueueDelegate alloc]init];

		PaymentQueueDelegate.shouldContinueTransactionDelegate = ^ (SKPaymentTransaction * transaction, SKStorefront * newStorefront) {
			UMGIosPaymentTransaction* Transaction = NewObject<UMGIosPaymentTransaction>();
			Transaction->Init(transaction);

			UMGIosStoreFront* StoreFront = NewObject<UMGIosStoreFront>();
			StoreFront->Init(newStorefront);

			return ShouldContinueTransaction(Transaction, StoreFront);
		};

		PaymentQueueDelegate.shouldShowPriceConsentDelegate = ^ () {
			return ShouldShowPriceConsent();
		};

		PaymentQueueDelegateGlobalRef = PaymentQueueDelegate;
		((SKPaymentQueue*) Queue->IOSObject).delegate = PaymentQueueDelegate;
	}
#endif

	return Queue;
}

bool UMGIosPaymentQueue::CanMakePurchases()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosPaymentQueue::CanMakePurchases"));

	bool Result = false;

#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	Result = [SKPaymentQueue canMakePayments];
#endif

	return Result;
}

UMGIosStoreFront* UMGIosPaymentQueue::GetStoreFront()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosPaymentQueue::GetStoreFront"));

	UMGIosStoreFront* StoreFront = NewObject<UMGIosStoreFront>();

	if (!IsNativeObjectValid())
	{
		return StoreFront;
	}

#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	if (@available(iOS 13, *)) {
		StoreFront->Init(((SKPaymentQueue*) IOSObject).storefront);
	}
#endif

	return StoreFront;
}

void UMGIosPaymentQueue::AddTransactionObserver(UMGIosTransactionObserver* Observer)
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosPaymentQueue::AddTransactionObserver"));

	if (!IsNativeObjectValid())
	{
		return;
	}

	if (!Observer->IsNativeObjectValid())
	{
		return;
	}

#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	[((SKPaymentQueue*) IOSObject) addTransactionObserver:Observer->IOSObject];
#endif
}

void UMGIosPaymentQueue::RemoveTransactionObserver(UMGIosTransactionObserver* Observer)
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosPaymentQueue::RemoveTransactionObserver"));

	if (!IsNativeObjectValid())
	{
		return;
	}

	if (!Observer->IsNativeObjectValid())
	{
		return;
	}

#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	[((SKPaymentQueue*) IOSObject) removeTransactionObserver:Observer->IOSObject];
#endif
}

TArray<UMGIosTransactionObserver*> UMGIosPaymentQueue::GetTransactionObservers()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosPaymentQueue::GetTransactionObservers"));

	TArray<UMGIosTransactionObserver*> Result;

	if (!IsNativeObjectValid())
	{
		return Result;
	}

#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	NSArray<id<SKPaymentTransactionObserver>>* transactionObservers = ((SKPaymentQueue*) IOSObject).transactionObservers;
	for (id observer in transactionObservers) {
		UMGIosTransactionObserver* Observer = NewObject<UMGIosTransactionObserver>();
		Observer->Init(observer);
		
		Result.Add(Observer);
	}
#endif

	return Result;
}

TArray<UMGIosPaymentTransaction*> UMGIosPaymentQueue::GetPendingTransactions()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosPaymentQueue::GetPendingTransactions"));

	TArray<UMGIosPaymentTransaction*> Result;

	if (!IsNativeObjectValid())
	{
		return Result;
	}

#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	NSArray<SKPaymentTransaction*>* transactions = ((SKPaymentQueue*) IOSObject).transactions;
	for (id transaction in transactions) {
		UMGIosPaymentTransaction* Transaction = NewObject<UMGIosPaymentTransaction>();
		Transaction->Init(transaction);
		
		Result.Add(Transaction);
	}
#endif

	return Result;
}

void UMGIosPaymentQueue::AddPayment(UMGIosPayment* Payment)
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosPaymentQueue::AddPayment"));

	if (!IsNativeObjectValid())
	{
		return;
	}

	if (!Payment->IsNativeObjectValid())
	{
		return;
	}

#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	[((SKPaymentQueue*) IOSObject) addPayment:Payment->IOSObject];
#endif
}

void UMGIosPaymentQueue::FinishTransaction(UMGIosPaymentTransaction* Transaction)
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosPaymentQueue::FinishTransaction"));

	if (!IsNativeObjectValid())
	{
		return;
	}

	if (!Transaction->IsNativeObjectValid())
	{
		return;
	}

#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	[((SKPaymentQueue*) IOSObject) finishTransaction:Transaction->IOSObject];
#endif
}

void UMGIosPaymentQueue::RestoreCompletedTransactions()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosPaymentQueue::RestoreCompletedTransactions"));

	if (!IsNativeObjectValid())
	{
		return;
	}

#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	[((SKPaymentQueue*) IOSObject) restoreCompletedTransactions];
#endif
}

void UMGIosPaymentQueue::RestoreCompletedTransactionsWithApplicationUsername(const FString& Username)
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosPaymentQueue::RestoreCompletedTransactionsWithApplicationUsername"));

	if (!IsNativeObjectValid())
	{
		return;
	}

#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	[((SKPaymentQueue*) IOSObject) restoreCompletedTransactionsWithApplicationUsername:Username.GetNSString()];
#endif
}

void UMGIosPaymentQueue::ShowPriceConsentIfNeeded()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosPaymentQueue::ShowPriceConsentIfNeeded"));

	if (!IsNativeObjectValid())
	{
		return;
	}

#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	if (@available(iOS 13.4, *)) {
		[((SKPaymentQueue*) IOSObject) showPriceConsentIfNeeded];
	}
#endif
}

void UMGIosPaymentQueue::PresentCodeRedemptionSheet()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosPaymentQueue::PresentCodeRedemptionSheet"));

	if (!IsNativeObjectValid())
	{
		return;
	}

#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	if (@available(iOS 14, *)) {
		[((SKPaymentQueue*) IOSObject) presentCodeRedemptionSheet];
	}
#endif
}

void UMGIosPaymentQueue::StartDownloads(const TArray<UMGIosDownload*> Downloads)
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosPaymentQueue::StartDownloads"));

	if (!IsNativeObjectValid())
	{
		return;
	}

#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	NSMutableArray* downloads = [NSMutableArray array];
	
	for (auto Download : Downloads) {
		if (!Download->IsNativeObjectValid())
		{
			continue;
		}
		
		[downloads addObject:Download->IOSObject];
	}
	
	[((SKPaymentQueue*) IOSObject) startDownloads:downloads];
#endif
}

void UMGIosPaymentQueue::PauseDownloads(const TArray<UMGIosDownload*> Downloads)
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosPaymentQueue::PauseDownloads"));

	if (!IsNativeObjectValid())
	{
		return;
	}

#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	NSMutableArray* downloads = [NSMutableArray array];
	
	for (auto Download : Downloads) {
		if (!Download->IsNativeObjectValid())
		{
			continue;
		}
		
		[downloads addObject:Download->IOSObject];
	}
	
	[((SKPaymentQueue*) IOSObject) pauseDownloads:downloads];
#endif
}

void UMGIosPaymentQueue::ResumeDownloads(const TArray<UMGIosDownload*> Downloads)
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosPaymentQueue::ResumeDownloads"));

	if (!IsNativeObjectValid())
	{
		return;
	}

#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	NSMutableArray* downloads = [NSMutableArray array];
	
	for (auto Download : Downloads) {
		if (!Download->IsNativeObjectValid())
		{
			continue;
		}
		
		[downloads addObject:Download->IOSObject];
	}
	
	[((SKPaymentQueue*) IOSObject) resumeDownloads:downloads];
#endif
}

void UMGIosPaymentQueue::CancelDownloads(const TArray<UMGIosDownload*> Downloads)
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosPaymentQueue::CancelDownloads"));

	if (!IsNativeObjectValid())
	{
		return;
	}

#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	NSMutableArray* downloads = [NSMutableArray array];
	
	for (auto Download : Downloads) {
		if (!Download->IsNativeObjectValid())
		{
			continue;
		}
		
		[downloads addObject:Download->IOSObject];
	}
	
	[((SKPaymentQueue*) IOSObject) cancelDownloads:downloads];
#endif
}

FString UMGIosPaymentQueue::GetReceipt()
{
	FString Result = "";
#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
    FString ReceiptData;
    
    NSURL* nsReceiptUrl = [[NSBundle mainBundle] appStoreReceiptURL];
    NSData* nsReceiptData = [NSData dataWithContentsOfURL : nsReceiptUrl];
    if (nsReceiptData)
    {
        NSString* nsEncodedReceiptData = [nsReceiptData base64EncodedStringWithOptions : NSDataBase64EncodingEndLineWithLineFeed];
        Result = nsEncodedReceiptData;
    }
#endif
	return Result;
}
