// Copyright (c) 2021 Nineva Studios

#include "MGIosDownload.h"

#include "MGIosPaymentTransaction.h"

#include "MonetizationGoodiesLog.h"

int64 UMGIosDownload::GetExpectedContentLength()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosDownload::GetExpectedContentLength"));
	
	int64 Result = 0;
	
	if (!IsNativeObjectValid())
	{
		return Result;
	}
	
#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	Result = (int64) ((SKDownload*) IOSObject).expectedContentLength;
#endif
	
	return Result;
}

FString UMGIosDownload::GetContentIdentifier()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosDownload::GetContentIdentifier"));
	
	FString Result;
	
	if (!IsNativeObjectValid())
	{
		return Result;
	}
	
#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	Result = ((SKDownload*) IOSObject).contentIdentifier;
#endif
	
	return Result;
}

FString UMGIosDownload::GetContentVersion()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosDownload::GetContentVersion"));
	
	FString Result;
	
	if (!IsNativeObjectValid())
	{
		return Result;
	}
	
#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	Result = ((SKDownload*) IOSObject).contentVersion;
#endif
	
	return Result;
}

UMGIosPaymentTransaction* UMGIosDownload::GetTransaction()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosDownload::GetTransaction"));
	
	UMGIosPaymentTransaction* Result = NewObject<UMGIosPaymentTransaction>();
	
	if (!IsNativeObjectValid())
	{
		return Result;
	}
	
#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	Result->Init(((SKDownload*) IOSObject).transaction);
#endif
	
	return Result;
}

EMGDownloadState UMGIosDownload::GetState()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosDownload::GetState"));
	
	EMGDownloadState Result = EMGDownloadState::Failed;
	
	if (!IsNativeObjectValid())
	{
		return Result;
	}
	
#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	Result = (EMGDownloadState) ((SKDownload*) IOSObject).state;
#endif
	
	return Result;
}

float UMGIosDownload::GetProgress()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosDownload::GetProgress"));
	
	float Result = 0;
	
	if (!IsNativeObjectValid())
	{
		return Result;
	}
	
#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	Result = ((SKDownload*) IOSObject).progress;
#endif
	
	return Result;
}

float UMGIosDownload::GetRemainingTime()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosDownload::GetRemainingTime"));
	
	float Result = -1;
	
	if (!IsNativeObjectValid())
	{
		return Result;
	}
	
#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	Result = ((SKDownload*) IOSObject).timeRemaining;
#endif
	
	return Result;
}

FString UMGIosDownload::GetError()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosDownload::GetError"));
	
	FString Result;
	
	if (!IsNativeObjectValid())
	{
		return Result;
	}
	
#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	Result = ((SKDownload*) IOSObject).error.localizedDescription;
#endif
	
	return Result;
}

FString UMGIosDownload::GetContentPath()
{
	UE_LOG(LogMonetizationGoodies, Verbose, TEXT("UMGIosDownload::GetContentPath"));
	
	FString Result;
	
	if (!IsNativeObjectValid())
	{
		return Result;
	}
	
#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	Result = ((SKDownload*) IOSObject).contentURL.absoluteString;
#endif
	
	return Result;
}
