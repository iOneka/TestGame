// Copyright (c) 2021 Nineva Studios

#include "MGIosWrapperObject.h"

#include "MonetizationGoodiesLog.h"

UMGIosWrapperObject::~UMGIosWrapperObject()
{
	if (!IsNativeObjectValid())
	{
		return;
	}

#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	[IOSObject release];
#endif
}

bool UMGIosWrapperObject::IsNativeObjectValid()
{
	bool Result = false;

#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
	Result = IOSObject != nil;
#endif
#if !WITH_EDITOR
	if (!Result)
	{
		UE_LOG(LogMonetizationGoodies, Error, TEXT("IOS native object is not valid."));
	}
#endif
	return Result;
}

#if PLATFORM_IOS && MG_ENABLE_PURCHASES_IOS
void UMGIosWrapperObject::Init(id iOSObject)
{
	if (iOSObject == nil) {
		return;
	}
	IOSObject = iOSObject;
	[IOSObject retain];
}
#endif
