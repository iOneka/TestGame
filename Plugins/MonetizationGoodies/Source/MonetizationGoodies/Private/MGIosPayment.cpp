// Copyright (c) 2021 Nineva Studios

#include "MGIosPayment.h"

#include "MGIosPaymentDiscount.h"
#include "MGIosProduct.h"

#include "MonetizationGoodiesLog.h"

UMGIosPayment* UMGIosPayment::CreatePayment(UMGIosProduct* Product, const FMGIosPaymentData& PaymentData)
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosPayment::CreatePayment"));
	
	UMGIosPayment* Result = NewObject<UMGIosPayment>();
	
	if (!Product->IsNativeObjectValid())
	{
		return Result;
	}
	
#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	SKMutablePayment* payment = [SKMutablePayment paymentWithProduct: (SKProduct*) Product->IOSObject];
	
	if (!PaymentData.ProductIdentifier.IsEmpty())
	{
		payment.productIdentifier = PaymentData.ProductIdentifier.GetNSString();
	}
	
	payment.quantity = PaymentData.Quantity;
	
	if (!PaymentData.ApplicationUsername.IsEmpty())
	{
		payment.applicationUsername = PaymentData.ApplicationUsername.GetNSString();
	}
	
	payment.simulatesAskToBuyInSandbox = PaymentData.SimulatesAskToBuyInSandbox;
	
	if (PaymentData.PaymentDiscount != nullptr && PaymentData.PaymentDiscount->IsNativeObjectValid())
	{
		payment.paymentDiscount = (SKPaymentDiscount*) PaymentData.PaymentDiscount->IOSObject;
	}
	
	Result->Init(payment);
#endif
	
	return Result;
}

FString UMGIosPayment::GetProductIdentifier()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosPayment::GetProductIdentifier"));
	
	FString Result;
	
	if (!IsNativeObjectValid())
	{
		return Result;
	}
	
#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	Result = ((SKPayment*) IOSObject).productIdentifier;
#endif
	
	return Result;
}

int UMGIosPayment::GetQuantity()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosPayment::GetQuantity"));
	
	int Result = 1;
	
	if (!IsNativeObjectValid())
	{
		return Result;
	}
	
#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	Result = ((SKPayment*) IOSObject).quantity;
#endif
	
	return Result;
}

FString UMGIosPayment::GetApplicationUsername()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosPayment::GetApplicationUsername"));
	
	FString Result;
	
	if (!IsNativeObjectValid())
	{
		return Result;
	}
	
#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	Result = ((SKPayment*) IOSObject).applicationUsername;
#endif
	
	return Result;
}

bool UMGIosPayment::GetSimulatesAskToBuyInSandbox()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosPayment::GetSimulatesAskToBuyInSandbox"));
	
	bool Result = false;
	
	if (!IsNativeObjectValid())
	{
		return Result;
	}
	
#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	Result = ((SKPayment*) IOSObject).simulatesAskToBuyInSandbox;
#endif
	
	return Result;
}

UMGIosPaymentDiscount* UMGIosPayment::GetPaymentDiscount()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosPayment::GetPaymentDiscount"));
	
	UMGIosPaymentDiscount* Result = NewObject<UMGIosPaymentDiscount>();
	
	if (!IsNativeObjectValid())
	{
		return Result;
	}
	
#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	Result->Init(((SKPayment*) IOSObject).paymentDiscount);
#endif
	
	return Result;
}
