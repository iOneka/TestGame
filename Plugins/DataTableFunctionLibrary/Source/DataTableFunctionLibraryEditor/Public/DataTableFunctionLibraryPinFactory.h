// Copyright 2022 Just2Devs. All Rights Reserved.

#pragma once
#include "EdGraph/EdGraphPin.h"
#include "EdGraphSchema_K2.h"
#include "SGraphPinDataTableRowName.h"
#include "EdGraphUtilities.h"
#include "K2Node_ReplaceDataTableRow.h"

class FDataTableFunctionLibraryPinFactory : public FGraphPanelPinFactory
{
	virtual TSharedPtr<SGraphPin> CreatePin(UEdGraphPin* InPin) const override
	{
		if (InPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Name)
		{
			UObject* Outer = InPin->GetOuter();

			// Create drop down combo boxes for DataTable and CurveTable RowName pins
			const UEdGraphPin* DataTablePin = nullptr;
			if (Outer->IsA(UK2Node_ReplaceDataTableRow::StaticClass()))
			{
				 const UK2Node_ReplaceDataTableRow* ReplaceDataTableRowNode = CastChecked<UK2Node_ReplaceDataTableRow>(Outer);
				 DataTablePin = ReplaceDataTableRowNode->GetDataTablePin();
			}

			if (DataTablePin)
			{
				 if (DataTablePin->DefaultObject != nullptr && DataTablePin->LinkedTo.Num() == 0)
				 {
					 if (auto DataTable = Cast<UDataTable>(DataTablePin->DefaultObject))
					 {
						 return SNew(SGraphPinDataTableRowName, InPin, DataTable);
					 }
				 }
			}
		}

		return nullptr;
	}
};
