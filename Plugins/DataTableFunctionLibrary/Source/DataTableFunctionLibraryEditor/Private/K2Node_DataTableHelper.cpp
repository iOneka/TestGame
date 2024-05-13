// Copyright 2022 Just2Devs. All Rights Reserved.

#include "K2Node_DataTableHelper.h"

void FK2Node_DataTableHelper::NotifyPinConnectionListChanged(UEdGraphPin* Pin, UEdGraphPin* DataTablePin, UEdGraphPin* ResultPin, const UEdGraphSchema* Schema)
{
	if (Pin == ResultPin)
	{
		const UEdGraphPin* TablePin = DataTablePin;
		// this connection would only change the output type if the table pin is undefined
		const bool bIsTypeAuthority = (TablePin->LinkedTo.Num() > 0 || TablePin->DefaultObject == nullptr);
		if (bIsTypeAuthority)
		{
			RefreshOutputPinType(DataTablePin, ResultPin, Schema);
		}
	}
	else if (Pin == DataTablePin)
	{
		const bool bConnectionAdded = Pin->LinkedTo.Num() > 0;
		if (bConnectionAdded)
		{
			// if the output connection was previously, incompatible, it now becomes the authority on this node's output type
			RefreshOutputPinType(DataTablePin, ResultPin, Schema);
		}
	}
}

void FK2Node_DataTableHelper::RefreshOutputPinType(UEdGraphPin* DataTablePin, UEdGraphPin* ResultPin, const UEdGraphSchema* Schema) 
{
	UScriptStruct* OutputType = GetDataTableRowStructType(DataTablePin, ResultPin);
	SetReturnTypeForStruct(Schema, ResultPin, OutputType);
}

void FK2Node_DataTableHelper::SetReturnTypeForStruct(const UEdGraphSchema* Schema, UEdGraphPin* ResultPin, UScriptStruct* NewRowStruct) 
{
	UScriptStruct* OldRowStruct = GetReturnTypeForStruct(ResultPin);
	if (NewRowStruct != OldRowStruct)
	{
		if (ResultPin->SubPins.Num() > 0)
		{
			Schema->RecombinePin(ResultPin);
		}

		// NOTE: purposefully not disconnecting the ResultPin (even though it changed type)... we want the user to see the old
		//       connections, and incompatible connections will produce an error (plus, some super-struct connections may still be valid)
		ResultPin->PinType.PinSubCategoryObject = NewRowStruct;
		ResultPin->PinType.PinCategory = (NewRowStruct == nullptr) ? UEdGraphSchema_K2::PC_Wildcard : UEdGraphSchema_K2::PC_Struct;
	}
}

UScriptStruct* FK2Node_DataTableHelper::GetReturnTypeForStruct(const UEdGraphPin* ResultPin) 
{
	UScriptStruct* ReturnStructType = static_cast<UScriptStruct*>(ResultPin->PinType.PinSubCategoryObject.Get());

	return ReturnStructType;
}

UScriptStruct* FK2Node_DataTableHelper::GetDataTableRowStructType(UEdGraphPin* DataTablePin, UEdGraphPin* ResultPin) 
{
	UScriptStruct* RowStructType = nullptr;

	if(DataTablePin && DataTablePin->DefaultObject != nullptr && DataTablePin->LinkedTo.Num() == 0)
	{
		if (const UDataTable* DataTable = Cast<const UDataTable>(DataTablePin->DefaultObject))
		{
			RowStructType = DataTable->RowStruct;
		}
	}

	if (RowStructType == nullptr)
	{
		if (ResultPin && ResultPin->LinkedTo.Num() > 0)
		{
			RowStructType = Cast<UScriptStruct>(ResultPin->LinkedTo[0]->PinType.PinSubCategoryObject.Get());
			for (int32 LinkIndex = 1; LinkIndex < ResultPin->LinkedTo.Num(); ++LinkIndex)
			{
				const UEdGraphPin* Link = ResultPin->LinkedTo[LinkIndex];
				UScriptStruct* LinkType = Cast<UScriptStruct>(Link->PinType.PinSubCategoryObject.Get());

				if (RowStructType->IsChildOf(LinkType))
				{
					RowStructType = LinkType;
				}
			}
		}
	}
	return RowStructType;
}
