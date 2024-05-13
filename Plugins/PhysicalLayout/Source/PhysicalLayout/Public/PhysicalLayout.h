// Copyright Saeid Gholizade. All Rights Reserved. 2020

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"


/**  
* Physical layout module
*/
class FPhysicalLayoutModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
