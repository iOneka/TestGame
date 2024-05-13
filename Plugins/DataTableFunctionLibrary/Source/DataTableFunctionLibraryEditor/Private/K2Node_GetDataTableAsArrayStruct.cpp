// Copyright 2022 Just2Devs. All Rights Reserved.

#include "K2Node_GetDataTableAsArrayStruct.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "EdGraphSchema_K2.h"
#include "K2Node_CallFunction.h"
#include "KismetCompiler.h"
#include "DataTableFL.h"
#include "K2Node_DataTableHelper.h"
#include "Engine/DataTable.h"
#include "Kismet2/CompilerResultsLog.h"
#include "K2Node_DataTableRowBase.h"

#define LOCTEXT_NAMESPACE "K2Node_GetDataTableAsArrayStruct"

namespace GetDataTableAsArrayStructHelper
{
	const FName OutRows = "OutRows";
}

void UK2Node_GetDataTableAsArrayStruct::AllocateDefaultPins()
{
	// Add execution pins
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);

	// Add DataTable pin
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, UDataTable::StaticClass(), DataTableRowBaseNodeHelper::DataTablePinName);

	// Result pin
	FCreatePinParams PinParams;
	PinParams.ContainerType = EPinContainerType::Array;
	UEdGraphPin* ResultPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Wildcard, UEdGraphSchema_K2::PN_ReturnValue, PinParams);
	ResultPin->PinFriendlyName = LOCTEXT("GetDataTableRow Output Row", "Out Rows");

	Super::AllocateDefaultPins();
}

FText UK2Node_GetDataTableAsArrayStruct::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("GetDataTableAsArrayStruct_NodeTitle", "Get Data Table As Array Struct");
}

FText UK2Node_GetDataTableAsArrayStruct::GetTooltipText() const
{
	return LOCTEXT("GetDataTableAsArrayStruct_TooltipText", "Convert the whole data table into the equivalent of an array of structs.");
}

void UK2Node_GetDataTableAsArrayStruct::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);
	
    UEdGraphPin* OriginalDataTableInPin = GetDataTablePin();
    UDataTable* Table = (OriginalDataTableInPin != NULL) ? Cast<UDataTable>(OriginalDataTableInPin->DefaultObject) : NULL;
    if((nullptr == OriginalDataTableInPin) || (0 == OriginalDataTableInPin->LinkedTo.Num() && nullptr == Table))
    {
        CompilerContext.MessageLog.Error(*LOCTEXT("GetDataTableRowNoDataTable_Error", "GetDataTableRow must have a DataTable specified.").ToString(), this);
        // we break exec links so this is the only error we get
        BreakAllNodeLinks();
        return;
    }

	// FUNCTION NODE
	const FName FunctionName = GET_FUNCTION_NAME_CHECKED(UDataTableFL, GetDataTableAsArrayStruct);
	UK2Node_CallFunction* GetDataTableRowFunction = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	GetDataTableRowFunction->FunctionReference.SetExternalMember(FunctionName, UDataTableFL::StaticClass());
	GetDataTableRowFunction->AllocateDefaultPins();
    CompilerContext.MovePinLinksToIntermediate(*GetExecPin(), *(GetDataTableRowFunction->GetExecPin()));

	// Connect the input of our GetDataTableRow to the Input of our Function pin
    UEdGraphPin* DataTableInPin = GetDataTableRowFunction->FindPinChecked(TEXT("DataTable"));
	if(OriginalDataTableInPin->LinkedTo.Num() > 0)
	{
		// Copy the connection
		CompilerContext.MovePinLinksToIntermediate(*OriginalDataTableInPin, *DataTableInPin);
	}
	else
	{
		// Copy literal
		DataTableInPin->DefaultObject = OriginalDataTableInPin->DefaultObject;
	}

	// Get some pins to work with
	UEdGraphPin* OriginalOutRowsPin = FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue);
	UEdGraphPin* FunctionOutRowsPin = GetDataTableRowFunction->FindPinChecked(DataTableRowBaseNodeHelper::RowPinName);
    UEdGraphPin* FunctionThenPin = GetDataTableRowFunction->GetThenPin();
        
    // Set the type of the OutRow pin on this expanded mode to match original
    FunctionOutRowsPin->PinType = OriginalOutRowsPin->PinType;
	FunctionOutRowsPin->PinType.PinSubCategoryObject = OriginalOutRowsPin->PinType.PinSubCategoryObject;
        
    // Hook up outputs
    CompilerContext.MovePinLinksToIntermediate(*GetThenPin(), *FunctionThenPin);
    CompilerContext.MovePinLinksToIntermediate(*OriginalOutRowsPin, *FunctionOutRowsPin);

	BreakAllNodeLinks();
}

FSlateIcon UK2Node_GetDataTableAsArrayStruct::GetIconAndTint(FLinearColor& OutColor) const
{
	OutColor = GetNodeTitleColor();
	static FSlateIcon Icon("EditorStyle", "Kismet.AllClasses.FunctionIcon");
	return Icon;
}

void UK2Node_GetDataTableAsArrayStruct::GetPinHoverText(const UEdGraphPin& Pin, FString& HoverTextOut) const
{
	Super::GetPinHoverText(Pin, HoverTextOut);

	if(Pin.PinName == DataTableRowBaseNodeHelper::DataTablePinName)
	{
		HoverTextOut = "The data table to convert into an array of struct.";
	}
	else if(Pin.PinName == GetDataTableAsArrayStructHelper::OutRows)
	{
		HoverTextOut = "The rows of the data table written inside an array of structs.";
	}
}

void UK2Node_GetDataTableAsArrayStruct::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	const UClass* ActionKey = GetClass();
	
	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);

		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

FText UK2Node_GetDataTableAsArrayStruct::GetMenuCategory() const
{
	return LOCTEXT("GetDataTableAsArrayStruct_Category", "Data Table Function Library");
}

UEdGraphPin* UK2Node_GetDataTableAsArrayStruct::GetThenPin() const
{
	UEdGraphPin* Pin = FindPin(UEdGraphSchema_K2::PN_Then);
	check(Pin == nullptr || Pin->Direction == EGPD_Output); // If pin exists, it must be output
	return Pin;
}
#undef LOCTEXT_NAMESPACE
