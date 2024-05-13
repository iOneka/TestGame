// Copyright (c) 2021 Nineva Studios

#include "MGAndroidBillingFlowParameters.h"
#include "MGAndroidSkuDetails.h"
#include "MGAndroidProductDetails.h"

#include "MonetizationGoodiesLog.h"

#include "MGAndroidBillingLibrary.h"

#if PLATFORM_ANDROID
#include "Android/Utils/MGJavaConvertor.h"
#include "Android/Utils/MGMethodCallUtils.h"
#endif

UMGAndroidBillingFlowParameters* UMGAndroidBillingFlowParameters::SetObfuscatedAccountId(const FString& ObfuscatedAccountId)
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidBillingFlowParameters::SetObfuscatedAccountId"));

	if (!IsNativeObjectValid())
	{
		return this;
	}

#if PLATFORM_ANDROID
	MGMethodCallUtils::CallObjectMethod(JavaObject, "setObfuscatedAccountId",
		"(Ljava/lang/String;)Lcom/android/billingclient/api/BillingFlowParams$Builder;", MGJavaConvertor::GetJavaString(ObfuscatedAccountId));
#endif

	return this;
}

UMGAndroidBillingFlowParameters* UMGAndroidBillingFlowParameters::SetObfuscatedProfileId(const FString& ObfuscatedProfileId)
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidBillingFlowParameters::SetObfuscatedProfileId"));

	if (!IsNativeObjectValid())
	{
		return this;
	}

#if PLATFORM_ANDROID
	MGMethodCallUtils::CallObjectMethod(JavaObject, "setObfuscatedProfileId",
		"(Ljava/lang/String;)Lcom/android/billingclient/api/BillingFlowParams$Builder;", MGJavaConvertor::GetJavaString(ObfuscatedProfileId));
#endif

	return this;
}

UMGAndroidBillingFlowParameters* UMGAndroidBillingFlowParameters::SetSkuDetails(UMGAndroidSkuDetails* SkuDetails)
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidBillingFlowParameters::SetSkuDetails"));

	if (!IsNativeObjectValid())
	{
		return this;
	}

#if PLATFORM_ANDROID
	MGMethodCallUtils::CallObjectMethod(JavaObject, "setSkuDetails", "(Lcom/android/billingclient/api/SkuDetails;)Lcom/android/billingclient/api/BillingFlowParams$Builder;", SkuDetails->JavaObject);
#endif

	return this;
}

UMGAndroidBillingFlowParameters* UMGAndroidBillingFlowParameters::SetSubscriptionUpdateParams(const FString& OldSkuPurchaseToken, EMGProrationMode ProrationMode)
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidBillingFlowParameters::SetSubscriptionUpdateParams"));

	if (!IsNativeObjectValid())
	{
		return this;
	}

#if PLATFORM_ANDROID
	MGMethodCallUtils::CallStaticVoidMethod(UMGAndroidBillingLibrary::BillingLibraryClassName, "setSubscriptionUpdateParams",
		"(Lcom/android/billingclient/api/BillingFlowParams$Builder;Ljava/lang/String;I)V", JavaObject, MGJavaConvertor::GetJavaString(OldSkuPurchaseToken), (int) ProrationMode);
#endif

	return this;
}

UMGAndroidBillingFlowParameters* UMGAndroidBillingFlowParameters::SetProductDetailsParams(UMGAndroidProductDetails* ProductDetails, const FString& OfferToken)
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidBillingFlowParameters::SetProductDetailsParams"));

	if (!IsNativeObjectValid())
	{
		return this;
	}

#if PLATFORM_ANDROID
	MGMethodCallUtils::CallStaticVoidMethod(UMGAndroidBillingLibrary::BillingLibraryClassName, "setProductDetailsParams",
		"(Lcom/android/billingclient/api/BillingFlowParams$Builder;Lcom/android/billingclient/api/ProductDetails;Ljava/lang/String;)V", 
		JavaObject, ProductDetails->JavaObject, MGJavaConvertor::GetJavaString(OfferToken));
#endif

	return this;
}