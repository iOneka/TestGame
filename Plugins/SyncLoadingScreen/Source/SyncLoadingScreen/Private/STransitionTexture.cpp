// 


#include "STransitionTexture.h"

#include "Slate/DeferredCleanupSlateBrush.h"


void STransitionTexture::Construct(const FArguments& InArgs, UTexture2D* Texture2D)
{
	ImageBrush = FDeferredCleanupSlateBrush::CreateBrush(Texture2D);
	ChildSlot
	[
		SNew(SBorder)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		.Padding(FMargin(0.0f))
		[
			SNew(SImage)
			.Image(ImageBrush.IsValid() ? ImageBrush->GetSlateBrush() : nullptr)
		]
	];
}
