// Copyright (c) 2021 Nineva Studios

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "MGAndroidBillingLibrary.generated.h"

class UMGAndroidProductDetails;
class UMGAndroidBillingClient;
class UMGAndroidBillingResult;
class UMGAndroidPurchase;
class UMGAndroidPurchaseHistoryRecord;
class UMGAndroidSkuDetails;
class UMGAndroidBillingFlowParameters;

DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnPurchasesUpdated, UMGAndroidBillingResult*, Result, const TArray<UMGAndroidPurchase*>&, Purchases);
DECLARE_DYNAMIC_DELEGATE(FAndroidBillingVoidDelegate);
DECLARE_DYNAMIC_DELEGATE_OneParam(FAndroidBillingResultDelegate, UMGAndroidBillingResult*, Result);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FAndroidConsumeResultDelegate, UMGAndroidBillingResult*, Result, const FString&, Token);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FAndroidPurchaseHistoryDelegate, UMGAndroidBillingResult*, Result, const TArray<UMGAndroidPurchaseHistoryRecord*>&, Records);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FAndroidPurchasesDelegate, UMGAndroidBillingResult*, Result, const TArray<UMGAndroidPurchase*>&, Purchases);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FAndroidProductDetailsDelegate, UMGAndroidBillingResult*, Result, const TArray<UMGAndroidProductDetails*>&, ProductDetails);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FAndroidSkuDetailsDelegate, UMGAndroidBillingResult*, Result, const TArray<UMGAndroidSkuDetails*>&, Details);

/*
* Helper library for common constants and object creation.
*/
UCLASS()
class MONETIZATIONGOODIES_API UMGAndroidBillingLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/*
	* Create an instance of the billing client used to work with SDK.
	* 
	* @param OnPurchasesUpdated - callback invoked every time user updates or receives a new Purchase.
	*/
	UFUNCTION(BlueprintCallable, Category = "Monetization Goodies|Android", meta = (AutoCreateRefTerm = "OnPurchasesUpdated"))
	static UMGAndroidBillingClient* CreateAndroidBillingClient(const FOnPurchasesUpdated& OnPurchasesUpdated);

	/*
	* Create an instance of the billing flow parameters to pass to the launch purchase function.
	*/
	UFUNCTION(BlueprintCallable, Category = "Monetization Goodies|Android")
	static UMGAndroidBillingFlowParameters* CreateAndroidBillingFlowParameters();

	static FOnPurchasesUpdated OnPurchasesUpdatedDelegate;

	static FAndroidBillingResultDelegate OnAcknowledgePurchaseCompleted;

	static FAndroidConsumeResultDelegate OnConsumeCompleted;

	static FAndroidBillingResultDelegate OnLaunchBillingFlowStarted;

	static FAndroidBillingResultDelegate OnLaunchPriceChangeConfirmationFlowCompleted;

	static FAndroidPurchaseHistoryDelegate OnQueryPurchaseHistoryCompleted;

	static FAndroidPurchasesDelegate OnQueryPurchasesCompleted;
	static FAndroidProductDetailsDelegate OnQueryProductDetailsCompleted;

	static FAndroidSkuDetailsDelegate OnQuerySkuDetailsCompleted;

	static FAndroidBillingResultDelegate OnStartConnectionBillingSetupFinished;
	static FAndroidBillingVoidDelegate OnStartConnectionBillingServiceDisconnected;

	static const ANSICHAR* BillingLibraryClassName;
};