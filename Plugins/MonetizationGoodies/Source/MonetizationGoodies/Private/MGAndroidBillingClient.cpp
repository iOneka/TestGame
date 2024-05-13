// Copyright (c) 2021 Nineva Studios

#include "MGAndroidBillingClient.h"

#include "Async/Async.h"

#include "MonetizationGoodiesLog.h"

#include "MGAndroidBillingResult.h"
#include "MGAndroidBillingFlowParameters.h"
#include "MGAndroidSkuDetails.h"

#if PLATFORM_ANDROID
#include "Android/Utils/MGJavaConvertor.h"
#include "Android/Utils/MGMethodCallUtils.h"
#endif

TMap<EMGFeature, FString> PlayStoreFeatures = {
	{EMGFeature::InAppItemsOnVr, TEXT("inAppItemsOnVr")},
	{EMGFeature::PriceChangeConfirmation, TEXT("priceChangeConfirmation")},
	{EMGFeature::Subscriptions, TEXT("subscriptions")},
	{EMGFeature::SubscriptionsOnVr, TEXT("subscriptionsOnVr")},
	{EMGFeature::SubscriptionsUpdate, TEXT("subscriptionsUpdate")},
};

TMap<EMGSkuType, FString> PlayStorePurchaseTypes = {
	{EMGSkuType::Purchase, TEXT("inapp")},
	{EMGSkuType::Subscription, TEXT("subs")},
};

void UMGAndroidBillingClient::AcknowledgePurchase(const FString& Token, const FAndroidBillingResultDelegate& OnCompleted)
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidBillingClient::AcknowledgePurchase"));

	if (!IsNativeObjectValid())
	{
		return;
	}

	UMGAndroidBillingLibrary::OnAcknowledgePurchaseCompleted = OnCompleted;

#if PLATFORM_ANDROID
	MGMethodCallUtils::CallStaticVoidMethod(UMGAndroidBillingLibrary::BillingLibraryClassName, "acknowledgePurchase", 
		"(Lcom/android/billingclient/api/BillingClient;Ljava/lang/String;)V", JavaObject, MGJavaConvertor::GetJavaString(Token));
#endif
}

void UMGAndroidBillingClient::Consume(const FString& Token, const FAndroidConsumeResultDelegate& OnCompleted)
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidBillingClient::Consume"));

	if (!IsNativeObjectValid())
	{
		return;
	}

	UMGAndroidBillingLibrary::OnConsumeCompleted = OnCompleted;

#if PLATFORM_ANDROID
	MGMethodCallUtils::CallStaticVoidMethod(UMGAndroidBillingLibrary::BillingLibraryClassName, "consumeItem",
		"(Lcom/android/billingclient/api/BillingClient;Ljava/lang/String;)V", JavaObject, MGJavaConvertor::GetJavaString(Token));
#endif
}

void UMGAndroidBillingClient::EndConnection()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidBillingClient::EndConnection"));

	if (!IsNativeObjectValid())
	{
		return;
	}

#if PLATFORM_ANDROID
	MGMethodCallUtils::CallVoidMethod(JavaObject, "endConnection", "()V");
#endif
}

EMGConnectionState UMGAndroidBillingClient::GetConnectionState()
{
	EMGConnectionState Result = EMGConnectionState::Closed;

	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidBillingClient::GetConnectionState"));

	if (!IsNativeObjectValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	Result = (EMGConnectionState) MGMethodCallUtils::CallIntMethod(JavaObject, "getConnectionState", "()I");
#endif

	return Result;
}

UMGAndroidBillingResult* UMGAndroidBillingClient::IsFeatureSupported(const EMGFeature Feature)
{
	UMGAndroidBillingResult* Result = NewObject<UMGAndroidBillingResult>();

	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidBillingClient::IsFeatureSupported"));

	const FString featureStr = PlayStoreFeatures[Feature];

	if (!IsNativeObjectValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	Result->Init(MGMethodCallUtils::CallObjectMethod(JavaObject, "isFeatureSupported", "(Ljava/lang/String;)Lcom/android/billingclient/api/BillingResult;", 
		MGJavaConvertor::GetJavaString(featureStr)));
#endif

	return Result;
}

bool UMGAndroidBillingClient::IsReady()
{
	bool Result = false;

	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidBillingClient::IsReady"));

	if (!IsNativeObjectValid())
	{
		return Result;
	}

#if PLATFORM_ANDROID
	Result = MGMethodCallUtils::CallBoolMethod(JavaObject, "isReady", "()Z");
#endif

	return Result;
}

void UMGAndroidBillingClient::LaunchBillingFlow(UMGAndroidBillingFlowParameters* Parameters, const FAndroidBillingResultDelegate& OnStarted)
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidBillingClient::LaunchBillingFlow"));

	if (!IsNativeObjectValid())
	{
		return;
	}

	UMGAndroidBillingLibrary::OnLaunchBillingFlowStarted = OnStarted;

#if PLATFORM_ANDROID
	MGMethodCallUtils::CallStaticVoidMethod(UMGAndroidBillingLibrary::BillingLibraryClassName, "launchBillingFlow",
		"(Landroid/app/Activity;Lcom/android/billingclient/api/BillingClient;Lcom/android/billingclient/api/BillingFlowParams$Builder;)V", 
		FJavaWrapper::GameActivityThis, JavaObject, Parameters->JavaObject);
#endif
}

void UMGAndroidBillingClient::LaunchPriceChangeConfirmationFlow(UMGAndroidSkuDetails* Details, const FAndroidBillingResultDelegate& OnCompleted)
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidBillingClient::LaunchPriceChangeConfirmationFlow"));

	if (!IsNativeObjectValid())
	{
		return;
	}

	UMGAndroidBillingLibrary::OnLaunchPriceChangeConfirmationFlowCompleted = OnCompleted;

#if PLATFORM_ANDROID
	MGMethodCallUtils::CallStaticVoidMethod(UMGAndroidBillingLibrary::BillingLibraryClassName, "launchPriceChangeConfirmationFlow",
		"(Landroid/app/Activity;Lcom/android/billingclient/api/BillingClient;Lcom/android/billingclient/api/SkuDetails;)V",
		FJavaWrapper::GameActivityThis, JavaObject, Details->JavaObject);
#endif
}

