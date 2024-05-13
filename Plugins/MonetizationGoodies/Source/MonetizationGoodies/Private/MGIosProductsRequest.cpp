// Copyright (c) 2021 Nineva Studios

#include "MGIosProductsRequest.h"

#include "MGIosProduct.h"

#include "Async/Async.h"

#include "MonetizationGoodiesLog.h"

UMGIosProductsRequest* UMGIosProductsRequest::CreateProductsRequest(const TArray<FString>& ProductIdentifiers, const FMGProductsRequestDelegate& OnCompleted, const FMGStringDelegate& OnError)
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosProductsRequest::CreateProductsRequest"));
	
	UMGIosProductsRequest* Result = NewObject<UMGIosProductsRequest>();
	
	Result->OnCompletedDelegate = OnCompleted;
	Result->OnErrorDelegate = OnError;
	
#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	Result->RequestDelegate = [MGProductsRequestDelegate new];
	Result->RequestDelegate.onResponseReceived = ^(SKProductsResponse* response) {
		SKProductsResponse* responseRetained = [response retain];
		AsyncTask(ENamedThreads::GameThread, [=]() {
			TArray<UMGIosProduct*> Products;
			
			for (id product in responseRetained.products) {
				UMGIosProduct* Product = NewObject<UMGIosProduct>();
				Product->Init(product);
				Products.Add(Product);
			}
			
			TArray<FString> InvalidIdentifiers;
			
			for (NSString* identifier in responseRetained.invalidProductIdentifiers) {
				FString Identifier = identifier;
				InvalidIdentifiers.Add(Identifier);
			}

			[responseRetained release];
			Result->OnCompletedDelegate.ExecuteIfBound(Products, InvalidIdentifiers);
		});
	};
	
	Result->RequestDelegate.onError = ^(NSError* error) {
		FString Error = error.localizedDescription;
		
		AsyncTask(ENamedThreads::GameThread, [=]() {
			Result->OnErrorDelegate.ExecuteIfBound(Error);
		});
	};
	
	NSMutableSet* identifiers = [NSMutableSet set];
	
	for (const auto Identifier : ProductIdentifiers) {
		[identifiers addObject:Identifier.GetNSString()];
	}
	
	SKProductsRequest* request = [[SKProductsRequest alloc] initWithProductIdentifiers:identifiers];
	request.delegate = Result->RequestDelegate;
	
	Result->Init(request);
#endif
	
	return Result;
}

void UMGIosProductsRequest::Start()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosProductsRequest::Start"));
	
	if (!IsNativeObjectValid())
	{
		return;
	}
	
#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	[((SKProductsRequest*) IOSObject) start];
#endif
}

void UMGIosProductsRequest::Cancel()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosProductsRequest::Cancel"));
	
	if (!IsNativeObjectValid())
	{
		return;
	}
	
#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	[((SKProductsRequest*) IOSObject) cancel];
#endif
}
