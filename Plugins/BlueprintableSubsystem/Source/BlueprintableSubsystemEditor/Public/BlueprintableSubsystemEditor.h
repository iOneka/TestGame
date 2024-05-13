// Copyright YTSS 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BlueprintableSubsystemAssetTypeActions.h"

class FBlueprintableSubsystemEditorModule : public IModuleInterface
{
protected:
    TArray<TSharedPtr<FAssetTypeActions_Base>> ActionTypes;
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};
