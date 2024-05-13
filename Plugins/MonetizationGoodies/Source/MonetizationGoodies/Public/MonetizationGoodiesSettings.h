// Copyright (c) 2020 Nineva Studios

#pragma once

#include "Engine/EngineTypes.h"
#include "MonetizationGoodiesSettings.generated.h"

UCLASS(config = Engine, defaultconfig)
class MONETIZATIONGOODIES_API UMonetizationGoodiesSettings : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "IOS")
	bool EnableInAppPurchases = false;
};
