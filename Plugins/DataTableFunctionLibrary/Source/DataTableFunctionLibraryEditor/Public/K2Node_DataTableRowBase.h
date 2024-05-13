// Copyright 2022 Just2Devs. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "K2Node.h"
#include "K2Node_DataTableRowBase.generated.h"

namespace DataTableRowBaseNodeHelper
{
	const FName DataTablePinName = "DataTable";
	const FName RowPinName = "Row";
}

UCLASS()
class DATATABLEFUNCTIONLIBRARYEDITOR_API UK2Node_DataTableRowBase : public UK2Node
{
	GENERATED_BODY()

	friend class FDataTableFunctionLibraryPinFactory;
public:
	
	virtual void PostReconstructNode() override;
	virtual void PinDefaultValueChanged(UEdGraphPin* Pin) override;
	
	virtual bool IsNodeSafeToIgnore() const override { return true; }
	virtual void ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins) override;
	virtual bool IsConnectionDisallowed(const UEdGraphPin* MyPin, const UEdGraphPin* OtherPin, FString& OutReason) const override;
	virtual void EarlyValidation(class FCompilerResultsLog& MessageLog) const override;
	virtual void PreloadRequiredAssets() override;
	virtual void NotifyPinConnectionListChanged(UEdGraphPin* Pin) override;

protected:
	
	UEdGraphPin* GetDataTablePin(const TArray<UEdGraphPin*>* InPinsToSearch = nullptr) const;
	virtual UEdGraphPin* GetRowPin() const;
};
