// Copyright 2022 Just2Devs. All Rights Reserved.

#include "K2Node_AddDataTableRows.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "DataTableFL.h"
#include "K2Node_CallFunction.h"
#include "KismetCompiler.h"

namespace AddDataTableRowsHelper
{
	const FName RowPinName = "Rows";
	const FName SavePin = "bSave";
	const FName ReversePin = "bReverse";
}

#define LOCTEXT_NAMESPACE "K2Node_AddDataTableRows"

void UK2Node_AddDataTableRows::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();
	
	// Create exec pin
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);

	// Create then pin
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);
	
	// Create data table pin
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, UDataTable::StaticClass(), DataTableRowBaseNodeHelper::DataTablePinName);

	// Create rows pin
	FCreatePinParams PinParams;
	PinParams.ContainerType = EPinContainerType::Array;
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Wildcard, AddDataTableRowsHelper::RowPinName, PinParams);

	// Create reverse pin
	UEdGraphPin* ReversePin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Boolean, AddDataTableRowsHelper::ReversePin);
	ReversePin->bAdvancedView = true;
	
	// Create save pin
	UEdGraphPin* SavePin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Boolean, AddDataTableRowsHelper::SavePin);
	SavePin->bAdvancedView = true;

	// Create return value
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Boolean, UEdGraphSchema_K2::PN_ReturnValue);
	
	// Hide advanced pins by default
	AdvancedPinDisplay = ENodeAdvancedPins::Hidden;
}

FText UK2Node_AddDataTableRows::GetTooltipText() const
{
	return LOCTEXT("AddDataTableRows_Tooltip", "Adds an array of rows to the data table. EDITOR ONLY!\nWARNING: PLEASE BE CAREFUL WITH THIS NODE AS IT DIRECTLY MODIFIES THE DATA TABLE.");
}

FText UK2Node_AddDataTableRows::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("AddDataTableRows_NodeTitle", "Add Data Table Rows (EDITOR ONLY)");
}

FSlateIcon UK2Node_AddDataTableRows::GetIconAndTint(FLinearColor& OutColor) const
{
	OutColor = GetNodeTitleColor();
	static FSlateIcon Icon("EditorStyle", "Kismet.AllClasses.FunctionIcon");
	return Icon;
}

void UK2Node_AddDataTableRows::GetPinHoverText(const UEdGraphPin& Pin, FString& HoverTextOut) const
{
	Super::GetPinHoverText(Pin, HoverTextOut);

	if(Pin.PinName == DataTableRowBaseNodeHelper::DataTablePinName)
	{
		HoverTextOut = "The data table the node adds the rows to.";
	}
	else if(Pin.PinName == AddDataTableRowsHelper::RowPinName)
	{
		HoverTextOut = "The rows to add to the data table.";
	}
	else if(Pin.PinName == UEdGraphSchema_K2::PN_ReturnValue)
	{
		HoverTextOut = "Returns true if the rows were successfully added and the package saved, otherwise returns false.";
	}
	else if(Pin.PinName == AddDataTableRowsHelper::SavePin)
	{
		HoverTextOut = "Should the data table asset be automatically saved?";
	}
}

void UK2Node_AddDataTableRows::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);

    const UEdGraphPin* OriginalDataTableInPin = GetDataTablePin();
    const UDataTable* Table = (OriginalDataTableInPin != nullptr) ? Cast<UDataTable>(OriginalDataTableInPin->DefaultObject) : nullptr;
    if((nullptr == OriginalDataTableInPin) || (0 == OriginalDataTableInPin->LinkedTo.Num() && nullptr == Table))
    {
        CompilerContext.MessageLog.Error(*LOCTEXT("AddDataTableRows_Error", "AddDataTableRow must have a DataTable specified.").ToString(), this);
        // we break exec links so this is the only error we get
        BreakAllNodeLinks();
        return;
    }
	
	const UEdGraphPin* RowsPin = GetRowPin();
	if(RowsPin && RowsPin->LinkedTo.Num() == 0)
	{
        CompilerContext.MessageLog.Error(*LOCTEXT("AddDataTableRows_Error", "AddDataTableRows must have the Rows specified.").ToString(), this);
        // we break exec links so this is the only error we get
		BreakAllNodeLinks();
		return;
	}
	
	UK2Node_CallFunction* CallFunction = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	CallFunction->SetFromFunction(UDataTableFL::StaticClass()->FindFunctionByName(GET_MEMBER_NAME_CHECKED(UDataTableFL, AddDataTableRows)));
	CallFunction->AllocateDefaultPins();

	UEdGraphPin* FunctionRowsPin = CallFunction->FindPinChecked(AddDataTableRowsHelper::RowPinName);
	const UEdGraphPin* OriginalOutRowPin = GetRowPin();
	
	// Set the type of the OutRow pin on this expanded mode to match original
	FunctionRowsPin->PinType = OriginalOutRowPin->PinType;
	FunctionRowsPin->PinType.PinSubCategoryObject = OriginalOutRowPin->PinType.PinSubCategoryObject;

	CompilerContext.MovePinLinksToIntermediate(*GetExecPin(), *CallFunction->GetExecPin());
	CompilerContext.MovePinLinksToIntermediate(*FindPinChecked(UEdGraphSchema_K2::PN_Then), *CallFunction->FindPinChecked(UEdGraphSchema_K2::PN_Then));
	CompilerContext.MovePinLinksToIntermediate(*GetDataTablePin(), *CallFunction->FindPinChecked(DataTableRowBaseNodeHelper::DataTablePinName, EGPD_Input));
	CompilerContext.MovePinLinksToIntermediate(*GetRowPin(), *CallFunction->FindPin(AddDataTableRowsHelper::RowPinName, EGPD_Input));
	CompilerContext.MovePinLinksToIntermediate(*FindPinChecked(AddDataTableRowsHelper::ReversePin, EGPD_Input), *CallFunction->FindPinChecked(AddDataTableRowsHelper::ReversePin, EGPD_Input));
	CompilerContext.MovePinLinksToIntermediate(*FindPinChecked(AddDataTableRowsHelper::SavePin, EGPD_Input), *CallFunction->FindPinChecked(AddDataTableRowsHelper::SavePin, EGPD_Input));
	CompilerContext.MovePinLinksToIntermediate(*FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue), *CallFunction->FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue));

	BreakAllNodeLinks();
}

void UK2Node_AddDataTableRows::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	const UClass* ActionKey = GetClass();

	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);

		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

FText UK2Node_AddDataTableRows::GetMenuCategory() const
{
	return LOCTEXT("AddDataTableRows_MenuCategory", "Data Table Function Library|Editor Only");
}

UEdGraphPin* UK2Node_AddDataTableRows::GetRowPin() const
{
	UEdGraphPin* Pin = FindPinChecked(AddDataTableRowsHelper::RowPinName);
	check(Pin->Direction == EGPD_Input);
	return Pin;
}
#undef LOCTEXT_NAMESPACE