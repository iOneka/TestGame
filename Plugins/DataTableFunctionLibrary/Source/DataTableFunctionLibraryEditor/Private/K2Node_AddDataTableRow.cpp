// Copyright 2022 Just2Devs. All Rights Reserved.

#include "K2Node_AddDataTableRow.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "DataTableFL.h"
#include "K2Node_CallFunction.h"
#include "KismetCompiler.h"

#define LOCTEXT_NAMESPACE "K2Node_AddDataTableRow"

namespace AddDataTableRowHelper
{
	const FName SavePin = "bSave";
	const FName RowName = "RowName";
}

void UK2Node_AddDataTableRow::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();

	// Create exec pin
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);

	// Create then pin
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);
	
	// Create data table pin
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, UDataTable::StaticClass(), DataTableRowBaseNodeHelper::DataTablePinName);

	// Create row pin
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Wildcard, DataTableRowBaseNodeHelper::RowPinName);

	// Create row name pin
	UEdGraphPin* RowNamePin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Name, AddDataTableRowHelper::RowName);
	RowNamePin->bAdvancedView = true;
	RowNamePin->DefaultValue = "None";
	
	// Create save pin
	UEdGraphPin* SavePin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Boolean, AddDataTableRowHelper::SavePin);
	SavePin->bAdvancedView = true;
	AdvancedPinDisplay = ENodeAdvancedPins::Hidden;
	
	// Create return value
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Boolean, UEdGraphSchema_K2::PN_ReturnValue);
}

FText UK2Node_AddDataTableRow::GetTooltipText() const
{
	return LOCTEXT("AddDataTableRow_Tooltip", "Adds a new row to the data table. EDITOR ONLY!\nWARNING: PLEASE BE CAREFUL WITH THIS NODE AS IT DIRECTLY MODIFIES THE DATA TABLE.");
}

FText UK2Node_AddDataTableRow::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("AddDataTableRow_NodeTitle", "Add Data Table Row (EDITOR ONLY)");
}

FSlateIcon UK2Node_AddDataTableRow::GetIconAndTint(FLinearColor& OutColor) const
{
	OutColor = GetNodeTitleColor();
	static FSlateIcon Icon("EditorStyle", "Kismet.AllClasses.FunctionIcon");
	return Icon;
}

void UK2Node_AddDataTableRow::GetPinHoverText(const UEdGraphPin& Pin, FString& HoverTextOut) const
{
	Super::GetPinHoverText(Pin, HoverTextOut);

	if(Pin.PinName == DataTableRowBaseNodeHelper::DataTablePinName)
	{
		HoverTextOut = "The data table the node adds a row to.";
	}
	else if(Pin.PinName == DataTableRowBaseNodeHelper::RowPinName)
	{
		HoverTextOut = "The row to add to the data table.";
	}
	else if(Pin.PinName == UEdGraphSchema_K2::PN_ReturnValue)
	{
		HoverTextOut = "Returns true if the row was successfully added and the package saved, otherwise returns false.";
	}
	else if(Pin.PinName == AddDataTableRowHelper::SavePin)
	{
		HoverTextOut = "Should the data table asset be automatically saved?";
	}
	else if(Pin.PinName == AddDataTableRowHelper::RowName)
	{
		HoverTextOut = "The optional name for the row added. If the name is None or empty one gets generated. Passing in a name for a row that already exists overrides the current data table row.";
	}
}

void UK2Node_AddDataTableRow::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);

    UEdGraphPin* OriginalDataTableInPin = GetDataTablePin();
    UDataTable* Table = (OriginalDataTableInPin != nullptr) ? Cast<UDataTable>(OriginalDataTableInPin->DefaultObject) : nullptr;
    if((nullptr == OriginalDataTableInPin) || (0 == OriginalDataTableInPin->LinkedTo.Num() && nullptr == Table))
    {
        CompilerContext.MessageLog.Error(*LOCTEXT("AddDataTableRow_Error", "AddDataTableRow must have a DataTable specified.").ToString(), this);
        // we break exec links so this is the only error we get
        BreakAllNodeLinks();
        return;
    }
	
	UEdGraphPin* RowPin = GetRowPin();
	if(RowPin && RowPin->LinkedTo.Num() == 0)
	{
        CompilerContext.MessageLog.Error(*LOCTEXT("AddDataTableRow_Error", "AddDataTableRow must have a Row specified.").ToString(), this);
        // we break exec links so this is the only error we get
		BreakAllNodeLinks();
		return;
	}
	
	UK2Node_CallFunction* CallFunction = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	CallFunction->SetFromFunction(UDataTableFL::StaticClass()->FindFunctionByName(GET_MEMBER_NAME_CHECKED(UDataTableFL, AddDataTableRow)));
	CallFunction->AllocateDefaultPins();

	UEdGraphPin* FunctionOutRowPin = CallFunction->FindPinChecked(DataTableRowBaseNodeHelper::RowPinName);
	const UEdGraphPin* OriginalOutRowPin = GetRowPin();
	
	// Set the type of the OutRow pin on this expanded mode to match original
	FunctionOutRowPin->PinType = OriginalOutRowPin->PinType;
	FunctionOutRowPin->PinType.PinSubCategoryObject = OriginalOutRowPin->PinType.PinSubCategoryObject;

	CompilerContext.MovePinLinksToIntermediate(*GetExecPin(), *CallFunction->GetExecPin());
	CompilerContext.MovePinLinksToIntermediate(*FindPinChecked(UEdGraphSchema_K2::PN_Then), *CallFunction->FindPinChecked(UEdGraphSchema_K2::PN_Then));
	CompilerContext.MovePinLinksToIntermediate(*GetDataTablePin(), *CallFunction->FindPinChecked(DataTableRowBaseNodeHelper::DataTablePinName, EGPD_Input));
	CompilerContext.MovePinLinksToIntermediate(*GetRowPin(), *CallFunction->FindPin(DataTableRowBaseNodeHelper::RowPinName, EGPD_Input));
	CompilerContext.MovePinLinksToIntermediate(*FindPinChecked(AddDataTableRowHelper::RowName, EGPD_Input), *CallFunction->FindPinChecked(AddDataTableRowHelper::RowName, EGPD_Input));
	CompilerContext.MovePinLinksToIntermediate(*FindPinChecked(AddDataTableRowHelper::SavePin, EGPD_Input), *CallFunction->FindPinChecked(AddDataTableRowHelper::SavePin, EGPD_Input));
	CompilerContext.MovePinLinksToIntermediate(*FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue), *CallFunction->FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue));

	BreakAllNodeLinks();
}

void UK2Node_AddDataTableRow::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	const UClass* ActionKey = GetClass();

	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);

		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

FText UK2Node_AddDataTableRow::GetMenuCategory() const
{
	return LOCTEXT("AddDataTableRow_MenuCategory", "Data Table Function Library|Editor Only");
}

UEdGraphPin* UK2Node_AddDataTableRow::GetRowPin() const
{
	UEdGraphPin* Pin = FindPinChecked(DataTableRowBaseNodeHelper::RowPinName);
	check(Pin->Direction == EGPD_Input);
	return Pin;
}

#undef LOCTEXT_NAMESPACE
