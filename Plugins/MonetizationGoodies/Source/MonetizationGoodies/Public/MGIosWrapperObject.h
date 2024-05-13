// Copyright (c) 2021 Nineva Studios

#pragma once

#include "MGIosWrapperObject.generated.h"

UCLASS(BlueprintType)
class UMGIosWrapperObject : public UObject
{
	GENERATED_BODY()

public:
	virtual ~UMGIosWrapperObject() override;

	/**
	 * Convenience function to check whether the native object is valid.
	 */
	UFUNCTION(BlueprintCallable, Category = "Monetization Goodies|IOS")
	bool IsNativeObjectValid();

#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	void Init(id iOSObject);
	id IOSObject;
#endif

};
