// Copyright (c) 2021 Nineva Studios

#include "MGIosPaymentTransaction.h"

#include "MGIosPayment.h"
#include "MGIosDownload.h"

#include "MonetizationGoodiesLog.h"

#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
#import <StoreKit/StoreKit.h>
#endif

UMGIosPayment* UMGIosPaymentTransaction::GetPayment()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosPaymentTransaction::GetPayment"));
	
	UMGIosPayment* Result = NewObject<UMGIosPayment>();
	
	if (!IsNativeObjectValid())
	{
		return Result;
	}
	
#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	Result->Init(((SKPaymentTransaction*) IOSObject).payment);
#endif
	
	return Result;
}

FString UMGIosPaymentTransaction::GetTransactionIdentifier()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosPaymentTransaction::GetTransactionIdentifier"));
	
	FString Result;
	
	if (!IsNativeObjectValid())
	{
		return Result;
	}
	
#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	Result = ((SKPaymentTransaction*) IOSObject).transactionIdentifier;
#endif
	
	return Result;
}

FDateTime UMGIosPaymentTransaction::GetTransactionDate()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosPaymentTransaction::GetTransactionDate"));
	
	FDateTime Result = FDateTime::MinValue();
	
	if (!IsNativeObjectValid())
	{
		return Result;
	}
	
#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	NSDate* date = ((SKPaymentTransaction*) IOSObject).transactionDate;
	NSTimeInterval interval = [date timeIntervalSince1970];
	
	Result = FDateTime::FromUnixTimestamp((long) interval);
#endif
	
	return Result;
}

UMGIosPaymentTransaction* UMGIosPaymentTransaction::GetOriginalTransaction()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosPaymentTransaction::GetOriginalTransaction"));
	
	UMGIosPaymentTransaction* Result = NewObject<UMGIosPaymentTransaction>();
	
	if (!IsNativeObjectValid())
	{
		return Result;
	}
	
#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	Result->Init(((SKPaymentTransaction*) IOSObject).originalTransaction);
#endif
	
	return Result;
}

FString UMGIosPaymentTransaction::GetError()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosPaymentTransaction::GetError"));
	
	FString Result;
	
	if (!IsNativeObjectValid())
	{
		return Result;
	}
	
#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	Result = ((SKPaymentTransaction*) IOSObject).error.localizedDescription;
#endif
	
	return Result;
}

TArray<UMGIosDownload*> UMGIosPaymentTransaction::GetDownloads()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosPaymentTransaction::GetDownloads"));
	
	TArray<UMGIosDownload*> Result;
	
	if (!IsNativeObjectValid())
	{
		return Result;
	}
	
#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	for (id download in ((SKPaymentTransaction*) IOSObject).downloads)
	{
		UMGIosDownload* Download = NewObject<UMGIosDownload>();
		Download->Init(download);
	}
#endif
	
	return Result;
}

EMGTransactionState UMGIosPaymentTransaction::GetState()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosPaymentTransaction::GetState"));
	
	EMGTransactionState Result = EMGTransactionState::Failed;
	
	if (!IsNativeObjectValid())
	{
		return Result;
	}
	
#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	Result = (EMGTransactionState) ((SKPaymentTransaction*) IOSObject).transactionState;
#endif
	
	return Result;
}
