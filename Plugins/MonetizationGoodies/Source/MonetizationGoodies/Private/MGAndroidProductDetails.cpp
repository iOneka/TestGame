// Copyright (c) 2021 Nineva Studios

#include "MGAndroidProductDetails.h"

#include "MGAndroidBillingLibrary.h"
#include "MonetizationGoodiesLog.h"

#if PLATFORM_ANDROID
#include "Android/Utils/MGJavaConvertor.h"
#include "Android/Utils/MGMethodCallUtils.h"
#endif


FString UMGAndroidProductDetails::GetDescription()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidProductDetails::GetDescription"));

	FString Result;

	if (!IsNativeObjectValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	Result = MGMethodCallUtils::CallStringMethod(JavaObject, "getDescription", "()Ljava/lang/String;");
#endif

	return Result;
}

FString UMGAndroidProductDetails::GetName()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidProductDetails::GetName"));

	FString Result;

	if (!IsNativeObjectValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	Result = MGMethodCallUtils::CallStringMethod(JavaObject, "getName", "()Ljava/lang/String;");
#endif

	return Result;
}

FString UMGAndroidProductDetails::GetProductId()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidProductDetails::GetProductId"));

	FString Result;

	if (!IsNativeObjectValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	Result = MGMethodCallUtils::CallStringMethod(JavaObject, "getProductId", "()Ljava/lang/String;");
#endif

	return Result;
}

FString UMGAndroidProductDetails::GetProductType()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidProductDetails::GetProductType"));

	FString Result;

	if (!IsNativeObjectValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	Result = MGMethodCallUtils::CallStringMethod(JavaObject, "getProductType", "()Ljava/lang/String;");
#endif

	return Result;
}

FString UMGAndroidProductDetails::GetTitle()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidProductDetails::GetTitle"));

	FString Result;

	if (!IsNativeObjectValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	Result = MGMethodCallUtils::CallStringMethod(JavaObject, "getTitle", "()Ljava/lang/String;");
#endif

	return Result;
}

FOneTimePurchaseOfferDetails UMGAndroidProductDetails::GetOneTimePurchaseOfferDetails()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidProductDetails::GetOneTimePurchaseOfferDetails"));

	FOneTimePurchaseOfferDetails Result;

	if (!IsNativeObjectValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	jobject javaDetails = MGMethodCallUtils::CallObjectMethod(JavaObject, "getOneTimePurchaseOfferDetails", "()Lcom/android/billingclient/api/ProductDetails$OneTimePurchaseOfferDetails;");
	if (javaDetails == nullptr)
	{
		return Result;
	}

	Result.FormattedPrice = MGMethodCallUtils::CallStringMethod(javaDetails, "getFormattedPrice", "()Ljava/lang/String;");
	Result.PriceAmountMicros = MGMethodCallUtils::CallLongMethod(javaDetails, "getPriceAmountMicros", "()J");
	Result.PriceCurrencyCode = MGMethodCallUtils::CallStringMethod(javaDetails, "getPriceCurrencyCode", "()Ljava/lang/String;");
	Result.IsValid = true;
#endif

	return Result;
}

TArray<FSubscriptionOfferDetails> UMGAndroidProductDetails::GetSubscriptionOfferDetails()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidProductDetails::GetSubscriptionOfferDetails"));

	TArray<FSubscriptionOfferDetails> Result;

	if (!IsNativeObjectValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	jobject javaDetails = MGMethodCallUtils::CallStaticObjectMethod(UMGAndroidBillingLibrary::BillingLibraryClassName, 
		"getSubscriptionOfferDetails", "(Lcom/android/billingclient/api/ProductDetails;)[Lcom/android/billingclient/api/ProductDetails$SubscriptionOfferDetails;", JavaObject);
	jobjectArray javaDetailsArray = static_cast<jobjectArray>(javaDetails);

	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	int length = Env->GetArrayLength(javaDetailsArray);

	for (int i = 0; i < length; i++)
	{
		jobject javaDetail = Env->GetObjectArrayElement(javaDetailsArray, i);

		FSubscriptionOfferDetails Details;

		jobject javaTags = MGMethodCallUtils::CallStaticObjectMethod(UMGAndroidBillingLibrary::BillingLibraryClassName, "getOfferTags", 
			"(Lcom/android/billingclient/api/ProductDetails$SubscriptionOfferDetails;)[Ljava/lang/String;", javaDetail);
		Details.OfferTags = MGJavaConvertor::ConvertToStringArray(static_cast<jobjectArray>(javaTags));

		Details.OfferToken = MGMethodCallUtils::CallStringMethod(javaDetail, "getOfferToken", "()Ljava/lang/String;");

		TArray<FSubscriptionPricingPhase> PricingPhases;

		jobject javaPhases = MGMethodCallUtils::CallStaticObjectMethod(UMGAndroidBillingLibrary::BillingLibraryClassName, "getPricingPhases", 
			"(Lcom/android/billingclient/api/ProductDetails$SubscriptionOfferDetails;)[Lcom/android/billingclient/api/ProductDetails$PricingPhase;", javaDetail);
		jobjectArray javaPhasesArray = static_cast<jobjectArray>(javaPhases);

		int phasesLength = Env->GetArrayLength(javaPhasesArray);

		for (int j = 0; j < phasesLength; j++)
		{
			jobject javaPhase = Env->GetObjectArrayElement(javaPhasesArray, j);

			FSubscriptionPricingPhase Phase;

			Phase.BillingCycleCount = MGMethodCallUtils::CallIntMethod(javaPhase, "getBillingCycleCount", "()I");
			Phase.RecurrenceMode = MGMethodCallUtils::CallIntMethod(javaPhase, "getRecurrenceMode", "()I");
			Phase.PriceAmountMicros = MGMethodCallUtils::CallLongMethod(javaPhase, "getPriceAmountMicros", "()J");
			Phase.BillingPeriod = MGMethodCallUtils::CallStringMethod(javaPhase, "getBillingPeriod", "()Ljava/lang/String;");
			Phase.FormattedPrice = MGMethodCallUtils::CallStringMethod(javaPhase, "getFormattedPrice", "()Ljava/lang/String;");
			Phase.PriceCurrencyCode = MGMethodCallUtils::CallStringMethod(javaPhase, "getPriceCurrencyCode", "()Ljava/lang/String;");

			PricingPhases.Add(Phase);
		}

		Details.PricingPhases = PricingPhases;

		Result.Add(Details);
	}
#endif

	return Result;
}
