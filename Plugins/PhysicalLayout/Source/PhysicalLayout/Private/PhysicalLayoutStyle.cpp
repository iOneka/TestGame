// Copyright Saeid Gholizade. All Rights Reserved. 2020


#include "PhysicalLayoutStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Framework/Application/SlateApplication.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"

TSharedPtr< FSlateStyleSet > FPhysicalLayoutStyle::StyleInstance = NULL;

void FPhysicalLayoutStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FPhysicalLayoutStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FPhysicalLayoutStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("PhysicalLayoutStyle"));
	return StyleSetName;
}

#define PLUGIN_IMAGE_BRUSH( Path, ... ) FSlateImageBrush( IPluginManager::Get().FindPlugin("PhysicalLayout")->GetBaseDir()  + "/" + Path + ".png", __VA_ARGS__ )
#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BOX_BRUSH( RelativePath, ... ) FSlateBoxBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BORDER_BRUSH( RelativePath, ... ) FSlateBorderBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define TTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".ttf") ), __VA_ARGS__ )
#define OTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".otf") ), __VA_ARGS__ )

const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);
const FVector2D Icon40x40(40.0f, 40.0f);

TSharedRef< FSlateStyleSet > FPhysicalLayoutStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("PhysicalLayoutStyle"));

	Style->SetContentRoot(FPaths::EngineContentDir() / TEXT("Editor/Slate"));

	Style->Set("PhysicalLayout.SelectCommand", new PLUGIN_IMAGE_BRUSH(TEXT("/Resources/select"), Icon40x40));
	Style->Set("PhysicalLayout.PaintSelectCommand", new PLUGIN_IMAGE_BRUSH(TEXT("/Resources/paintselect"), Icon40x40));
	Style->Set("PhysicalLayout.TransformationCommand", new PLUGIN_IMAGE_BRUSH(TEXT("/Resources/icon128"), Icon40x40));
	Style->Set("PhysicalLayout.PaintCommand", new PLUGIN_IMAGE_BRUSH(TEXT("/Resources/paint"), Icon40x40));
	Style->Set("PhysicalLayout.Delete", new IMAGE_BRUSH(TEXT("icons/icon_Delete_40x"), Icon40x40));

	Style->Set("Save", FButtonStyle()
			.SetNormal(PLUGIN_IMAGE_BRUSH("/Resources/icon_file_save_40x", Icon20x20))
			.SetPressed(PLUGIN_IMAGE_BRUSH("/Resources/icon_file_save_clicked_40x", Icon20x20))
			.SetHovered(PLUGIN_IMAGE_BRUSH("/Resources/icon_file_save_hover_40x", Icon20x20)));
	
	Style->Set("Delete", FButtonStyle()
		.SetNormal(IMAGE_BRUSH("icons/icon_Delete_40x", Icon20x20))
		.SetPressed(IMAGE_BRUSH("icons/icon_Delete_40x", Icon20x20))
		.SetHovered(IMAGE_BRUSH("icons/icon_Delete_40x", Icon20x20)));
	
	Style->Set("Select", FButtonStyle()
		.SetNormal(PLUGIN_IMAGE_BRUSH("/Resources/selectActors", Icon20x20))
		.SetPressed(PLUGIN_IMAGE_BRUSH("/Resources/selectActors", Icon20x20))
		.SetHovered(PLUGIN_IMAGE_BRUSH("/Resources/selectActors", Icon20x20)));
	
	Style->Set("PhysicalLayout.Icon",  new PLUGIN_IMAGE_BRUSH(TEXT("/Resources/icon128"), Icon40x40));
	
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("PhysicalLayout")->GetBaseDir() / TEXT("Resources"));

	Style->Set("ClassIcon.PhysicalLayoutPreset",  new PLUGIN_IMAGE_BRUSH(TEXT("/Resources/paint"), Icon40x40));
	Style->Set("ClassThumbnail.PhysicalLayoutPreset",  new PLUGIN_IMAGE_BRUSH(TEXT("/Resources/paint"), Icon40x40));
	
	return Style;
}

void FPhysicalLayoutStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FPhysicalLayoutStyle::Get()
{
	return *StyleInstance;
}

#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef BORDER_BRUSH
#undef TTF_FONT
#undef OTF_FONT
