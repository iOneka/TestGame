// Copyright (c) 2021 Nineva Studios

#include "MGIosProductDiscount.h"

#include "MonetizationGoodiesLog.h"

FString UMGIosProductDiscount::GetIdentifier()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosProductDiscount::GetIdentifier"));
	
	FString Result;
	
	if (!IsNativeObjectValid())
	{
		return Result;
	}
	
#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	Result = ((SKProductDiscount*) IOSObject).identifier;
#endif
	
	return Result;
}

EMGProductDiscountType UMGIosProductDiscount::GetType()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosProductDiscount::GetType"));
	
	EMGProductDiscountType Result = EMGProductDiscountType::Subscription;
	
	if (!IsNativeObjectValid())
	{
		return Result;
	}
	
#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	Result = (EMGProductDiscountType) ((SKProductDiscount*) IOSObject).type;
#endif
	
	return Result;
}

FString UMGIosProductDiscount::GetFormattedPrice()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosProductDiscount::GetFormattedPrice"));
	
	FString Result;
	
	if (!IsNativeObjectValid())
	{
		return Result;
	}
	
#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	SKProductDiscount* discount = (SKProductDiscount*) IOSObject;
	
	NSNumberFormatter *numberFormatter = [[NSNumberFormatter alloc] init];
	[numberFormatter setFormatterBehavior:NSNumberFormatterBehavior10_4];
	[numberFormatter setNumberStyle:NSNumberFormatterCurrencyStyle];
	[numberFormatter setLocale:discount.priceLocale];
	NSString *formattedString = [numberFormatter stringFromNumber:discount.price];
	
	Result = formattedString;
#endif
	
	return Result;
}

EMGProductPaymentType UMGIosProductDiscount::GetPaymentType()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosProductDiscount::GetPaymentType"));
	
	EMGProductPaymentType Result = EMGProductPaymentType::PayAsYouGo;
	
	if (!IsNativeObjectValid())
	{
		return Result;
	}
	
#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	Result = (EMGProductPaymentType) ((SKProductDiscount*) IOSObject).paymentMode;
#endif
	
	return Result;
}

FMGIosSubscriptionPeriod UMGIosProductDiscount::GetSubscriptionPeriod()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosProductDiscount::GetSubscriptionPeriod"));
	
	FMGIosSubscriptionPeriod Result;
	
	if (!IsNativeObjectValid())
	{
		return Result;
	}
	
#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	SKProductSubscriptionPeriod* period = ((SKProductDiscount*) IOSObject).subscriptionPeriod;
	
	if (period == nil) {
		return Result;
	}
	
	Result.NumberOfUnits = period.numberOfUnits;
	Result.Unit = (EMGProductPeriodUnit) period.unit;
#endif
	
	return Result;
}

int UMGIosProductDiscount::GetNumberOfPeriods()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosProductDiscount::GetNumberOfPeriods"));
	
	int Result = 0;
	
	if (!IsNativeObjectValid())
	{
		return Result;
	}
	
#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	Result = ((SKProductDiscount*) IOSObject).numberOfPeriods;
#endif
	
	return Result;
}
