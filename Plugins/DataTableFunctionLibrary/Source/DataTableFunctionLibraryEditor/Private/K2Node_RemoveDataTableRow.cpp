// Copyright 2022 Just2Devs. All Rights Reserved.


#include "K2Node_RemoveDataTableRow.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "DataTableFL.h"
#include "K2Node_CallFunction.h"
#include "KismetCompiler.h"

#define LOCTEXT_NAMESPACE "K2Node_RemoveDataTableRow"

namespace RemoveDataTableRowHelper
{
	const FName SavePin = "bSave";
}

void UK2Node_RemoveDataTableRow::AllocateDefaultPins()
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

	// Create save pin
	UEdGraphPin* SavePin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Boolean, RemoveDataTableRowHelper::SavePin);
	SavePin->bAdvancedView = true;
	AdvancedPinDisplay = ENodeAdvancedPins::Hidden;
	
	// Create return value
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Boolean, UEdGraphSchema_K2::PN_ReturnValue);
}

FText UK2Node_RemoveDataTableRow::GetTooltipText() const
{
	return LOCTEXT("RemoveDataTableRow_Tooltip", "Removes a row from the data table. EDITOR ONLY!\nWARNING: PLEASE BE CAREFUL WITH THIS NODE AS IT DIRECTLY MODIFIES THE DATA TABLE.");
}

FText UK2Node_RemoveDataTableRow::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("RemoveDataTableRow_NodeTitle", "Remove Data Table Row (EDITOR ONLY)");
}

FSlateIcon UK2Node_RemoveDataTableRow::GetIconAndTint(FLinearColor& OutColor) const
{
	OutColor = GetNodeTitleColor();
	static FSlateIcon Icon("EditorStyle", "Kismet.AllClasses.FunctionIcon");
	return Icon;
}

void UK2Node_RemoveDataTableRow::GetPinHoverText(const UEdGraphPin& Pin, FString& HoverTextOut) const
{
	Super::GetPinHoverText(Pin, HoverTextOut);

	if(Pin.PinName == DataTableRowBaseNodeHelper::DataTablePinName)
	{
		HoverTextOut = "The data table the node removes a row from.";
	}
	else if(Pin.PinName == DataTableRowBaseNodeHelper::RowPinName)
	{
		HoverTextOut = "The row to remove from the data table.";
	}
	else if(Pin.PinName == UEdGraphSchema_K2::PN_ReturnValue)
	{
		HoverTextOut = "Returns true if the row was successfully removed and the package saved, otherwise returns false.";
	}
	else if(Pin.PinName == RemoveDataTableRowHelper::SavePin)
	{
		HoverTextOut = "Should the data table asset be automatically saved?";
	}
}

void UK2Node_RemoveDataTableRow::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);

    UEdGraphPin* OriginalDataTableInPin = GetDataTablePin();
    UDataTable* Table = (OriginalDataTableInPin != nullptr) ? Cast<UDataTable>(OriginalDataTableInPin->DefaultObject) : nullptr;
    if((nullptr == OriginalDataTableInPin) || (0 == OriginalDataTableInPin->LinkedTo.Num() && nullptr == Table))
    {
        CompilerContext.MessageLog.Error(*LOCTEXT("RemoveDataTableRow_Error", "RemoveDataTableRow must have a DataTable specified.").ToString(), this);
        // we break exec links so this is the only error we get
        BreakAllNodeLinks();
        return;
    }
	
	UEdGraphPin* RowPin = GetRowPin();
	if(RowPin && RowPin->LinkedTo.Num() == 0)
	{
        CompilerContext.MessageLog.Error(*LOCTEXT("RemoveDataTableRow_Error", "RemoveDataTableRow must have a Row specified.").ToString(), this);
        // we break exec links so this is the only error we get
		BreakAllNodeLinks();
		return;
	}
	
	UK2Node_CallFunction* CallFunction = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	CallFunction->SetFromFunction(UDataTableFL::StaticClass()->FindFunctionByName(GET_MEMBER_NAME_CHECKED(UDataTableFL, RemoveDataTableRow)));
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
	CompilerContext.MovePinLinksToIntermediate(*FindPinChecked(RemoveDataTableRowHelper::SavePin, EGPD_Input), *CallFunction->FindPinChecked(RemoveDataTableRowHelper::SavePin, EGPD_Input));
	CompilerContext.MovePinLinksToIntermediate(*FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue), *CallFunction->FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue));

	BreakAllNodeLinks();
}

void UK2Node_RemoveDataTableRow::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	const UClass* ActionKey = GetClass();

	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);

		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

FText UK2Node_RemoveDataTableRow::GetMenuCategory() const
{
	return LOCTEXT("RemoveDataTableRow_MenuCategory", "Data Table Function Library|Editor Only");
}

UEdGraphPin* UK2Node_RemoveDataTableRow::GetRowPin() const
{
	UEdGraphPin* Pin = FindPinChecked(DataTableRowBaseNodeHelper::RowPinName);
	check(Pin->Direction == EGPD_Input);
	return Pin;
}

#undef LOCTEXT_NAMESPACE
