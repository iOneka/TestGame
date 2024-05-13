// Copyright Epic Games, Inc. All Rights Reserved.

#include "AndroedStyle.h"
#include "Androed.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/SlateStyleRegistry.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"

#define RootToContentDir Style->RootToContentDir

TSharedPtr<FSlateStyleSet> FAndroedStyle::StyleInstance = nullptr;

void FAndroedStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FAndroedStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FAndroedStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("AndroedStyle"));
	return StyleSetName;
}


const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);

TSharedRef< FSlateStyleSet > FAndroedStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("AndroedStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("Androed")->GetBaseDir() / TEXT("Resources"));

	Style->Set("Androed.PluginAction", new IMAGE_BRUSH(TEXT("Icon128"), Icon20x20));
	return Style;
}

void FAndroedStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FAndroedStyle::Get()
{
	return *StyleInstance;
}
