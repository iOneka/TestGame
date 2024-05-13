// Copyright (c) 2021 Nineva Studios

#include "MGAndroidPurchaseHistoryRecord.h"

#include "MonetizationGoodiesLog.h"

#include "MGAndroidBillingLibrary.h"

#if PLATFORM_ANDROID
#include "Android/Utils/MGJavaConvertor.h"
#include "Android/Utils/MGMethodCallUtils.h"
#endif

FString UMGAndroidPurchaseHistoryRecord::GetDeveloperPayload()
{
	FString Result;

	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidPurchaseHistoryRecord::GetDeveloperPayload"));

	if (!IsNativeObjectValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	Result = MGMethodCallUtils::CallStringMethod(JavaObject, "getDeveloperPayload", "()Ljava/lang/String;");
#endif

	return Result;
}

FString UMGAndroidPurchaseHistoryRecord::GetOriginalJson()
{
	FString Result;

	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidPurchaseHistoryRecord::GetOriginalJson"));

	if (!IsNativeObjectValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	Result = MGMethodCallUtils::CallStringMethod(JavaObject, "getOriginalJson", "()Ljava/lang/String;");
#endif

	return Result;
}

FDateTime UMGAndroidPurchaseHistoryRecord::GetPurchaseTime()
{
	FDateTime Result = FDateTime::MinValue();

	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidPurchaseHistoryRecord::GetPurchaseTime"));

	if (!IsNativeObjectValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	long millis = MGMethodCallUtils::CallLongMethod(JavaObject, "getPurchaseTime", "()J");
	Result = FDateTime::FromUnixTimestamp(millis / 1000);
#endif

	return Result;
}

FString UMGAndroidPurchaseHistoryRecord::GetPurchaseToken()
{
	FString Result;

	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidPurchaseHistoryRecord::GetPurchaseToken"));

	if (!IsNativeObjectValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	Result = MGMethodCallUtils::CallStringMethod(JavaObject, "getPurchaseToken", "()Ljava/lang/String;");
#endif

	return Result;
}

int UMGAndroidPurchaseHistoryRecord::GetQuantity()
{
	int Result = 1;

	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidPurchaseHistoryRecord::GetQuantity"));

	if (!IsNativeObjectValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	Result = MGMethodCallUtils::CallIntMethod(JavaObject, "getQuantity", "()I");
#endif

	return Result;
}

FString UMGAndroidPurchaseHistoryRecord::GetSignature()
{
	FString Result;

	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidPurchaseHistoryRecord::GetSignature"));

	if (!IsNativeObjectValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	Result = MGMethodCallUtils::CallStringMethod(JavaObject, "getSignature", "()Ljava/lang/String;");
#endif

	return Result;
}

TArray<FString> UMGAndroidPurchaseHistoryRecord::GetSkus()
{
	TArray<FString> Result;

	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidPurchaseHistoryRecord::GetSkus"));

	if (!IsNativeObjectValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	jobject javaResult = MGMethodCallUtils::CallStaticObjectMethod(UMGAndroidBillingLibrary::BillingLibraryClassName, "getPurchaseSkusForRecord",
		"(Lcom/android/billingclient/api/PurchaseHistoryRecord;)[Ljava/lang/String;", JavaObject);
	Result = MGJavaConvertor::ConvertToStringArray(static_cast<jobjectArray>(javaResult));
#endif

	return Result;
}
