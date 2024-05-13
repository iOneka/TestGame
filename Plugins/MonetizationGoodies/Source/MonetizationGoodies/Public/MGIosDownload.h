// Copyright (c) 2021 Nineva Studios

#pragma once

#include "MGIosWrapperObject.h"

#include "MGIosDownload.generated.h"

class UMGIosPaymentTransaction;

/*
* The states that a download operation can be in.
*/
UENUM(BlueprintType)
enum class EMGDownloadState : uint8
{
	/*
	* Indicates that the download has not started yet.
	*/
	Waiting = 0,
	/*
	* Indicates that the content is currently being downloaded.
	*/
	Active = 1,
	/*
	* Indicates that your app paused the download.
	*/
	Paused = 2,
	/*
	* Indicates that the content was successfully downloaded.
	*/
	Finished = 3,
	/*
	* Indicates that an error occurred while the file was being downloaded.
	*/
	Failed = 4,
	/*
	* Indicates that your app canceled the download.
	*/
	Cancelled = 5
};

/*
* Downloadable content associated with a product.
*/
UCLASS(BlueprintType)
class MONETIZATIONGOODIES_API UMGIosDownload : public UMGIosWrapperObject
{
	GENERATED_BODY()

public:
	
	/*
	* The length of the downloadable content, in bytes.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|IOS")
	int64 GetExpectedContentLength();
	
	/*
	* A string that uniquely identifies the downloadable content.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|IOS")
	FString GetContentIdentifier();
	
	/*
	* A string that identifies which version of the content is available for download.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|IOS")
	FString GetContentVersion();
	
	/*
	* The transaction associated with the downloadable file.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|IOS")
	UMGIosPaymentTransaction* GetTransaction();
	
	/*
	* The current state of the download object.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|IOS")
	EMGDownloadState GetState();
	
	/*
	* A value that indicates how much of the file has been downloaded.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|IOS")
	float GetProgress();
	
	/*
	* An estimated time, in seconds, to finish downloading the content.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|IOS")
	float GetRemainingTime();
	
	/*
	* The error that prevented the content from being downloaded.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|IOS")
	FString GetError();
	
	/*
	* The local location of the downloaded file.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|IOS")
	FString GetContentPath();
};
