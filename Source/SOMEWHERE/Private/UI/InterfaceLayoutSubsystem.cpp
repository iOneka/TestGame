// 


#include "UI/InterfaceLayoutSubsystem.h"

#include "Blueprint/UserWidget.h"
#include "UI/InterfaceLayout.h"

bool UInterfaceLayoutSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return true;
}

void UInterfaceLayoutSubsystem::PlayerControllerChanged(APlayerController* NewPlayerController)
{
	Super::PlayerControllerChanged(NewPlayerController);

	if(IsValid(CurrentInterfaceLayout))
		CurrentInterfaceLayout->RemoveFromParent();
	
	const USomewhereSettings* SomewhereSettings = GetDefault<USomewhereSettings>();
	
	CurrentInterfaceLayout = CreateWidget<UInterfaceLayout>(NewPlayerController, SomewhereSettings->InterfaceLayoutClass);
	CurrentInterfaceLayout->AddToViewport(1000);
}
