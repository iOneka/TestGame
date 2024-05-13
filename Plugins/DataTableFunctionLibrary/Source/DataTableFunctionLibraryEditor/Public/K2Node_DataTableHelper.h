// Copyright 2022 Just2Devs. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"

struct FK2Node_DataTableHelper
{
public:
	static void NotifyPinConnectionListChanged(UEdGraphPin* Pin, UEdGraphPin* DataTablePin, UEdGraphPin* ResultPin, const UEdGraphSchema* Schema);
	
	static void RefreshOutputPinType(UEdGraphPin* DataTablePin, UEdGraphPin* ResultPin, const UEdGraphSchema* Schema);
	static UScriptStruct* GetDataTableRowStructType(UEdGraphPin* DataTablePin, UEdGraphPin* ResultPin);
	static void SetReturnTypeForStruct(const UEdGraphSchema* Schema, UEdGraphPin* ResultPin, UScriptStruct* NewRowStruct);
	static UScriptStruct* GetReturnTypeForStruct(const UEdGraphPin* ResultPin);
};