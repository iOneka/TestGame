// Copyright (c) 2021 Nineva Studios

#include "MGIosStoreFront.h"

#include "MonetizationGoodiesLog.h"

FString UMGIosStoreFront::GetCountryCode()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosStoreFront::GetCountryCode"));
	
	FString Result;
	
	if (!IsNativeObjectValid())
	{
		return Result;
	}
	
#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	Result = ((SKStorefront*) IOSObject).countryCode;
#endif
	
	return Result;
}

FString UMGIosStoreFront::GetIdentifier()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosStoreFront::GetIdentifier"));
	
	FString Result;
	
	if (!IsNativeObjectValid())
	{
		return Result;
	}
	
#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	Result = ((SKStorefront*) IOSObject).identifier;
#endif
	
	return Result;
}
