// Copyright 2022 Just2Devs. All Rights Reserved.

#include "K2Node_DoesDataTableContainRow.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "DataTableFL.h"
#include "K2Node_CallFunction.h"
#include "KismetCompiler.h"

#define LOCTEXT_NAMESPACE "K2Node_DoesDataTableContainRow"

void UK2Node_DoesDataTableContainRow::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();
	
	// Create data table pin
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, UDataTable::StaticClass(), DataTableRowBaseNodeHelper::DataTablePinName);

	// Create row pin
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Wildcard, DataTableRowBaseNodeHelper::RowPinName);

	// Create return value
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Boolean, UEdGraphSchema_K2::PN_ReturnValue);
}

FText UK2Node_DoesDataTableContainRow::GetTooltipText() const
{
	return LOCTEXT("DoesDataTableContainRow_Tooltip", "Check if the data table passed in contains a row exactly equal to the one provided, if so it returns true otherwise false.");
}

FText UK2Node_DoesDataTableContainRow::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("DoesDataTableContainRow_NodeTitle", "Does Data Table Contain Row");
}

FSlateIcon UK2Node_DoesDataTableContainRow::GetIconAndTint(FLinearColor& OutColor) const
{
	OutColor = GetNodeTitleColor();
	static FSlateIcon Icon("EditorStyle", "Kismet.AllClasses.FunctionIcon");
	return Icon;
}

void UK2Node_DoesDataTableContainRow::GetPinHoverText(const UEdGraphPin& Pin, FString& HoverTextOut) const
{
	Super::GetPinHoverText(Pin, HoverTextOut);

	if(Pin.PinName == DataTableRowBaseNodeHelper::DataTablePinName)
	{
		HoverTextOut = "The data table the node operates on.";
	}
	else if(Pin.PinName == DataTableRowBaseNodeHelper::RowPinName)
	{
		HoverTextOut = "The row the data table should contain.";
	}
	else if(Pin.PinName == UEdGraphSchema_K2::PN_ReturnValue)
	{
		HoverTextOut = "Returns true if the data table contains the row otherwise returns false.";
	}
}

void UK2Node_DoesDataTableContainRow::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);

    const UEdGraphPin* OriginalDataTableInPin = GetDataTablePin();
    const UDataTable* Table = (OriginalDataTableInPin != nullptr) ? Cast<UDataTable>(OriginalDataTableInPin->DefaultObject) : nullptr;
    if((nullptr == OriginalDataTableInPin) || (0 == OriginalDataTableInPin->LinkedTo.Num() && nullptr == Table))
    {
        CompilerContext.MessageLog.Error(*LOCTEXT("DoesDataTableContainRow_Error", "DoesDataTableContainRow must have a DataTable specified.").ToString(), this);
        // we break exec links so this is the only error we get
        BreakAllNodeLinks();
        return;
    }
	
	const UEdGraphPin* RowPin = GetRowPin();
	if(RowPin && RowPin->LinkedTo.Num() == 0)
	{
        CompilerContext.MessageLog.Error(*LOCTEXT("DoesDataTableContainRow_Error", "DoesDataTableContainRow must have a Row specified.").ToString(), this);
        // we break exec links so this is the only error we get
		BreakAllNodeLinks();
		return;
	}
	
	UK2Node_CallFunction* CallFunction = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	CallFunction->SetFromFunction(UDataTableFL::StaticClass()->FindFunctionByName(GET_MEMBER_NAME_CHECKED(UDataTableFL, DoesDataTableContainRow)));
	CallFunction->AllocateDefaultPins();

	UEdGraphPin* FunctionOutRowPin = CallFunction->FindPinChecked(DataTableRowBaseNodeHelper::RowPinName);
	const UEdGraphPin* OriginalOutRowPin = GetRowPin();
	
	// Set the type of the OutRow pin on this expanded mode to match original
	FunctionOutRowPin->PinType = OriginalOutRowPin->PinType;
	FunctionOutRowPin->PinType.PinSubCategoryObject = OriginalOutRowPin->PinType.PinSubCategoryObject;

	CompilerContext.MovePinLinksToIntermediate(*GetDataTablePin(), *CallFunction->FindPinChecked(DataTableRowBaseNodeHelper::DataTablePinName, EGPD_Input));
	CompilerContext.MovePinLinksToIntermediate(*GetRowPin(), *CallFunction->FindPin(DataTableRowBaseNodeHelper::RowPinName, EGPD_Input));
	CompilerContext.MovePinLinksToIntermediate(*FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue), *CallFunction->FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue));

	BreakAllNodeLinks();
}

void UK2Node_DoesDataTableContainRow::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	const UClass* ActionKey = GetClass();

	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);

		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

FText UK2Node_DoesDataTableContainRow::GetMenuCategory() const
{
	return LOCTEXT("DoesDataTableContainRow_MenuCategory", "Data Table Function Library");
}

UEdGraphPin* UK2Node_DoesDataTableContainRow::GetRowPin() const
{
	UEdGraphPin* Pin = FindPinChecked(DataTableRowBaseNodeHelper::RowPinName);
	check(Pin->Direction == EGPD_Input);
	return Pin;
}

#undef LOCTEXT_NAMESPACE
