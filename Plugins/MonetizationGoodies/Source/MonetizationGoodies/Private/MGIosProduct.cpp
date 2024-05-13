// Copyright (c) 2021 Nineva Studios

#include "MGIosProduct.h"

#include "MGIosProductDiscount.h"

#include "MonetizationGoodiesLog.h"

FString UMGIosProduct::GetProductIdentifier()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosProduct::GetProductIdentifier"));
	
	FString Result;
	
	if (!IsNativeObjectValid())
	{
		return Result;
	}
	
#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	Result = ((SKProduct*) IOSObject).productIdentifier;
#endif
	
	return Result;
}

FString UMGIosProduct::GetLocalizedDescription()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosProduct::GetLocalizedDescription"));
	
	FString Result;
	
	if (!IsNativeObjectValid())
	{
		return Result;
	}
	
#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	Result = ((SKProduct*) IOSObject).localizedDescription;
#endif
	
	return Result;
}

FString UMGIosProduct::GetLocalizedTitle()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosProduct::GetLocalizedTitle"));
	
	FString Result;
	
	if (!IsNativeObjectValid())
	{
		return Result;
	}
	
#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	Result = ((SKProduct*) IOSObject).localizedTitle;
#endif
	
	return Result;
}

FString UMGIosProduct::GetContentVersion()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosProduct::GetContentVersion"));
	
	FString Result;
	
	if (!IsNativeObjectValid())
	{
		return Result;
	}
	
#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	Result = ((SKProduct*) IOSObject).contentVersion;
#endif
	
	return Result;
}

bool UMGIosProduct::IsFamilyShareable()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosProduct::IsFamilyShareable"));
	
	bool Result = false;
	
	if (!IsNativeObjectValid())
	{
		return Result;
	}
	
#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	Result = ((SKProduct*) IOSObject).isFamilyShareable;
#endif
	
	return Result;
}

FString UMGIosProduct::GetFormattedPrice()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosProduct::GetFormattedPrice"));
	
	FString Result;
	
	if (!IsNativeObjectValid())
	{
		return Result;
	}
	
#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	SKProduct* product = (SKProduct*) IOSObject;
	
	NSNumberFormatter *numberFormatter = [[NSNumberFormatter alloc] init];
	[numberFormatter setFormatterBehavior:NSNumberFormatterBehavior10_4];
	[numberFormatter setNumberStyle:NSNumberFormatterCurrencyStyle];
	[numberFormatter setLocale:product.priceLocale];
	NSString *formattedString = [numberFormatter stringFromNumber:product.price];
	
	Result = formattedString;
#endif
	
	return Result;
}

UMGIosProductDiscount* UMGIosProduct::GetIntroductoryPrice()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosProduct::GetIntroductoryPrice"));
	
	UMGIosProductDiscount* Result = NewObject<UMGIosProductDiscount>();
	
	if (!IsNativeObjectValid())
	{
		return Result;
	}
	
#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	Result->Init(((SKProduct*) IOSObject).introductoryPrice);
#endif
	
	return Result;
}

TArray<UMGIosProductDiscount*> UMGIosProduct::GetDiscounts()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosProduct::GetDiscounts"));
	
	TArray<UMGIosProductDiscount*> Result;
	
	if (!IsNativeObjectValid())
	{
		return Result;
	}
	
#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	for (id discount in ((SKProduct*) IOSObject).discounts)
	{
		UMGIosProductDiscount* Discount = NewObject<UMGIosProductDiscount>();
		Discount->Init(discount);
		
		Result.Add(Discount);
	}
#endif
	
	return Result;
}

FString UMGIosProduct::GetSubscriptionGroupIdentifier()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosProduct::GetSubscriptionGroupIdentifier"));
	
	FString Result;
	
	if (!IsNativeObjectValid())
	{
		return Result;
	}
	
#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	Result = ((SKProduct*) IOSObject).subscriptionGroupIdentifier;
#endif
	
	return Result;
}

FMGIosSubscriptionPeriod UMGIosProduct::GetSubscriptionPeriod()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosProduct::GetSubscriptionPeriod"));
	
	FMGIosSubscriptionPeriod Result;
	
	if (!IsNativeObjectValid())
	{
		return Result;
	}
	
#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	SKProductSubscriptionPeriod* period = ((SKProduct*) IOSObject).subscriptionPeriod;
	
	if (period == nil) {
		return Result;
	}
	
	Result.NumberOfUnits = period.numberOfUnits;
	Result.Unit = (EMGProductPeriodUnit) period.unit;
#endif
	
	return Result;
}

bool UMGIosProduct::IsDownloadable()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosProduct::IsDownloadable"));
	
	bool Result = false;
	
	if (!IsNativeObjectValid())
	{
		return Result;
	}
	
#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	Result = ((SKProduct*) IOSObject).isDownloadable;
#endif
	
	return Result;
}

TArray<int64> UMGIosProduct::GetDownloadContentLengths()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosProduct::GetDownloadContentLengths"));
	
	TArray<int64> Result;
	
	if (!IsNativeObjectValid())
	{
		return Result;
	}
	
#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	for (NSNumber* length in ((SKProduct*) IOSObject).downloadContentLengths)
	{
		Result.Add((int64) length.longLongValue);
	}
#endif
	
	return Result;
}

FString UMGIosProduct::GetDownloadContentVersion()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosProduct::GetDownloadContentVersion"));
	
	FString Result;
	
	if (!IsNativeObjectValid())
	{
		return Result;
	}
	
#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	Result = ((SKProduct*) IOSObject).downloadContentVersion;
#endif
	
	return Result;
}
