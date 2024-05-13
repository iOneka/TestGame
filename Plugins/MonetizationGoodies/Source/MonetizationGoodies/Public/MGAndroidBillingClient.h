// Copyright (c) 2021 Nineva Studios

#pragma once

#include "MGAndroidWrapperObject.h"

#include "MGAndroidBillingLibrary.h"

#include "MGAndroidBillingClient.generated.h"

class UMGAndroidBillingResult;
class UMGAndroidBillingFlowParameters;
class UMGAndroidSkuDetails;
class UMGAndroidProductDetails;

/*
* Connection state of billing client.
*/
UENUM(BlueprintType)
enum class EMGConnectionState : uint8
{
	/*
	* This client was not yet connected to billing service or was already closed.
	*/
	Disconnected = 0,
	/*
	* This client is currently in process of connecting to billing service.
	*/
	Connecting = 1,
	/*
	* This client is currently connected to billing service.
	*/
	Connected = 2,
	/*
	* This client was already closed and shouldn't be used again.
	*/
	Closed = 3
};

/*
* Feature/capability that can be supported by Play Store.
*/
UENUM(BlueprintType)
enum class EMGFeature : uint8
{
	/*
	* Purchase/query for in-app items on VR.
	*/
	InAppItemsOnVr = 0,
	/*
	* Launch a price change confirmation flow.
	*/
	PriceChangeConfirmation = 1,
	/*
	* Purchase/query for subscriptions.
	*/
	Subscriptions = 2,
	/*
	* Purchase/query for subscriptions on VR.
	*/
	SubscriptionsOnVr = 3,
	/*
	* Subscriptions update/replace.
	*/
	SubscriptionsUpdate = 4,
};

/*
* Type the SKU.
*/
UENUM(BlueprintType)
enum class EMGSkuType : uint8
{
	/*
	* Purchase.
	*/
	Purchase = 0,
	/*
	* Subscription.
	*/
	Subscription = 1,
};

/*
* Main interface for communication between the library and user application code.
* 
* Reference: https://developer.android.com/reference/com/android/billingclient/api/BillingClient
*/
UCLASS(BlueprintType)
class MONETIZATIONGOODIES_API UMGAndroidBillingClient : public UMGAndroidWrapperObject
{
	GENERATED_BODY()

public:
	/*
	* Acknowledges in-app purchases.
	* Developers are required to acknowledge that they have granted entitlement for all in-app purchases for their application.
	* 
	* @param Token - purchase token.
	* @param OnCompleted - callback with the result of the acknowledge operation.
	*/
	UFUNCTION(BlueprintCallable, Category = "Monetization Goodies|Android", meta = (AutoCreateRefTerm = "OnCompleted"))
	void AcknowledgePurchase(const FString& Token, const FAndroidBillingResultDelegate& OnCompleted);

	/*
	* Consumes a given in-app product. Consuming can only be done on an item that's owned, and as a result of consumption, the user will no longer own it.
	*
	* @param Token - purchase token.
	* @param OnCompleted - callback with the result of the consume operation.
	*/
	UFUNCTION(BlueprintCallable, Category = "Monetization Goodies|Android", meta = (AutoCreateRefTerm = "OnCompleted"))
	void Consume(const FString& Token, const FAndroidConsumeResultDelegate& OnCompleted);

	/*
	* Closes the connection and releases all held resources such as service connections. 
	* Call this method once you are done with this BillingClient reference.
	*/
	UFUNCTION(BlueprintCallable, Category = "Monetization Goodies|Android")
	void EndConnection();

	/*
	* Get the current billing client connection state.
	* 
	* @return EMGConnectionState corresponding to the current client connection state.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|Android")
	EMGConnectionState GetConnectionState();

	/*
	* Checks if the specified feature or capability is supported by the Play Store.
	*
	* @param Feature - Play Store feature to be checked.
	* 
	* @return Response code will be EMGAndroidBillingResponseCode::OK if the feature is supported or EMGAndroidBillingResponseCode::FeatureNotSupported otherwise.
	*/
	UFUNCTION(BlueprintCallable, Category = "Monetization Goodies|Android")
	UMGAndroidBillingResult* IsFeatureSupported(const EMGFeature Feature);

