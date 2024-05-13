﻿// Copyright 2022 Just2Devs. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "K2Node_DataTableRowBase.h"
#include "K2Node_FindDataTableRowByProperty.generated.h"

UCLASS(BlueprintType)
class DATATABLEFUNCTIONLIBRARYEDITOR_API UK2Node_FindDataTableRowByProperty : public UK2Node_DataTableRowBase
{
	GENERATED_BODY()

public:
	
	//~ Begin UEdGraphNode Interface.
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;
	virtual void ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	virtual FSlateIcon GetIconAndTint(FLinearColor& OutColor) const override;
	virtual void PostReconstructNode() override;
	virtual void PinTypeChanged(UEdGraphPin* Pin) override;
	virtual void GetPinHoverText(const UEdGraphPin& Pin, FString& HoverTextOut) const override;
	//~ End UEdGraphNode Interface.
	
	//~ Begin UK2Node Interface
	virtual bool IsNodeSafeToIgnore() const override { return true; }
	virtual void ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins) override;
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual FText GetMenuCategory() const override;
	virtual void NotifyPinConnectionListChanged(UEdGraphPin* Pin) override;
	//~ End UK2Node Interface

private:
	void RefreshPropertyPin();
	
	UEdGraphPin* GetPropertyPin() const;
	UEdGraphPin* GetThenPin() const;
	UEdGraphPin* GetRowNotFoundPin() const;

	UPROPERTY()
	FName FindDataTableRowByPropertyFunctionName = "FindDataTableRowByProperty_Int";
};
