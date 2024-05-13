// Copyright (c) 2021 Nineva Studios

#pragma once

#include "MGIosWrapperObject.h"

#include "MGIosStoreFront.generated.h"

/*
* An object containing the location and unique identifier of an Apple App Store storefront.
*/
UCLASS(BlueprintType)
class MONETIZATIONGOODIES_API UMGIosStoreFront : public UMGIosWrapperObject
{
	GENERATED_BODY()

public:
	
	/*
	* The three-letter code representing the country or region associated with the App Store storefront.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|IOS")
	FString GetCountryCode();
	
	/*
	* A value defined by Apple that uniquely identifies an App Store storefront.
	*/
	UFUNCTION(BlueprintPure, Category = "Monetization Goodies|IOS")
	FString GetIdentifier();
};
