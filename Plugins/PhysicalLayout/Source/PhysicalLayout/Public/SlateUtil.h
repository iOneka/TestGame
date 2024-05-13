// Copyright Saeid Gholizade. All Rights Reserved. 2020

#pragma once

#include "CoreMinimal.h"

#include "Framework/MultiBox/MultiBox.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Misc/Attribute.h"
#include "Textures/SlateIcon.h"
#include "PhysicalLayoutCommands.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Input/SVectorInputBox.h"
#include "Widgets/Input/SRotatorInputBox.h"
#include "Widgets/Input/SSpinBox.h"

#include "Widgets/Views/SListView.h"
#include "Widgets/Input/SSlider.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SComboButton.h"
#include "Widgets/Text/STextBlock.h"
/**
 * The Slate Utility functions, which will grow during the projects
 */
class PHYSICALLAYOUT_API SlateUtil
{
public:

	/** Creates Sized TextBlock in a SBox  */
	static TSharedPtr<SWidget> SSizedTextBlock(FText InText, float InWidth, float InHeight)
	{
		return SNew(SBox)
			.HeightOverride(InHeight)
			.VAlign(EVerticalAlignment::VAlign_Center)
			[
				SNew(STextBlock)
				.Text(InText)
			];
			
	}
};
