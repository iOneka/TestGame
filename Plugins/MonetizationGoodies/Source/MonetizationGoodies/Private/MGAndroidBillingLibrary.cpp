// Copyright (c) 2021 Nineva Studios

#include "MGAndroidBillingLibrary.h"

#include "MonetizationGoodiesLog.h"

#include "MGAndroidBillingClient.h"
#include "MGAndroidBillingFlowParameters.h"
#include "MGAndroidBillingResult.h"
#include "MGAndroidPurchase.h"
#include "MGAndroidPurchaseHistoryRecord.h"
#include "MGAndroidSkuDetails.h"
#include "MGAndroidProductDetails.h"

#if PLATFORM_ANDROID
#include "Android/Utils/MGJavaConvertor.h"
#include "Android/Utils/MGMethodCallUtils.h"
#include "Async/Async.h"
#endif


#if PLATFORM_ANDROID

/**
 * Create global object reference and automatically deletes when out of scope
 */
template <typename T>
class FGlobalJavaObject
{
public:
	FGlobalJavaObject(const T& InObjRef) 
		: ObjRef(InObjRef)
	{
		check(ObjRef);

		JNIEnv* Env = FAndroidApplication::GetJavaEnv();
		check(Env);

		ObjRef = (T)Env->NewGlobalRef(ObjRef);
	}

	FGlobalJavaObject(FGlobalJavaObject&& Other) 
		: ObjRef(Other.ObjRef)
	{
		Other.ObjRef = nullptr;
	}

	FGlobalJavaObject(const FGlobalJavaObject& Other) = delete;
	FGlobalJavaObject& operator=(const FGlobalJavaObject& Other) = delete;

	~FGlobalJavaObject()
	{
		if (*this)
		{
			JNIEnv* Env = FAndroidApplication::GetJavaEnv();
			Env->DeleteGlobalRef(ObjRef);
		}
		ObjRef = nullptr;
	}

	operator bool() const
	{
		JNIEnv* Env = FAndroidApplication::GetJavaEnv();
		if (!Env || !ObjRef || Env->IsSameObject(ObjRef, NULL))
		{
			return false;
		}

		return true;
	}

	// Returns the underlying JNI pointer
	T operator*() const { return ObjRef; }	

private:
	T ObjRef = nullptr;
};

/**
 Helper function that allows template deduction on the java object type, for example:
 auto ScopeObject = NewScopedJavaObject(Env, JavaString);
 instead of FScopedJavaObject<jstring> ScopeObject(Env, JavaString);
 */
template <typename T>
FGlobalJavaObject<T> NewGlobalJavaObject(const T& InObjRef)
{
	return FGlobalJavaObject<T>(InObjRef);
}

#endif

const ANSICHAR* UMGAndroidBillingLibrary::BillingLibraryClassName = "com/ninevastudios/monetization/MGBilling";

FOnPurchasesUpdated UMGAndroidBillingLibrary::OnPurchasesUpdatedDelegate;

FAndroidBillingResultDelegate UMGAndroidBillingLibrary::OnAcknowledgePurchaseCompleted;

FAndroidConsumeResultDelegate UMGAndroidBillingLibrary::OnConsumeCompleted;

FAndroidBillingResultDelegate UMGAndroidBillingLibrary::OnLaunchBillingFlowStarted;

FAndroidBillingResultDelegate UMGAndroidBillingLibrary::OnLaunchPriceChangeConfirmationFlowCompleted;

FAndroidPurchaseHistoryDelegate UMGAndroidBillingLibrary::OnQueryPurchaseHistoryCompleted;

FAndroidPurchasesDelegate UMGAndroidBillingLibrary::OnQueryPurchasesCompleted;
FAndroidProductDetailsDelegate UMGAndroidBillingLibrary::OnQueryProductDetailsCompleted;

FAndroidSkuDetailsDelegate UMGAndroidBillingLibrary::OnQuerySkuDetailsCompleted;

FAndroidBillingResultDelegate UMGAndroidBillingLibrary::OnStartConnectionBillingSetupFinished;
FAndroidBillingVoidDelegate UMGAndroidBillingLibrary:: OnStartConnectionBillingServiceDisconnected;

UMGAndroidBillingClient* UMGAndroidBillingLibrary::CreateAndroidBillingClient(const FOnPurchasesUpdated& OnPurchasesUpdated)
{
	OnPurchasesUpdatedDelegate = OnPurchasesUpdated;

	UMGAndroidBillingClient* Result = NewObject<UMGAndroidBillingClient>();

#if PLATFORM_ANDROID
	jobject javaClient = MGMethodCallUtils::CallStaticObjectMethod(BillingLibraryClassName, "createBillingClient", 
		"(Landroid/app/Activity;)Lcom/android/billingclient/api/BillingClient;", FJavaWrapper::GameActivityThis);
	Result->Init(javaClient);
#endif

	return Result;
}

