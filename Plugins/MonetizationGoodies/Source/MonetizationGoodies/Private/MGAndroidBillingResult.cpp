// Copyright (c) 2021 Nineva Studios

#include "MGAndroidBillingResult.h"

#include "MonetizationGoodiesLog.h"

#if PLATFORM_ANDROID
#include "Android/Utils/MGJavaConvertor.h"
#include "Android/Utils/MGMethodCallUtils.h"
#endif

FString UMGAndroidBillingResult::GetDebugMessage()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidBillingResult::GetDebugMessage"));

	FString Result;

	if (!IsNativeObjectValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	Result = MGMethodCallUtils::CallStringMethod(JavaObject, "getDebugMessage", "()Ljava/lang/String;");
#endif

	return Result;
}

EMGAndroidBillingResponseCode UMGAndroidBillingResult::GetResponseCode()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidBillingResult::GetResponseCode"));

	EMGAndroidBillingResponseCode Result = EMGAndroidBillingResponseCode::Error;

	if (!IsNativeObjectValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	int code = MGMethodCallUtils::CallIntMethod(JavaObject, "getResponseCode", "()I");
	if (code < 0) {
		code += 100;
	}

	Result = (EMGAndroidBillingResponseCode) code;
#endif

	return Result;
}
