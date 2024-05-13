// Copyright 2022 Just2Devs. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "K2Node.h"
#include "K2Node_ForEachDataTableRow.generated.h"

UCLASS(BlueprintType)
class DATATABLEFUNCTIONLIBRARYEDITOR_API UK2Node_ForEachDataTableRow : public UK2Node
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
	virtual void ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins) override;
	virtual bool IsConnectionDisallowed(const UEdGraphPin* MyPin, const UEdGraphPin* OtherPin, FString& OutReason) const override;
	virtual void EarlyValidation(FCompilerResultsLog& MessageLog) const override;
	virtual void PreloadRequiredAssets() override;
	virtual void ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual FText GetMenuCategory() const override;
	virtual void NotifyPinConnectionListChanged(UEdGraphPin* Pin) override;
	virtual void PostReconstructNode() override;
	virtual void PinDefaultValueChanged(UEdGraphPin* Pin) override;
	//~ End UK2Node Interface

private:
	UEdGraphPin* GetResultPin() const;
	UEdGraphPin* GetDataTablePin(const TArray<UEdGraphPin*>* InPinsToSearch = nullptr) const;
};
