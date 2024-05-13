// Copyright 2022 Just2Devs. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "K2Node_DataTableRowBase.h"
#include "K2Node_AddDataTableRow.generated.h"

UCLASS(BlueprintType)
class DATATABLEFUNCTIONLIBRARYEDITOR_API UK2Node_AddDataTableRow : public UK2Node_DataTableRowBase
{
	GENERATED_BODY()

public:

	//~ Begin UEdGraphNode Interface
	virtual void AllocateDefaultPins() override;
	virtual FText GetTooltipText() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FSlateIcon GetIconAndTint(FLinearColor& OutColor) const override;
	virtual void GetPinHoverText(const UEdGraphPin& Pin, FString& HoverTextOut) const override;
	//~ End UEdGraphNode Interface
	
	//~ Begin UK2Node Interface
	virtual bool IsNodePure() const override { return false; }
	virtual void ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual FText GetMenuCategory() const override;
	//~ End UK2Node Interface


protected:
	virtual UEdGraphPin* GetRowPin() const override;
	
};
