// Copyright 2022 Just2Devs. All Rights Reserved.

#include "K2Node_GetDataTableRowName.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "DataTableFL.h"
#include "K2Node_CallFunction.h"
#include "KismetCompiler.h"

#define LOCTEXT_NAMESPACE "K2Node_GetDataTableRowName"

void UK2Node_GetDataTableRowName::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();
	
	// Create data table pin
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, UDataTable::StaticClass(), DataTableRowBaseNodeHelper::DataTablePinName);

	// Create row pin
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Wildcard, DataTableRowBaseNodeHelper::RowPinName);

	// Create return value
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Name, UEdGraphSchema_K2::PN_ReturnValue);
}

FText UK2Node_GetDataTableRowName::GetTooltipText() const
{
	return LOCTEXT("GetDataTableRowName_Tooltip", "Gets the name of the row passed in. Returns None if the name does not exist.");
}

FText UK2Node_GetDataTableRowName::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("GetDataTableRowName_NodeTitle", "Get Data Table Row Name");
}

FSlateIcon UK2Node_GetDataTableRowName::GetIconAndTint(FLinearColor& OutColor) const
{
	OutColor = GetNodeTitleColor();
	static FSlateIcon Icon("EditorStyle", "Kismet.AllClasses.FunctionIcon");
	return Icon;
}

void UK2Node_GetDataTableRowName::GetPinHoverText(const UEdGraphPin& Pin, FString& HoverTextOut) const
{
	Super::GetPinHoverText(Pin, HoverTextOut);

	if(Pin.PinName == DataTableRowBaseNodeHelper::DataTablePinName)
	{
		HoverTextOut = "The data table the node operates on.";
	}
	else if(Pin.PinName == DataTableRowBaseNodeHelper::RowPinName)
	{
		HoverTextOut = "The row to retrieve the name for.";
	}
	else if(Pin.PinName == UEdGraphSchema_K2::PN_ReturnValue)
	{
		HoverTextOut = "Returns the name of the row if found otherwise returns None.";
	}
}

void UK2Node_GetDataTableRowName::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);

    const UEdGraphPin* OriginalDataTableInPin = GetDataTablePin();
    const UDataTable* Table = (OriginalDataTableInPin != nullptr) ? Cast<UDataTable>(OriginalDataTableInPin->DefaultObject) : nullptr;
    if((nullptr == OriginalDataTableInPin) || (0 == OriginalDataTableInPin->LinkedTo.Num() && nullptr == Table))
    {
        CompilerContext.MessageLog.Error(*LOCTEXT("GetDataTableRowName_Error", "GetDataTableRowName must have a DataTable specified.").ToString(), this);
        // we break exec links so this is the only error we get
        BreakAllNodeLinks();
        return;
    }
	
	const UEdGraphPin* RowPin = GetRowPin();
	if(RowPin && RowPin->LinkedTo.Num() == 0)
	{
        CompilerContext.MessageLog.Error(*LOCTEXT("GetDataTableRowName_Error", "GetDataTableRowName must have a Row specified.").ToString(), this);
        // we break exec links so this is the only error we get
		BreakAllNodeLinks();
		return;
	}
	
	UK2Node_CallFunction* CallFunction = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	CallFunction->SetFromFunction(UDataTableFL::StaticClass()->FindFunctionByName(GET_MEMBER_NAME_CHECKED(UDataTableFL, GetDataTableRowName)));
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

void UK2Node_GetDataTableRowName::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	const UClass* ActionKey = GetClass();

	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);

		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

FText UK2Node_GetDataTableRowName::GetMenuCategory() const
{
	return LOCTEXT("GetDataTableRowName_MenuCategory", "Data Table Function Library");
}

UEdGraphPin* UK2Node_GetDataTableRowName::GetRowPin() const
{
	UEdGraphPin* Pin = FindPinChecked(DataTableRowBaseNodeHelper::RowPinName);
	check(Pin->Direction == EGPD_Input);
	return Pin;
}

#undef LOCTEXT_NAMESPACE