	/*
	* Checks if the client is currently connected to the service, so that requests to other methods will succeed. 
	*  
	* Note: It also means that INAPP items are supported for purchasing, queries and all other actions. If you need to check support for SUBSCRIPTIONS or something different, use IsFeatureSupported(String) method.
	*
	* @return true if the client is currently connected to the service, false otherwise. 
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|Android")
	bool IsReady();

	/*
	* Initiates the billing flow for an in-app purchase or subscription.
	*
	* @param Parameters - Params specific to the launch billing flow request.
	* @param OnStarted - callback with the result of the launch billing flow operation. 
	* EMGAndroidBillingResponseCode::ItemAlreadyOwned if the user already owns the item being purchased, 
	* EMGAndroidBillingResponseCode::ItemUnavailable if the item is not available to be purchased, 
	* EMGAndroidBillingResponseCode::UserCancelled if the user dismissed the purchase flow.
	*/
	UFUNCTION(BlueprintCallable, Category = "Monetization Goodies|Android", meta = (AutoCreateRefTerm = "OnStarted"))
	void LaunchBillingFlow(UMGAndroidBillingFlowParameters* Parameters, const FAndroidBillingResultDelegate& OnStarted);

	/*
	* Initiates a flow to confirm the change of price for an item subscribed by the user.
	* When the price of a user subscribed item has changed, launch this flow to take users to a screen with price change information. User can confirm the new price or cancel the flow.
	*
	* @param Details - SKU details of the item with the changed price.
	* @param OnCompleted - callback with the result of the launch price change confirmation flow operation.
	*/
	UFUNCTION(BlueprintCallable, Category = "Monetization Goodies|Android", meta = (AutoCreateRefTerm = "OnCompleted", DeprecatedFunction))
	void LaunchPriceChangeConfirmationFlow(UMGAndroidSkuDetails* Details, const FAndroidBillingResultDelegate& OnCompleted);

	/*
	* Returns the most recent purchase made by the user for each SKU, even if that purchase is expired, canceled, or consumed.
	*
	* @param SkuType - The type of SKU.
	* @param OnCompleted - callback with the result of the query.
	*/
	UFUNCTION(BlueprintCallable, Category = "Monetization Goodies|Android", meta = (AutoCreateRefTerm = "OnCompleted"))
	void QueryPurchaseHistory(EMGSkuType SkuType, const FAndroidPurchaseHistoryDelegate& OnCompleted);

	/*
	* Returns purchases details for currently owned items bought within your app.
	* Only active subscriptions and non-consumed one-time purchases are returned. This method uses a cache of Google Play Store app without initiating a network request.
	*
	* @param SkuType - The type of SKU.
	* @param OnCompleted - callback with the result of the query.
	*/
	UFUNCTION(BlueprintCallable, Category = "Monetization Goodies|Android", meta = (AutoCreateRefTerm = "OnCompleted"))
	void QueryPurchases(EMGSkuType SkuType, const FAndroidPurchasesDelegate& OnCompleted);

	/*
	* Performs a network query to get SKU details and return the result asynchronously.
	*
	* @param SkuType - The type of SKU.
	* @param Skus - SKUs the details need to be fetched for.
	* @param OnCompleted - callback with the result of the query.
	*/
	UFUNCTION(BlueprintCallable, Category = "Monetization Goodies|Android", meta = (AutoCreateRefTerm = "OnCompleted", DeprecatedFunction, DeprecationMessage = "Use QueryProductDetails Instead."))
	void QuerySkuDetails(EMGSkuType SkuType, const TArray<FString>& Skus, const FAndroidSkuDetailsDelegate& OnCompleted);

	UFUNCTION(BlueprintCallable, Category = "Monetization Goodies|Android", meta = (AutoCreateRefTerm = "OnCompleted"))
	void QueryProductDetails(EMGSkuType SkuType, const TArray<FString>& ProductIds, const FAndroidProductDetailsDelegate& OnCompleted);

	/*
	* Starts up BillingClient setup process asynchronously.
	*
	* @param OnBillingSetupFinished - callback with the result of the setup operation.
	* @param OnBillingServiceDisconnected - callback invoked when the connection is lost.
	*/
	UFUNCTION(BlueprintCallable, Category = "Monetization Goodies|Android", meta = (AutoCreateRefTerm = "OnBillingSetupFinished,OnBillingServiceDisconnected"))
	void StartConnection(const FAndroidBillingResultDelegate& OnBillingSetupFinished, const FAndroidBillingVoidDelegate& OnBillingServiceDisconnected);
};
