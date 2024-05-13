// Copyright (c) 2021 Nineva Studios

#include "MGAndroidPurchase.h"

#include "MonetizationGoodiesLog.h"

#include "MGAndroidBillingLibrary.h"

#if PLATFORM_ANDROID
#include "Android/Utils/MGJavaConvertor.h"
#include "Android/Utils/MGMethodCallUtils.h"
#endif

FMGAndroidAccountIdentifiers UMGAndroidPurchase::GetAccountIdentifiers()
{
	FMGAndroidAccountIdentifiers Result;

	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidPurchase::GetAccountIdentifiers"));

	if (!IsNativeObjectValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	jobject javaIdentifiers = MGMethodCallUtils::CallObjectMethod(JavaObject, "getAccountIdentifiers", "()Lcom/android/billingclient/api/AccountIdentifiers;");

	if (javaIdentifiers == nullptr)
	{
		return Result;
	}

	Result.ObfuscatedAccountId = MGMethodCallUtils::CallStringMethod(javaIdentifiers, "getObfuscatedAccountId", "()Ljava/lang/String;");
	Result.ObfuscatedProfileId = MGMethodCallUtils::CallStringMethod(javaIdentifiers, "getObfuscatedProfileId", "()Ljava/lang/String;");
#endif

	return Result;
}

FString UMGAndroidPurchase::GetDeveloperPayload()
{
	FString Result;

	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidPurchase::GetDeveloperPayload"));

	if (!IsNativeObjectValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	Result = MGMethodCallUtils::CallStringMethod(JavaObject, "getDeveloperPayload", "()Ljava/lang/String;");
#endif

	return Result;
}

FString UMGAndroidPurchase::GetOrderId()
{
	FString Result;

	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidPurchase::GetOrderId"));

	if (!IsNativeObjectValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	Result = MGMethodCallUtils::CallStringMethod(JavaObject, "getOrderId", "()Ljava/lang/String;");
#endif

	return Result;
}

FString UMGAndroidPurchase::GetOriginalJson()
{
	FString Result;

	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidPurchase::GetOriginalJson"));

	if (!IsNativeObjectValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	Result = MGMethodCallUtils::CallStringMethod(JavaObject, "getOriginalJson", "()Ljava/lang/String;");
#endif

	return Result;
}

FString UMGAndroidPurchase::GetPackageName()
{
	FString Result;

	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidPurchase::GetPackageName"));

	if (!IsNativeObjectValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	Result = MGMethodCallUtils::CallStringMethod(JavaObject, "getPackageName", "()Ljava/lang/String;");
#endif

	return Result;
}

EMGAndroidPurchaseState UMGAndroidPurchase::GetPurchaseState()
{
	EMGAndroidPurchaseState Result = EMGAndroidPurchaseState::Unspecified;

	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidPurchase::GetPurchaseState"));

	if (!IsNativeObjectValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	Result = (EMGAndroidPurchaseState) MGMethodCallUtils::CallIntMethod(JavaObject, "getPurchaseState", "()I");
#endif

	return Result;
}

FDateTime UMGAndroidPurchase::GetPurchaseTime()
{
	FDateTime Result = FDateTime::MinValue();

	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidPurchase::GetPurchaseTime"));

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

FString UMGAndroidPurchase::GetPurchaseToken()
{
	FString Result;

	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidPurchase::GetPurchaseToken"));

	if (!IsNativeObjectValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	Result = MGMethodCallUtils::CallStringMethod(JavaObject, "getPurchaseToken", "()Ljava/lang/String;");
#endif

	return Result;
}

int UMGAndroidPurchase::GetQuantity()
{
	int Result = 1;

	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidPurchase::GetQuantity"));

	if (!IsNativeObjectValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	Result = MGMethodCallUtils::CallIntMethod(JavaObject, "getQuantity", "()I");
#endif

	return Result;
}

FString UMGAndroidPurchase::GetSignature()
{
	FString Result;

	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidPurchase::GetSignature"));

	if (!IsNativeObjectValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	Result = MGMethodCallUtils::CallStringMethod(JavaObject, "getSignature", "()Ljava/lang/String;");
#endif

	return Result;
}

TArray<FString> UMGAndroidPurchase::GetSkus()
{
	TArray<FString> Result;

	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidPurchase::GetSkus"));

	if (!IsNativeObjectValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	jobject javaResult = MGMethodCallUtils::CallStaticObjectMethod(UMGAndroidBillingLibrary::BillingLibraryClassName, "getPurchaseSkus", 
		"(Lcom/android/billingclient/api/Purchase;)[Ljava/lang/String;", JavaObject);
	Result = MGJavaConvertor::ConvertToStringArray(static_cast<jobjectArray>(javaResult));
#endif

	return Result;
}

bool UMGAndroidPurchase::IsAcknowledged()
{
	bool Result = false;

	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidPurchase::IsAcknowledged"));

	if (!IsNativeObjectValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	Result = MGMethodCallUtils::CallBoolMethod(JavaObject, "isAcknowledged", "()Z");
#endif

	return Result;
}

bool UMGAndroidPurchase::IsAutoRenewing()
{
	bool Result = false;

	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidPurchase::IsAutoRenewing"));

	if (!IsNativeObjectValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	Result = MGMethodCallUtils::CallBoolMethod(JavaObject, "isAutoRenewing", "()Z");
#endif

	return Result;
}
