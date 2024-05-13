// Copyright Saeid Gholizade. All Rights Reserved. 2020

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateStyle.h"

/**  */
class FPhysicalLayoutStyle
{
public:
	/** Initialize toolkit style */
	static void Initialize();

	/** Destroys toolkit style */
	static void Shutdown();

	/** reloads textures used by slate renderer */
	static void ReloadTextures();

	/** Returns The Slate style set */
	static const ISlateStyle& Get();

	/** Returns The Slate style set name */
	static FName GetStyleSetName();

private:

	/** Creates all styles settings */
	static TSharedRef< class FSlateStyleSet > Create();

private:

	/** Slate Style Ptr */
	static TSharedPtr< class FSlateStyleSet > StyleInstance;
};