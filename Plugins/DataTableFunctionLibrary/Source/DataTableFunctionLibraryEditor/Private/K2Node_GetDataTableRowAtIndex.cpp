// Copyright 2022 Just2Devs. All Rights Reserved.

#include "K2Node_GetDataTableRowAtIndex.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "DataTableFL.h"
#include "K2Node_CallFunction.h"
#include "KismetCompiler.h"
#include "K2Node_DataTableRowBase.h"

#define LOCTEXT_NAMESPACE "K2Node_GetDataTableRowAtIndex"

namespace GetDataTableRowAtIndexNodeHelper
{
	const FName IndexPinName = "Index";
}

void UK2Node_GetDataTableRowAtIndex::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();

	// Create data table pin
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, UDataTable::StaticClass(), DataTableRowBaseNodeHelper::DataTablePinName);

	// Create index pin
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Int, GetDataTableRowAtIndexNodeHelper::IndexPinName);

	// Create row pin
	UEdGraphPin* RowPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Wildcard, UEdGraphSchema_K2::PN_ReturnValue);
	RowPin->PinFriendlyName = FText::FromName(DataTableRowBaseNodeHelper::RowPinName);
}

FText UK2Node_GetDataTableRowAtIndex::GetTooltipText() const
{
	return LOCTEXT("GetDataTableRowAtIndex_Tooltip", "Get the data table row by an index. The index must be within the data table size -1. The index starts at 0.");
}

FText UK2Node_GetDataTableRowAtIndex::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("GetDataTableRowAtIndex_NodeTitle", "Get Data Table Row At Index");
}

FSlateIcon UK2Node_GetDataTableRowAtIndex::GetIconAndTint(FLinearColor& OutColor) const
{
	OutColor = GetNodeTitleColor();
	static FSlateIcon Icon("EditorStyle", "Kismet.AllClasses.FunctionIcon");
	return Icon;
}

void UK2Node_GetDataTableRowAtIndex::GetPinHoverText(const UEdGraphPin& Pin, FString& HoverTextOut) const
{
	Super::GetPinHoverText(Pin, HoverTextOut);

	if(Pin.PinName == GetDataTableRowAtIndexNodeHelper::IndexPinName)
	{
		HoverTextOut = "The index of the row to retrieve.";
	}
	else if(Pin.PinFriendlyName.EqualTo(FText::FromName(DataTableRowBaseNodeHelper::RowPinName)))
	{
		HoverTextOut = "The row retrieved from the data table using the index.";
	}
	else if(Pin.PinName == DataTableRowBaseNodeHelper::DataTablePinName)
	{
		HoverTextOut = "The data table to search.";
	}
}

void UK2Node_GetDataTableRowAtIndex::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);

	UK2Node_CallFunction* CallFunction = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	CallFunction->SetFromFunction(UDataTableFL::StaticClass()->FindFunctionByName(GET_MEMBER_NAME_CHECKED(UDataTableFL, GetDataTableRowAtIndex)));
	CallFunction->AllocateDefaultPins();

	UEdGraphPin* FunctionOutRowPin = CallFunction->FindPinChecked(DataTableRowBaseNodeHelper::RowPinName);
	const UEdGraphPin* OriginalOutRowPin = GetRowPin();
	// Set the type of the OutRow pin on this expanded mode to match original
	FunctionOutRowPin->PinType = OriginalOutRowPin->PinType;
	FunctionOutRowPin->PinType.PinSubCategoryObject = OriginalOutRowPin->PinType.PinSubCategoryObject;
	
	CompilerContext.MovePinLinksToIntermediate(*GetDataTablePin(), *CallFunction->FindPinChecked(DataTableRowBaseNodeHelper::DataTablePinName, EGPD_Input));
	CompilerContext.MovePinLinksToIntermediate(*GetIndexPin(), *CallFunction->FindPinChecked(GetDataTableRowAtIndexNodeHelper::IndexPinName, EGPD_Input));
	CompilerContext.MovePinLinksToIntermediate(*GetRowPin(), *CallFunction->FindPin(DataTableRowBaseNodeHelper::RowPinName, EGPD_Output));

	BreakAllNodeLinks();
}

void UK2Node_GetDataTableRowAtIndex::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	const UClass* ActionKey = GetClass();

	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);

		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

FText UK2Node_GetDataTableRowAtIndex::GetMenuCategory() const
{
	return LOCTEXT("GetDataTableRowAtIndex_MenuCategory", "Data Table Function Library");
}

UEdGraphPin* UK2Node_GetDataTableRowAtIndex::GetIndexPin() const
{
	UEdGraphPin* Pin = FindPinChecked(GetDataTableRowAtIndexNodeHelper::IndexPinName);
	check(Pin->Direction == EGPD_Input);
	return Pin;
}

#undef LOCTEXT_NAMESPACE
