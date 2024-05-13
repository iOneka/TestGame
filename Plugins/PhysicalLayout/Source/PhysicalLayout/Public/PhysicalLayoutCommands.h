// Copyright Saeid Gholizade. All Rights Reserved. 2020

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "PhysicalLayoutStyle.h"

/**
 * Physical layout mode commands
 */
class FPhysicalLayoutCommands : public TCommands< FPhysicalLayoutCommands >
{

public:
	/** Constructor */
	FPhysicalLayoutCommands()
		: TCommands<FPhysicalLayoutCommands>(
			TEXT("PhysicalLayout"), // Context name for fast lookup
			NSLOCTEXT("Contexts", "PhysicalLayout", "Physical Layout"), // Localized context name for displaying
			NAME_None, // Parent
			FPhysicalLayoutStyle::GetStyleSetName()
			)
	{
	}

	/** Command interface */
	virtual void RegisterCommands() override;

	/** Select command */
	TSharedPtr<FUICommandInfo> SelectCommand;
	
	/** Paint select command */
	TSharedPtr<FUICommandInfo> PaintSelectCommand;
	
	/** Transform command */
	TSharedPtr<FUICommandInfo> TransformationCommand;
	
	/** Paint place command */
	TSharedPtr<FUICommandInfo> PaintCommand;
	
	/** Command list */
	TArray<TSharedPtr<FUICommandInfo>> Commands = {
		SelectCommand,
		PaintSelectCommand,
		TransformationCommand,
		PaintCommand
	};

};
