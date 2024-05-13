// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "AndroedStyle.h"

class FAndroedCommands : public TCommands<FAndroedCommands>
{
public:

	FAndroedCommands()
		: TCommands<FAndroedCommands>(TEXT("Androed"), NSLOCTEXT("Contexts", "Androed", "Androed Plugin"), NAME_None, FAndroedStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
