// Copyright Epic Games, Inc. All Rights Reserved.

#include "AndroedCommands.h"

#define LOCTEXT_NAMESPACE "FAndroedModule"

void FAndroedCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "Androed", "А ВЫ ЗНАЕТЕ ЧТО АНДРОЕДЫ ПО НОЧАМ СЪЕДАТ МОЗГ ?!", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
