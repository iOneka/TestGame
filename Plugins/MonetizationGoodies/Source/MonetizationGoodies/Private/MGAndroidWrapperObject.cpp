// Copyright (c) 2021 Nineva Studios

#include "MGAndroidWrapperObject.h"

#include "MonetizationGoodiesLog.h"

#if PLATFORM_ANDROID
#include "Android/Utils/MGJavaConvertor.h"
#include "Android/Utils/MGMethodCallUtils.h"
#endif

UMGAndroidWrapperObject::~UMGAndroidWrapperObject()
{
	if (!IsNativeObjectValid())
	{
		return;
	}

#if PLATFORM_ANDROID
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	Env->DeleteGlobalRef(JavaObject);
	JavaObject = nullptr;
#endif
}

bool UMGAndroidWrapperObject::IsNativeObjectValid()
{
	bool Result = false;

#if PLATFORM_ANDROID
	Result = JavaObject != nullptr;
#endif
#if !WITH_EDITOR
	if (!Result)
	{
		UE_LOG(LogMonetizationGoodies, Error, TEXT("Android native object is not valid."));
	}
#endif
	return Result;
}

#if PLATFORM_ANDROID
void UMGAndroidWrapperObject::Init(jobject javaObject)
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	JavaObject = Env->NewGlobalRef(javaObject);
}
#endif