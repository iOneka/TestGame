#include "../../Public/Game/SomewhereGameInstance.h"

#include "Library/SomewhereStatics.h"

USomewhereGameInstance::USomewhereGameInstance()
{
}

void USomewhereGameInstance::Init()
{
	Super::Init();

#if WITH_EDITOR
	ULevelEditorPlaySettings* PlaySettings = GetMutableDefault<ULevelEditorPlaySettings>();

	if (USomewhereStatics::IsDesktopPlatform(this))
		PlaySettings->UseMouseForTouch = false;

	if (USomewhereStatics::IsMobilePlatform(this))
		PlaySettings->UseMouseForTouch = true;
#endif
}

void USomewhereGameInstance::Shutdown()
{
	Super::Shutdown();

#if WITH_EDITOR
	ULevelEditorPlaySettings* PlaySettings = GetMutableDefault<ULevelEditorPlaySettings>();
	PlaySettings->UseMouseForTouch = false;
#endif
}
