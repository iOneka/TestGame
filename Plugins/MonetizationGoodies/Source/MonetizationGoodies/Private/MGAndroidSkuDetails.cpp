// Copyright (c) 2021 Nineva Studios

#include "MGAndroidSkuDetails.h"

#include "MonetizationGoodiesLog.h"

#include "MGAndroidBillingLibrary.h"

#if PLATFORM_ANDROID
#include "Android/Utils/MGJavaConvertor.h"
#include "Android/Utils/MGMethodCallUtils.h"
#endif

FString UMGAndroidSkuDetails::GetDescription()
{
	FString Result;

	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidSkuDetails::GetDescription"));

	if (!IsNativeObjectValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	Result = MGMethodCallUtils::CallStringMethod(JavaObject, "getDescription", "()Ljava/lang/String;");
#endif

	return Result;
}

FString UMGAndroidSkuDetails::GetFreeTrialPeriod()
{
	FString Result;

	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidSkuDetails::GetFreeTrialPeriod"));

	if (!IsNativeObjectValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	Result = MGMethodCallUtils::CallStringMethod(JavaObject, "getFreeTrialPeriod", "()Ljava/lang/String;");
#endif

	return Result;
}

FString UMGAndroidSkuDetails::GetIconUrl()
{
	FString Result;

	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidSkuDetails::GetIconUrl"));

	if (!IsNativeObjectValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	Result = MGMethodCallUtils::CallStringMethod(JavaObject, "getIconUrl", "()Ljava/lang/String;");
#endif

	return Result;
}

FString UMGAndroidSkuDetails::GetIntroductoryPrice()
{
	FString Result;

	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidSkuDetails::GetIntroductoryPrice"));

	if (!IsNativeObjectValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	Result = MGMethodCallUtils::CallStringMethod(JavaObject, "getIntroductoryPrice", "()Ljava/lang/String;");
#endif

	return Result;
}

int64 UMGAndroidSkuDetails::GetIntroductoryPriceAmountMicros()
{
	int64 Result = 0;

	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidSkuDetails::GetIntroductoryPriceAmountMicros"));

	if (!IsNativeObjectValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	Result = MGMethodCallUtils::CallLongMethod(JavaObject, "getIntroductoryPriceAmountMicros", "()J");
#endif

	return Result;
}

int UMGAndroidSkuDetails::GetIntroductoryPriceCycles()
{
	int Result = 0;

	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidSkuDetails::GetIntroductoryPriceCycles"));

	if (!IsNativeObjectValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	Result = MGMethodCallUtils::CallIntMethod(JavaObject, "getIntroductoryPriceCycles", "()I");
#endif

	return Result;
}

FString UMGAndroidSkuDetails::GetIntroductoryPricePeriod()
{
	FString Result;

	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidSkuDetails::GetIntroductoryPricePeriod"));

	if (!IsNativeObjectValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	Result = MGMethodCallUtils::CallStringMethod(JavaObject, "getIntroductoryPricePeriod", "()Ljava/lang/String;");
#endif

	return Result;
}

FString UMGAndroidSkuDetails::GetOriginalJson()
{
	FString Result;

	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidSkuDetails::GetOriginalJson"));

	if (!IsNativeObjectValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	Result = MGMethodCallUtils::CallStringMethod(JavaObject, "getOriginalJson", "()Ljava/lang/String;");
#endif

	return Result;
}

FString UMGAndroidSkuDetails::GetOriginalPrice()
{
	FString Result;

	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidSkuDetails::GetOriginalPrice"));

	if (!IsNativeObjectValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	Result = MGMethodCallUtils::CallStringMethod(JavaObject, "getOriginalPrice", "()Ljava/lang/String;");
#endif

	return Result;
}

int64 UMGAndroidSkuDetails::GetOriginalPriceAmountMicros()
{
	int64 Result = 0;

	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidSkuDetails::GetOriginalPriceAmountMicros"));

	if (!IsNativeObjectValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	Result = MGMethodCallUtils::CallLongMethod(JavaObject, "getOriginalPriceAmountMicros", "()J");
#endif

	return Result;
}

FString UMGAndroidSkuDetails::GetPrice()
{
	FString Result;

	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidSkuDetails::GetPrice"));

	if (!IsNativeObjectValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	Result = MGMethodCallUtils::CallStringMethod(JavaObject, "getPrice", "()Ljava/lang/String;");
#endif

	return Result;
}

int64 UMGAndroidSkuDetails::GetPriceAmountMicros()
{
	int64 Result = 0;

	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidSkuDetails::GetPriceAmountMicros"));

	if (!IsNativeObjectValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	Result = MGMethodCallUtils::CallLongMethod(JavaObject, "getPriceAmountMicros", "()J");
#endif

	return Result;
}

FString UMGAndroidSkuDetails::GetPriceCurrencyCode()
{
	FString Result;

	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidSkuDetails::GetPriceCurrencyCode"));

	if (!IsNativeObjectValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	Result = MGMethodCallUtils::CallStringMethod(JavaObject, "getPriceCurrencyCode", "()Ljava/lang/String;");
#endif

	return Result;
}

FString UMGAndroidSkuDetails::GetSku()
{
	FString Result;

	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidSkuDetails::GetSku"));

	if (!IsNativeObjectValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	Result = MGMethodCallUtils::CallStringMethod(JavaObject, "getSku", "()Ljava/lang/String;");
#endif

	return Result;
}

FString UMGAndroidSkuDetails::GetSubscriptionPeriod()
{
	FString Result;

	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidSkuDetails::GetSubscriptionPeriod"));

	if (!IsNativeObjectValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	Result = MGMethodCallUtils::CallStringMethod(JavaObject, "getSubscriptionPeriod", "()Ljava/lang/String;");
#endif

	return Result;
}

FString UMGAndroidSkuDetails::GetTitle()
{
	FString Result;

	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidSkuDetails::GetTitle"));

	if (!IsNativeObjectValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	Result = MGMethodCallUtils::CallStringMethod(JavaObject, "getTitle", "()Ljava/lang/String;");
#endif

	return Result;
}

FString UMGAndroidSkuDetails::GetType()
{
	FString Result;

	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidSkuDetails::GetType"));

	if (!IsNativeObjectValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	Result = MGMethodCallUtils::CallStringMethod(JavaObject, "getType", "()Ljava/lang/String;");
#endif

	return Result;
}