UMGAndroidBillingFlowParameters* UMGAndroidBillingLibrary::CreateAndroidBillingFlowParameters()
{
	UMGAndroidBillingFlowParameters* Result = NewObject<UMGAndroidBillingFlowParameters>();

#if PLATFORM_ANDROID
	jobject javaParams = MGMethodCallUtils::CallStaticObjectMethod(BillingLibraryClassName, "createBillingFlowParamsBuilder",
		"()Lcom/android/billingclient/api/BillingFlowParams$Builder;");
	Result->Init(javaParams);
#endif

	return Result;
}

#if PLATFORM_ANDROID
JNI_METHOD void Java_com_ninevastudios_monetization_MGBilling_OnPurchasesUpdated(JNIEnv* env, jclass clazz, jobject billingResult, jobjectArray purchasesArray)
{
	AsyncTask(ENamedThreads::GameThread, [Result = NewGlobalJavaObject(billingResult), Array = NewGlobalJavaObject(purchasesArray)]()
	{
		UMGAndroidBillingResult* BillingResult = NewObject<UMGAndroidBillingResult>();
		BillingResult->Init(*Result);

		TArray<UMGAndroidPurchase*> Purchases;

		JNIEnv* Env = FAndroidApplication::GetJavaEnv();

		int length = Env->GetArrayLength(*Array);

		for (int i = 0; i < length; i++)
		{
			jobject purchase = Env->GetObjectArrayElement(*Array, i);

			UMGAndroidPurchase* Purchase = NewObject<UMGAndroidPurchase>();
			Purchase->Init(purchase);

			Purchases.Add(Purchase);
		}

		UMGAndroidBillingLibrary::OnPurchasesUpdatedDelegate.ExecuteIfBound(BillingResult, Purchases);
	});
}

JNI_METHOD void Java_com_ninevastudios_monetization_MGBilling_OnBillingSetupFinished(JNIEnv* env, jclass clazz, jobject billingResult)
{
	AsyncTask(ENamedThreads::GameThread, [Result = NewGlobalJavaObject(billingResult)]()
	{
		UMGAndroidBillingResult* BillingResult = NewObject<UMGAndroidBillingResult>();
		BillingResult->Init(*Result);

		UMGAndroidBillingLibrary::OnStartConnectionBillingSetupFinished.ExecuteIfBound(BillingResult);
	});
}

JNI_METHOD void Java_com_ninevastudios_monetization_MGBilling_OnBillingServiceDisconnected(JNIEnv* env, jclass clazz)
{
	AsyncTask(ENamedThreads::GameThread, [=]() 
	{
		UMGAndroidBillingLibrary::OnStartConnectionBillingServiceDisconnected.ExecuteIfBound();
	});
}

JNI_METHOD void Java_com_ninevastudios_monetization_MGBilling_OnAcknowledgePurchaseResponse(JNIEnv* env, jclass clazz, jobject billingResult)
{
	AsyncTask(ENamedThreads::GameThread, [Result = NewGlobalJavaObject(billingResult)]()
	{
		UMGAndroidBillingResult* BillingResult = NewObject<UMGAndroidBillingResult>();
		BillingResult->Init(*Result);

		UMGAndroidBillingLibrary::OnAcknowledgePurchaseCompleted.ExecuteIfBound(BillingResult);
	});
}

JNI_METHOD void Java_com_ninevastudios_monetization_MGBilling_OnConsumeResponse(JNIEnv* env, jclass clazz, jobject billingResult, jstring purchaseToken)
{
	FString PurchaseToken = MGJavaConvertor::FromJavaString(purchaseToken);
	AsyncTask(ENamedThreads::GameThread, [PurchaseToken, Result = NewGlobalJavaObject(billingResult)]()
	{
		UMGAndroidBillingResult* BillingResult = NewObject<UMGAndroidBillingResult>();
		BillingResult->Init(*Result);

		UMGAndroidBillingLibrary::OnConsumeCompleted.ExecuteIfBound(BillingResult, PurchaseToken);
	});
}

JNI_METHOD void Java_com_ninevastudios_monetization_MGBilling_OnLaunchBillingFlowStarted(JNIEnv* env, jclass clazz, jobject billingResult)
{
	AsyncTask(ENamedThreads::GameThread, [Result = NewGlobalJavaObject(billingResult)]()
	{
		UMGAndroidBillingResult* BillingResult = NewObject<UMGAndroidBillingResult>();
		BillingResult->Init(*Result);

		UMGAndroidBillingLibrary::OnLaunchBillingFlowStarted.ExecuteIfBound(BillingResult);
	});
}

JNI_METHOD void Java_com_ninevastudios_monetization_MGBilling_OnPriceChangeConfirmationResult(JNIEnv* env, jclass clazz, jobject billingResult)
{	
	AsyncTask(ENamedThreads::GameThread, [Result = NewGlobalJavaObject(billingResult)]()
	{
		UMGAndroidBillingResult* BillingResult = NewObject<UMGAndroidBillingResult>();
		BillingResult->Init(*Result);

		UMGAndroidBillingLibrary::OnLaunchPriceChangeConfirmationFlowCompleted.ExecuteIfBound(BillingResult);
	});
}

