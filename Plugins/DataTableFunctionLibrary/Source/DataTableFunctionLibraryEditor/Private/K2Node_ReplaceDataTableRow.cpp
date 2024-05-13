// Copyright 2022 Just2Devs. All Rights Reserved.

#include "K2Node_ReplaceDataTableRow.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "DataTableFL.h"
#include "FindInBlueprintManager.h"
#include "K2Node_CallFunction.h"
#include "KismetCompiler.h"

#define LOCTEXT_NAMESPACE "K2Node_ReplaceDataTableRow"

namespace ReplaceDataTableRowHelper
{
	const FName RowName = "RowName";
	const FName SavePin = "bSave";
}

void UK2Node_ReplaceDataTableRow::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();

	// Create exec pin
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);

	// Create then pin
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);
	
	// Create data table pin
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, UDataTable::StaticClass(), DataTableRowBaseNodeHelper::DataTablePinName);

	// Create the row name pin
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Name, ReplaceDataTableRowHelper::RowName);
	
	// Create row pin
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Wildcard, DataTableRowBaseNodeHelper::RowPinName);

	// Create save pin
	UEdGraphPin* SavePin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Boolean, ReplaceDataTableRowHelper::SavePin);
	SavePin->bAdvancedView = true;
	AdvancedPinDisplay = ENodeAdvancedPins::Hidden;
	
	// Create return value
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Boolean, UEdGraphSchema_K2::PN_ReturnValue);
}

FText UK2Node_ReplaceDataTableRow::GetTooltipText() const
{
	return LOCTEXT("ReplaceDataTableRow_Tooltip", "Finds a row by the given name and replaces a row from the data table. EDITOR ONLY!\nWARNING: PLEASE BE CAREFUL WITH THIS NODE AS IT DIRECTLY MODIFIES THE DATA TABLE.");
}

FText UK2Node_ReplaceDataTableRow::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("ReplaceDataTableRow_NodeTitle", "Replace Data Table Row (EDITOR ONLY)");
}

FSlateIcon UK2Node_ReplaceDataTableRow::GetIconAndTint(FLinearColor& OutColor) const
{
	OutColor = GetNodeTitleColor();
	static FSlateIcon Icon("EditorStyle", "Kismet.AllClasses.FunctionIcon");
	return Icon;
}

void UK2Node_ReplaceDataTableRow::GetPinHoverText(const UEdGraphPin& Pin, FString& HoverTextOut) const
{
	Super::GetPinHoverText(Pin, HoverTextOut);

	if(Pin.PinName == DataTableRowBaseNodeHelper::DataTablePinName)
	{
		HoverTextOut = "The data table the node replace a row from.";
	}
	else if(Pin.PinName == DataTableRowBaseNodeHelper::RowPinName)
	{
		HoverTextOut = "The row that will replace the found row.";
	}
	else if(Pin.PinName == UEdGraphSchema_K2::PN_ReturnValue)
	{
		HoverTextOut = "Returns true if the row was successfully replaced, otherwise returns false.";
	}
	else if(Pin.PinName == ReplaceDataTableRowHelper::SavePin)
	{
		HoverTextOut = "Should the data table asset be automatically saved?";
	}
	else if(Pin.PinName == ReplaceDataTableRowHelper::RowName)
	{
		HoverTextOut = "The row name of the row to find and replace.";
	}
}

void UK2Node_ReplaceDataTableRow::PinDefaultValueChanged(UEdGraphPin* Pin)
{
	Super::PinDefaultValueChanged(Pin);
	
	if (Pin && Pin->PinName == DataTableRowBaseNodeHelper::DataTablePinName)
	{
		UEdGraphPin* RowNamePin = FindPinChecked(ReplaceDataTableRowHelper::RowName, EGPD_Input);
		UDataTable*  DataTable = Cast<UDataTable>(Pin->DefaultObject);
		if (RowNamePin)
		{
			if (DataTable && (RowNamePin->DefaultValue.IsEmpty() || !DataTable->GetRowMap().Contains(*RowNamePin->DefaultValue)))
			{
				if (const auto Iterator = DataTable->GetRowMap().CreateConstIterator())
				{
					RowNamePin->DefaultValue = Iterator.Key().ToString();
				}
			}	

			RefreshRowNameOptions();
		}
	}
}