void UMGAndroidBillingClient::QueryPurchaseHistory(EMGSkuType SkuType, const FAndroidPurchaseHistoryDelegate& OnCompleted)
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidBillingClient::QueryPurchaseHistory"));

	if (!IsNativeObjectValid())
	{
		return;
	}

	UMGAndroidBillingLibrary::OnQueryPurchaseHistoryCompleted = OnCompleted;

#if PLATFORM_ANDROID
	MGMethodCallUtils::CallStaticVoidMethod(UMGAndroidBillingLibrary::BillingLibraryClassName, "queryPurchaseHistoryAsync",
		"(Lcom/android/billingclient/api/BillingClient;Ljava/lang/String;)V", JavaObject, MGJavaConvertor::GetJavaString(PlayStorePurchaseTypes[SkuType]));
#endif
}

void UMGAndroidBillingClient::QueryPurchases(EMGSkuType SkuType, const FAndroidPurchasesDelegate& OnCompleted)
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidBillingClient::QueryPurchases"));

	if (!IsNativeObjectValid())
	{
		return;
	}

	UMGAndroidBillingLibrary::OnQueryPurchasesCompleted = OnCompleted;

#if PLATFORM_ANDROID
	MGMethodCallUtils::CallStaticVoidMethod(UMGAndroidBillingLibrary::BillingLibraryClassName, "queryPurchasesAsync",
		"(Lcom/android/billingclient/api/BillingClient;Ljava/lang/String;)V", JavaObject, MGJavaConvertor::GetJavaString(PlayStorePurchaseTypes[SkuType]));
#endif
}

void UMGAndroidBillingClient::QuerySkuDetails(EMGSkuType SkuType, const TArray<FString>& Skus, const FAndroidSkuDetailsDelegate& OnCompleted)
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidBillingClient::QuerySkuDetails"));

	if (!IsNativeObjectValid())
	{
		return;
	}

	UMGAndroidBillingLibrary::OnQuerySkuDetailsCompleted = OnCompleted;

#if PLATFORM_ANDROID
	MGMethodCallUtils::CallStaticVoidMethod(UMGAndroidBillingLibrary::BillingLibraryClassName, "querySkuDetailsAsync",
		"(Lcom/android/billingclient/api/BillingClient;[Ljava/lang/String;Ljava/lang/String;)V", 
		JavaObject, MGJavaConvertor::ToJavaStringArray(Skus), MGJavaConvertor::GetJavaString(PlayStorePurchaseTypes[SkuType]));
#endif
}

void UMGAndroidBillingClient::QueryProductDetails(EMGSkuType SkuType, const TArray<FString>& ProductIds, const FAndroidProductDetailsDelegate& OnCompleted)
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidBillingClient::QueryProductDetails"));

	if (!IsNativeObjectValid())
	{
		return;
	}

	UMGAndroidBillingLibrary::OnQueryProductDetailsCompleted = OnCompleted;

#if PLATFORM_ANDROID
	MGMethodCallUtils::CallStaticVoidMethod(UMGAndroidBillingLibrary::BillingLibraryClassName, "queryProductDetailsAsync",
		"(Lcom/android/billingclient/api/BillingClient;[Ljava/lang/String;Ljava/lang/String;)V",
		JavaObject, MGJavaConvertor::ToJavaStringArray(ProductIds), MGJavaConvertor::GetJavaString(PlayStorePurchaseTypes[SkuType]));
#endif
}

void UMGAndroidBillingClient::StartConnection(const FAndroidBillingResultDelegate& OnBillingSetupFinished, const FAndroidBillingVoidDelegate& OnBillingServiceDisconnected)
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGAndroidBillingClient::QuerySkuDetails"));

	if (!IsNativeObjectValid())
	{
		return;
	}

	UMGAndroidBillingLibrary::OnStartConnectionBillingSetupFinished = OnBillingSetupFinished;
	UMGAndroidBillingLibrary::OnStartConnectionBillingServiceDisconnected = OnBillingServiceDisconnected;

#if PLATFORM_ANDROID
	MGMethodCallUtils::CallStaticVoidMethod(UMGAndroidBillingLibrary::BillingLibraryClassName, "startConnection",
		"(Lcom/android/billingclient/api/BillingClient;)V", JavaObject);
#endif
}