JNI_METHOD void Java_com_ninevastudios_monetization_MGBilling_OnPurchaseHistoryResponse(JNIEnv* env, jclass clazz, jobject billingResult, jobjectArray recordsArray)
{
	AsyncTask(ENamedThreads::GameThread, [Result = NewGlobalJavaObject(billingResult), Array = NewGlobalJavaObject(recordsArray)]()
	{
		UMGAndroidBillingResult* BillingResult = NewObject<UMGAndroidBillingResult>();
		BillingResult->Init(*Result);

		TArray<UMGAndroidPurchaseHistoryRecord*> Records;

		JNIEnv* Env = FAndroidApplication::GetJavaEnv();

		int length = Env->GetArrayLength(*Array);

		for (int i = 0; i < length; i++)
		{
			jobject record = Env->GetObjectArrayElement(*Array, i);

			UMGAndroidPurchaseHistoryRecord* Record = NewObject<UMGAndroidPurchaseHistoryRecord>();
			Record->Init(record);

			Records.Add(Record);
		}

		UMGAndroidBillingLibrary::OnQueryPurchaseHistoryCompleted.ExecuteIfBound(BillingResult, Records);
	});
}

JNI_METHOD void Java_com_ninevastudios_monetization_MGBilling_OnQueryPurchasesResponse(JNIEnv* env, jclass clazz, jobject billingResult, jobjectArray purchases)
{
	AsyncTask(ENamedThreads::GameThread, [Result = NewGlobalJavaObject(billingResult), Array = NewGlobalJavaObject(purchases)]()
	{
		UMGAndroidBillingResult* BillingResult = NewObject<UMGAndroidBillingResult>();
		BillingResult->Init(*Result);

		TArray<UMGAndroidPurchase*> Purchases;

		JNIEnv* Env = FAndroidApplication::GetJavaEnv();

		int length = Env->GetArrayLength(*Array);

		for (int i = 0; i < length; i++)
		{
			jobject purchase = Env->GetObjectArrayElement(*Array, i);

			UMGAndroidPurchase* Purchase = NewObject<UMGAndroidPurchase>();
			Purchase->Init(purchase);

			Purchases.Add(Purchase);
		}

		UMGAndroidBillingLibrary::OnQueryPurchasesCompleted.ExecuteIfBound(BillingResult, Purchases);
	});
}

JNI_METHOD void Java_com_ninevastudios_monetization_MGBilling_OnSkuDetailsResponse(JNIEnv* env, jclass clazz, jobject billingResult, jobjectArray details)
{
	AsyncTask(ENamedThreads::GameThread, [Result = NewGlobalJavaObject(billingResult), Array = NewGlobalJavaObject(details)]()
	{
		UMGAndroidBillingResult* BillingResult = NewObject<UMGAndroidBillingResult>();
		BillingResult->Init(*Result);

		TArray<UMGAndroidSkuDetails*> DetailsArray;

		JNIEnv* Env = FAndroidApplication::GetJavaEnv();

		int length = Env->GetArrayLength(*Array);

		for (int i = 0; i < length; i++)
		{
			jobject detail = Env->GetObjectArrayElement(*Array, i);

			UMGAndroidSkuDetails* Details = NewObject<UMGAndroidSkuDetails>();
			Details->Init(detail);

			DetailsArray.Add(Details);
		}

		UMGAndroidBillingLibrary::OnQuerySkuDetailsCompleted.ExecuteIfBound(BillingResult, DetailsArray);
	});
}

JNI_METHOD void Java_com_ninevastudios_monetization_MGBilling_OnProductDetailsResponse(JNIEnv* env, jclass clazz, jobject billingResult, jobjectArray details)
{
	AsyncTask(ENamedThreads::GameThread, [Result = NewGlobalJavaObject(billingResult), Array = NewGlobalJavaObject(details)]()
	{
		UMGAndroidBillingResult* BillingResult = NewObject<UMGAndroidBillingResult>();
		BillingResult->Init(*Result);

		TArray<UMGAndroidProductDetails*> DetailsArray;

		JNIEnv* Env = FAndroidApplication::GetJavaEnv();

		int length = Env->GetArrayLength(*Array);

		for (int i = 0; i < length; i++)
		{
			jobject detail = Env->GetObjectArrayElement(*Array, i);

			UMGAndroidProductDetails* Details = NewObject<UMGAndroidProductDetails>();
			Details->Init(detail);

			DetailsArray.Add(Details);
		}

		UMGAndroidBillingLibrary::OnQueryProductDetailsCompleted.ExecuteIfBound(BillingResult, DetailsArray);
	});
}
#endif