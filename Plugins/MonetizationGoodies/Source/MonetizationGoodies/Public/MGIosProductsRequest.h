// Copyright (c) 2021 Nineva Studios

#pragma once

#include "MGIosWrapperObject.h"

#include "MGIosTransactionObserver.h"

#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
#include "IOS/MGProductsRequestDelegate.h"
#endif

#include "MGIosProductsRequest.generated.h"

class UMGIosProduct;

DECLARE_DYNAMIC_DELEGATE_TwoParams(FMGProductsRequestDelegate, const TArray<UMGIosProduct*>&, Products, const TArray<FString>&, InvalidProductIdentifiers);

/*
* An object that can retrieve localized information from the App Store about a specified list of products.
*/
UCLASS(BlueprintType)
class MONETIZATIONGOODIES_API UMGIosProductsRequest : public UMGIosWrapperObject
{
	GENERATED_BODY()

public:
	
	/*
	* Initializes the request with the set of product identifiers.
	*/
	UFUNCTION(BlueprintCallable, Category = "Monetization Goodies|IOS")
	static UMGIosProductsRequest* CreateProductsRequest(const TArray<FString>& ProductIdentifiers, const FMGProductsRequestDelegate& OnCompleted, const FMGStringDelegate& OnError);
	
	/*
	* Sends the request to the Apple App Store.
	*/
	UFUNCTION(BlueprintCallable, Category = "Monetization Goodies|IOS")
	void Start();
	
	/*
	* Cancels a previously started request.
	*/
	UFUNCTION(BlueprintCallable, Category = "Monetization Goodies|IOS")
	void Cancel();
	
private:
	
#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	MGProductsRequestDelegate* RequestDelegate;
#endif
	
	FMGProductsRequestDelegate OnCompletedDelegate;
	FMGStringDelegate OnErrorDelegate;
};