void UK2Node_ReplaceDataTableRow::NotifyPinConnectionListChanged(UEdGraphPin* Pin)
{
	Super::NotifyPinConnectionListChanged(Pin);

	if (Pin == GetDataTablePin())
	{
		const bool bConnectionAdded = Pin->LinkedTo.Num() > 0;
		if (bConnectionAdded)
		{
			RefreshRowNameOptions();
		}
	}
}

void UK2Node_ReplaceDataTableRow::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);

    const UEdGraphPin* OriginalDataTableInPin = GetDataTablePin();
    const UDataTable* Table = (OriginalDataTableInPin != nullptr) ? Cast<UDataTable>(OriginalDataTableInPin->DefaultObject) : nullptr;
    if((nullptr == OriginalDataTableInPin) || (0 == OriginalDataTableInPin->LinkedTo.Num() && nullptr == Table))
    {
        CompilerContext.MessageLog.Error(*LOCTEXT("ReplaceDataTableRow_Error", "ReplaceDataTableRow must have a DataTable specified.").ToString(), this);
        // we break exec links so this is the only error we get
        BreakAllNodeLinks();
        return;
    }
	
	const UEdGraphPin* RowPin = GetRowPin();
	if(RowPin && RowPin->LinkedTo.Num() == 0)
	{
        CompilerContext.MessageLog.Error(*LOCTEXT("ReplaceDataTableRow_Error", "ReplaceDataTableRow must have a Row specified.").ToString(), this);
        // we break exec links so this is the only error we get
		BreakAllNodeLinks();
		return;
	}
	
	UK2Node_CallFunction* CallFunction = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	CallFunction->SetFromFunction(UDataTableFL::StaticClass()->FindFunctionByName(GET_MEMBER_NAME_CHECKED(UDataTableFL, ReplaceDataTableRow)));
	CallFunction->AllocateDefaultPins();

	UEdGraphPin* FunctionOutRowPin = CallFunction->FindPinChecked(DataTableRowBaseNodeHelper::RowPinName);
	const UEdGraphPin* OriginalOutRowPin = GetRowPin();
	
	// Set the type of the OutRow pin on this expanded mode to match original
	FunctionOutRowPin->PinType = OriginalOutRowPin->PinType;
	FunctionOutRowPin->PinType.PinSubCategoryObject = OriginalOutRowPin->PinType.PinSubCategoryObject;

	CompilerContext.MovePinLinksToIntermediate(*GetExecPin(), *CallFunction->GetExecPin());
	CompilerContext.MovePinLinksToIntermediate(*FindPinChecked(UEdGraphSchema_K2::PN_Then), *CallFunction->FindPinChecked(UEdGraphSchema_K2::PN_Then));
	CompilerContext.MovePinLinksToIntermediate(*GetDataTablePin(), *CallFunction->FindPinChecked(DataTableRowBaseNodeHelper::DataTablePinName, EGPD_Input));
	CompilerContext.MovePinLinksToIntermediate(*FindPinChecked(ReplaceDataTableRowHelper::RowName, EGPD_Input), *CallFunction->FindPinChecked(ReplaceDataTableRowHelper::RowName, EGPD_Input));
	CompilerContext.MovePinLinksToIntermediate(*GetRowPin(), *CallFunction->FindPin(DataTableRowBaseNodeHelper::RowPinName, EGPD_Input));
	CompilerContext.MovePinLinksToIntermediate(*FindPinChecked(ReplaceDataTableRowHelper::SavePin, EGPD_Input), *CallFunction->FindPinChecked(ReplaceDataTableRowHelper::SavePin, EGPD_Input));
	CompilerContext.MovePinLinksToIntermediate(*FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue), *CallFunction->FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue));

	BreakAllNodeLinks();
}

void UK2Node_ReplaceDataTableRow::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	const UClass* ActionKey = GetClass();

	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);

		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

FText UK2Node_ReplaceDataTableRow::GetMenuCategory() const
{
	return LOCTEXT("ReplaceDataTableRow_MenuCategory", "Data Table Function Library|Editor Only");
}

UEdGraphPin* UK2Node_ReplaceDataTableRow::GetRowPin() const
{
	UEdGraphPin* Pin = FindPinChecked(DataTableRowBaseNodeHelper::RowPinName);
	check(Pin->Direction == EGPD_Input);
	return Pin;
}

void UK2Node_ReplaceDataTableRow::RefreshRowNameOptions()
{
	UEdGraph* Graph = GetGraph();
	Graph->NotifyGraphChanged();
}
#undef LOCTEXT_NAMESPACE 
