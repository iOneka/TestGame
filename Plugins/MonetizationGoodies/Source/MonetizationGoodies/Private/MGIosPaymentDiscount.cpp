// Copyright (c) 2021 Nineva Studios

#include "MGIosPaymentDiscount.h"

#include "MonetizationGoodiesLog.h"

UMGIosPaymentDiscount* UMGIosPaymentDiscount::CreatePaymentDiscount(const FString& Identifier, const FString& KeyIdentifier, const FGuid& Nonce, const FString& Signature, const FDateTime& Timestamp)
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosPaymentDiscount::CreatePaymentDiscount"));
	
	UMGIosPaymentDiscount* Result = NewObject<UMGIosPaymentDiscount>();
	
#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	NSUUID* nonce = [[NSUUID alloc] initWithUUIDString:Nonce.ToString(EGuidFormats::DigitsWithHyphens).GetNSString()];
	
	int64 MillisSinceEpoch = Timestamp.ToUnixTimestamp() * 1000;
	
	SKPaymentDiscount* discount = [[SKPaymentDiscount alloc] initWithIdentifier:Identifier.GetNSString()
																  keyIdentifier:KeyIdentifier.GetNSString()
																		  nonce:nonce
																	  signature:Signature.GetNSString()
																	  timestamp:[NSNumber numberWithLongLong:MillisSinceEpoch]];
	
	Result->Init(discount);
#endif
	
	return Result;
}

FString UMGIosPaymentDiscount::GetIdentifier()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosPaymentDiscount::GetIdentifier"));
	
	FString Result;
	
	if (!IsNativeObjectValid())
	{
		return Result;
	}
	
#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	Result = ((SKPaymentDiscount*) IOSObject).identifier;
#endif
	
	return Result;
}

FString UMGIosPaymentDiscount::GetKeyIdentifier()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosPaymentDiscount::GetKeyIdentifier"));
	
	FString Result;
	
	if (!IsNativeObjectValid())
	{
		return Result;
	}
	
#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	Result = ((SKPaymentDiscount*) IOSObject).keyIdentifier;
#endif
	
	return Result;
}

FGuid UMGIosPaymentDiscount::GetNonce()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosPaymentDiscount::GetNonce"));
	
	FGuid Result;
	
	if (!IsNativeObjectValid())
	{
		return Result;
	}
	
#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	FString UuidString = ((SKPaymentDiscount*) IOSObject).nonce.UUIDString;
	FGuid::Parse(UuidString, Result);
#endif
	
	return Result;
}

FString UMGIosPaymentDiscount::GetSignature()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosPaymentDiscount::GetSignature"));
	
	FString Result;
	
	if (!IsNativeObjectValid())
	{
		return Result;
	}
	
#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	Result = ((SKPaymentDiscount*) IOSObject).signature;
#endif
	
	return Result;
}

FDateTime UMGIosPaymentDiscount::GetTimestamp()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosPaymentDiscount::GetTimestamp"));
	
	FDateTime Result = FDateTime::MinValue();
	
	if (!IsNativeObjectValid())
	{
		return Result;
	}
	
#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	int64 Timestamp = [((SKPaymentDiscount*) IOSObject).timestamp longLongValue] / 1000;
	
	Result = FDateTime::FromUnixTimestamp(Timestamp);
#endif
	
	return Result;
}
