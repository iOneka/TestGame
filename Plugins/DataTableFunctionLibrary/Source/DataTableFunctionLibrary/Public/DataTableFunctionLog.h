// Copyright 2022 Just2Devs. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"

DECLARE_LOG_CATEGORY_EXTERN(DTFLog, Log, Log);

#define CUR_FUNC (FString(__FUNCTION__).Right(FString(__FUNCTION__).Len() - FString(__FUNCTION__).Find(TEXT("::")) - 2 ))

/** PrintF(CategoryName, true, 1.0f, TEXT("Some value %i"), 5);*/
#define PrintF(Category, Print, Duration, Format, ...) \
{ \
	FString StringToPrint = FString::Printf(Format, ##__VA_ARGS__); \
	StringToPrint = "(" + CUR_FUNC + ") " + StringToPrint; \
	if(Print && GEngine) GEngine->AddOnScreenDebugMessage(-1, Duration, FColor::Cyan, StringToPrint); \
	UE_LOG(Category, Log, TEXT("%s"), *StringToPrint); \
}

/* QPrintF(TEXT("Some value %i"), 5);  */
#define QPrintF(Format, ...) \
{ \
	FString StringToPrint = FString::Printf(Format, ##__VA_ARGS__); \
	StringToPrint = "(" + CUR_FUNC + ") " + StringToPrint; \
	if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, StringToPrint); \
	UE_LOG(LogTemp, Log, TEXT("%s"), *StringToPrint); \
}

/** PrintWF(CategoryName, true, 1.0f, TEXT("Some value %i"), 5);*/
#define PrintWF(Category, Print, Duration, Format, ...) \
{ \
	FString StringToPrint = FString::Printf(Format, ##__VA_ARGS__); \
	StringToPrint = "(" + CUR_FUNC + ") " + StringToPrint; \
	if(Print && GEngine) GEngine->AddOnScreenDebugMessage(-1, Duration, FColor::Yellow, StringToPrint); \
	UE_LOG(Category, Warning, TEXT("%s"), *StringToPrint); \
}

/* QPrintWF(TEXT("Some Value %i"), 5)*/
#define QPrintWF(Format, ...) \
{ \
	FString StringToPrint = FString::Printf(Format, ##__VA_ARGS__); \
	StringToPrint = "(" + CUR_FUNC + ") " + StringToPrint; \
	if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, StringToPrint); \
	UE_LOG(LogTemp, Warning, TEXT("%s"), *StringToPrint); \
}

/** PrintEF(CategoryName, true, 1.0f, TEXT("Some value %i"), 5);*/
#define PrintEF(Category, Print, Duration, Format, ...) \
{ \
	FString StringToPrint = FString::Printf(Format, ##__VA_ARGS__); \
	StringToPrint = "(" + CUR_FUNC + ") " + StringToPrint; \
	if(Print && GEngine) GEngine->AddOnScreenDebugMessage(-1, Duration, FColor::Red, StringToPrint); \
	UE_LOG(Category, Error, TEXT("%s"), *StringToPrint); \
}

/* QPrintEF(TEXT("Some Value %i"), 5); */
#define QPrintEF(Format, ...) \
{ \
	FString StringToPrint = FString::Printf(Format, ##__VA_ARGS__); \
	StringToPrint = "(" + CUR_FUNC + ") " + StringToPrint; \
	if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, StringToPrint); \
	UE_LOG(LogTemp, Error, TEXT("%s"), *StringToPrint); \
}

/** Print(CategoryName, true, 1.0f, "Something"); */
#define Print(Category, Print, Duration, String) \
{\
	FString StringToPrint = "(" + CUR_FUNC + ") " + String; \
	if(Print && GEngine) GEngine->AddOnScreenDebugMessage(-1, Duration, FColor::Cyan, StringToPrint); \
	UE_LOG(Category, Log, TEXT("%s"), *StringToPrint); \
}

/* QPrint("Something"); */
#define QPrint(String) \
{\
	FString StringToPrint = "(" + CUR_FUNC + ") " + String; \
	if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, StringToPrint); \
	UE_LOG(LogTemp, Log, TEXT("%s"), *StringToPrint); \
}

/** PrintW(CategoryName, true, 1.0f, "Something"); */
#define PrintW(Category, Print, Duration, String) \
{ \
	FString StringToPrint = "(" + CUR_FUNC + ") " + String; \
	if(Print && GEngine) GEngine->AddOnScreenDebugMessage(-1, Duration, FColor::Yellow, StringToPrint); \
	UE_LOG(Category, Warning, TEXT("%s"), *StringToPrint); \
}

/* QPrintW("Something"); */
#define QPrintW(String) \
{ \
	FString StringToPrint = "(" + CUR_FUNC + ") " + String; \
	if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, StringToPrint); \
	UE_LOG(LogTemp, Warning, TEXT("%s"), *StringToPrint); \
}

/** PrintE(CategoryName, true, 1.0f, "Something"); */
#define PrintE(Category, Print, Duration, String) \
{ \
	FString StringToPrint = "(" + CUR_FUNC + ") " + String; \
	if(Print && GEngine) GEngine->AddOnScreenDebugMessage(-1, Duration, FColor::Red, StringToPrint); \
	UE_LOG(Category, Error, TEXT("%s"), *StringToPrint); \
}

/* QPrintE("Something"); */
#define QPrintE(String) \
{ \
	FString StringToPrint = "(" + CUR_FUNC + ") " + String; \
	if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, StringToPrint); \
	UE_LOG(LogTemp, Error, TEXT("%s"), *StringToPrint); \
}
