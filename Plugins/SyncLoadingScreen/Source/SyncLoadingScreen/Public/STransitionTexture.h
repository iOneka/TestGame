// 

#pragma once

#include "Widgets/SCompoundWidget.h"

class FDeferredCleanupSlateBrush;

/**
 * 
 */
class SYNCLOADINGSCREEN_API STransitionTexture : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(STransitionTexture) {}

	SLATE_END_ARGS()
	
	void Construct(const FArguments& InArgs, UTexture2D* Texture2D);
	
private:
	TSharedPtr<FDeferredCleanupSlateBrush> ImageBrush;
};
