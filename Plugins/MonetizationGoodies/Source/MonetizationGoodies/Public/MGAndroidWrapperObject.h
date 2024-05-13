// Copyright (c) 2021 Nineva Studios

#pragma once

#include "UObject/Object.h"

#if PLATFORM_ANDROID
#include "Android/AndroidJNI.h"
#endif

#include "MGAndroidWrapperObject.generated.h"

UCLASS(BlueprintType)
class UMGAndroidWrapperObject : public UObject
{
	GENERATED_BODY()

public:

	virtual ~UMGAndroidWrapperObject();

	/**
	 * Convenience function to check whether the native object is valid.
	 */
	UFUNCTION(BlueprintCallable, Category = "Monetization Goodies|Android")
	bool IsNativeObjectValid();

#if PLATFORM_ANDROID
	void Init(jobject javaObject);
	jobject JavaObject;
#endif

};
