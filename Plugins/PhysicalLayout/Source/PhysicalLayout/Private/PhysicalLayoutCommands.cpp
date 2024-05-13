// Copyright Saeid Gholizade. All Rights Reserved. 2020


#include "PhysicalLayoutCommands.h"

#define LOCTEXT_NAMESPACE "PhysialLayoutCommands"

void FPhysicalLayoutCommands::RegisterCommands()
{
	UI_COMMAND(SelectCommand, "Select", "Select Objects", EUserInterfaceActionType::RadioButton, FInputChord());
	UI_COMMAND(PaintSelectCommand, "Paint Select", "Paint Select Objects (hold Q to activate)", EUserInterfaceActionType::RadioButton, FInputChord());
	UI_COMMAND(TransformationCommand, "Transform", "Manipulate object physicaly", EUserInterfaceActionType::RadioButton, FInputChord());
	UI_COMMAND(PaintCommand, "Paint Place", "Paint object physicaly", EUserInterfaceActionType::RadioButton, FInputChord());
}
#undef LOCTEXT_NAMESPACE
