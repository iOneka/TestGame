// Copyright 2022 Just2Devs. All Rights Reserved.

#include "K2Node_FindDataTableRowsByProperty.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "DataTableEditorUtils.h"
#include "DataTableFL.h"
#include "K2Node_CallFunction.h"
#include "K2Node_IfThenElse.h"
#include "KismetCompiler.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/CompilerResultsLog.h"
#include "K2Node_DataTableRowBase.h"

#define LOCTEXT_NAMESPACE "K2Node_FindDataTableRowsByProperty"

namespace FindDataTableRowsByPropertyHelper
{
	const FName PropertyPinName = "Property";
	const FName OutRows = "OutRows";
	const FName RowsFoundPinName = "Rows Found";
	const FName RowsNotFoundPinName = "Rows Not Found";
}

void UK2Node_FindDataTableRowsByProperty::AllocateDefaultPins()
{
	// Add execution pins
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
	UEdGraphPin* RowFoundPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);
	RowFoundPin->PinFriendlyName = LOCTEXT("GetDataTableRow Rows Found Exec pin", "Rows Found");
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, FindDataTableRowsByPropertyHelper::RowsNotFoundPinName);

	// Add DataTable pin
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, UDataTable::StaticClass(), DataTableRowBaseNodeHelper::DataTablePinName);

	// Property pin
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Wildcard, FindDataTableRowsByPropertyHelper::PropertyPinName);

	// Result pin
	FCreatePinParams PinParams;
	PinParams.ContainerType = EPinContainerType::Array;
	UEdGraphPin* ResultPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Wildcard, UEdGraphSchema_K2::PN_ReturnValue, PinParams);
	ResultPin->PinFriendlyName = LOCTEXT("GetDataTableRow Output Row", "Out Rows");

	Super::AllocateDefaultPins();
}

FText UK2Node_FindDataTableRowsByProperty::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("FindDataTableRowsByProperty_NodeTitle", "Find Data Table Rows By Property");
}

FText UK2Node_FindDataTableRowsByProperty::GetTooltipText() const
{
	return LOCTEXT("FindDataTableRowsByProperty_TooltipText", "Find all the rows inside the data table which contain the property passed in.\nWARNING: The node doesn't support the use of literals connected to the property pin. Please make sure to use actual variables for the property pin.");
}

void UK2Node_FindDataTableRowsByProperty::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);

	const UEdGraphPin* WildCardPropertyPin = GetPropertyPin();
	if(WildCardPropertyPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Wildcard)
	{
		CompilerContext.MessageLog.Error(*LOCTEXT("FindDataTableRowByPropertyNoProperty_Error", "FindDataTableRowByProperty must have a property specified.").ToString(), this);
		return;
	}
	
	UEdGraphPin* OriginalDataTableInPin = GetDataTablePin();
	UDataTable* Table = (OriginalDataTableInPin != nullptr) ? Cast<UDataTable>(OriginalDataTableInPin->DefaultObject) : nullptr;
	if ((nullptr == OriginalDataTableInPin) || (0 == OriginalDataTableInPin->LinkedTo.Num() && nullptr == Table))
	{
		CompilerContext.MessageLog.Error(*LOCTEXT("FindDataTableRowByPropertyNoDataTable_Error", "FindDataTableRowByProperty must have a DataTable specified.").ToString(), this);
		// we break exec links so this is the only error we get
		BreakAllNodeLinks();
		return;
	}

	// FUNCTION NODE
	const FName FunctionName = FindDataTableRowsByPropertyFunctionName;
	UK2Node_CallFunction* FindDataTableRowByPropertyFunction = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	if (!FindDataTableRowByPropertyFunction)
	{
		CompilerContext.MessageLog.Error(*LOCTEXT("InvalidFunctionName", "Property connected may not be valid. @@").ToString(), this);
		return;
	}
	
	FindDataTableRowByPropertyFunction->FunctionReference.SetExternalMember(FunctionName, UDataTableFL::StaticClass());
	FindDataTableRowByPropertyFunction->AllocateDefaultPins();
	CompilerContext.MovePinLinksToIntermediate(*GetExecPin(), *(FindDataTableRowByPropertyFunction->GetExecPin()));

	// Connect the input of our GetDataTableRow to the Input of our Function pin
	UEdGraphPin* DataTableInPin = FindDataTableRowByPropertyFunction->FindPinChecked(TEXT("DataTable"));
	if (OriginalDataTableInPin->LinkedTo.Num() > 0)
	{
		// Copy the connection
		CompilerContext.MovePinLinksToIntermediate(*OriginalDataTableInPin, *DataTableInPin);
	}
	else
	{
		// Copy literal
		DataTableInPin->DefaultObject = OriginalDataTableInPin->DefaultObject;
	}

	UEdGraphPin* PropertyPin = FindDataTableRowByPropertyFunction->FindPinChecked(TEXT("Property"));
	CompilerContext.MovePinLinksToIntermediate(*GetPropertyPin(), *PropertyPin);

	// Get some pins to work with
	UEdGraphPin* OriginalOutRowPin = FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue);
	UEdGraphPin* FunctionOutRowPin = FindDataTableRowByPropertyFunction->FindPinChecked(TEXT("OutRows"));
	UEdGraphPin* FunctionReturnPin = FindDataTableRowByPropertyFunction->FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue);
	UEdGraphPin* FunctionThenPin = FindDataTableRowByPropertyFunction->GetThenPin();

	// Set the type of the OutRow pin on this expanded mode to match original
	FunctionOutRowPin->PinType = OriginalOutRowPin->PinType;
	FunctionOutRowPin->PinType.PinSubCategoryObject = OriginalOutRowPin->PinType.PinSubCategoryObject;

	//BRANCH NODE
	UK2Node_IfThenElse* BranchNode = CompilerContext.SpawnIntermediateNode<UK2Node_IfThenElse>(this, SourceGraph);
	BranchNode->AllocateDefaultPins();
	// Hook up inputs to branch
	FunctionThenPin->MakeLinkTo(BranchNode->GetExecPin());
	FunctionReturnPin->MakeLinkTo(BranchNode->GetConditionPin());

	// Hook up outputs
	CompilerContext.MovePinLinksToIntermediate(*GetThenPin(), *(BranchNode->GetThenPin()));
	CompilerContext.MovePinLinksToIntermediate(*GetRowsNotFoundPin(), *(BranchNode->GetElsePin()));
	CompilerContext.MovePinLinksToIntermediate(*OriginalOutRowPin, *FunctionOutRowPin);

	BreakAllNodeLinks();
}

FSlateIcon UK2Node_FindDataTableRowsByProperty::GetIconAndTint(FLinearColor& OutColor) const
{
	OutColor = GetNodeTitleColor();
	static FSlateIcon Icon("EditorStyle", "Kismet.AllClasses.FunctionIcon");
	return Icon;
}

void UK2Node_FindDataTableRowsByProperty::PostReconstructNode()
{
	Super::PostReconstructNode();

	RefreshPropertyPin();
}

void UK2Node_FindDataTableRowsByProperty::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	const UClass* ActionKey = GetClass();

	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);

		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

FText UK2Node_FindDataTableRowsByProperty::GetMenuCategory() const
{
	return LOCTEXT("FindDataTableRowsByProperty_MenuCategory", "Data Table Function Library");
}

void UK2Node_FindDataTableRowsByProperty::PinTypeChanged(UEdGraphPin* Pin)
{
	Super::PinTypeChanged(Pin);

	RefreshPropertyPin();
}

void UK2Node_FindDataTableRowsByProperty::GetPinHoverText(const UEdGraphPin& Pin, FString& HoverTextOut) const
{
	Super::GetPinHoverText(Pin, HoverTextOut);
	
	if(Pin.PinFriendlyName.EqualTo(FText::FromName(FindDataTableRowsByPropertyHelper::RowsFoundPinName)))
	{
		HoverTextOut = "Executed if the row is found.";
	}
	else if(Pin.PinName == FindDataTableRowsByPropertyHelper::RowsNotFoundPinName)
	{
		HoverTextOut = "Executed if the row is not found.";
	}
	else if(Pin.PinName == FindDataTableRowsByPropertyHelper::PropertyPinName)
	{
		HoverTextOut = "The property to look for inside the data table.";
	}
	else if(Pin.PinName == DataTableRowBaseNodeHelper::DataTablePinName)
	{
		HoverTextOut = "The data table where to look for the property.";
	}
	else if(Pin.PinFriendlyName.EqualTo(FText::FromName(FindDataTableRowsByPropertyHelper::OutRows)))
	{
		HoverTextOut = "The row found in the data table.";
	}
}

void UK2Node_FindDataTableRowsByProperty::RefreshPropertyPin()
{
	UEdGraphPin* Pin = GetPropertyPin();
	
	bool bPinTypeChanged = false;
		
	const bool bPinConnected = Pin->LinkedTo.Num() > 0;
	if (bPinConnected)
	{
		const UEdGraphPin* ArgumentSourcePin = Pin->LinkedTo[0];

		if (Pin->PinType != ArgumentSourcePin->PinType)
		{
			Pin->PinType = ArgumentSourcePin->PinType;
			bPinTypeChanged = true;
		}
			
		const FName PinCategory = Pin->PinType.PinCategory;
		if(PinCategory == UEdGraphSchema_K2::PC_Float)
		{
			FindDataTableRowsByPropertyFunctionName = "FindDataTableRowsByProperty_Float";
		}
		else if(PinCategory == UEdGraphSchema_K2::PC_Int)
		{
			FindDataTableRowsByPropertyFunctionName = "FindDataTableRowsByProperty_Int";
		}
		else if(PinCategory == UEdGraphSchema_K2::PC_Int64)
		{
			FindDataTableRowsByPropertyFunctionName = "FindDataTableRowsByProperty_Int64";
		}
		else if(PinCategory == UEdGraphSchema_K2::PC_String)
		{
			FindDataTableRowsByPropertyFunctionName = "FindDataTableRowsByProperty_String";
		}
		else if(PinCategory == UEdGraphSchema_K2::PC_Object)
		{
			FindDataTableRowsByPropertyFunctionName = "FindDataTableRowsByProperty_Object";
		}
		else if(PinCategory == UEdGraphSchema_K2::PC_Text)
		{
			FindDataTableRowsByPropertyFunctionName = "FindDataTableRowsByProperty_Text";
		}
		else if(PinCategory == UEdGraphSchema_K2::PC_Name)
		{
			FindDataTableRowsByPropertyFunctionName = "FindDataTableRowsByProperty_Name";
		}
		else if(PinCategory == UEdGraphSchema_K2::PC_Boolean)
		{
			FindDataTableRowsByPropertyFunctionName = "FindDataTableRowsByProperty_Bool";
		}
		else if(PinCategory == UEdGraphSchema_K2::PC_Byte)
		{
			FindDataTableRowsByPropertyFunctionName = "FindDataTableRowsByProperty_Enum";
		}
		else if(PinCategory == UEdGraphSchema_K2::PC_Struct)
		{
			if(Pin->PinType.PinSubCategoryObject.Get()->GetName() == "Vector") FindDataTableRowsByPropertyFunctionName = "FindDataTableRowsByProperty_FVector";
			if(Pin->PinType.PinSubCategoryObject.Get()->GetName() == "Transform") FindDataTableRowsByPropertyFunctionName = "FindDataTableRowsByProperty_FTransform";
			if(Pin->PinType.PinSubCategoryObject.Get()->GetName() == "Rotator") FindDataTableRowsByPropertyFunctionName = "FindDataTableRowsByProperty_FRotator";
		}
	}
	else
	{
		static const FEdGraphPinType WildcardPinType = FEdGraphPinType(UEdGraphSchema_K2::PC_Wildcard, NAME_None, nullptr, EPinContainerType::None, false, FEdGraphTerminalType());

		// Ensure wildcard
		if (Pin->PinType != WildcardPinType)
		{
			Pin->PinType = WildcardPinType;
			bPinTypeChanged = true;
		}
	}

	if(bPinTypeChanged)
	{
		// Let the graph know to refresh
		GetGraph()->NotifyGraphChanged();

		UBlueprint* Blueprint = GetBlueprint();
		if (!Blueprint->bBeingCompiled)
		{
			FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);
			Blueprint->BroadcastChanged();
		}
	}
}

void UK2Node_FindDataTableRowsByProperty::NotifyPinConnectionListChanged(UEdGraphPin* Pin)
{
	Super::NotifyPinConnectionListChanged(Pin);

	if (Pin == GetPropertyPin())
	{
		RefreshPropertyPin();
	}
}

UEdGraphPin* UK2Node_FindDataTableRowsByProperty::GetPropertyPin() const
{
	UEdGraphPin* Pin = FindPinChecked(FindDataTableRowsByPropertyHelper::PropertyPinName);
	check(Pin->Direction == EGPD_Input);
	return Pin;
}

UEdGraphPin* UK2Node_FindDataTableRowsByProperty::GetThenPin() const
{
	UEdGraphPin* Pin = FindPinChecked(UEdGraphSchema_K2::PN_Then);
	check(Pin->Direction == EGPD_Output);
	return Pin;
}

UEdGraphPin* UK2Node_FindDataTableRowsByProperty::GetRowsNotFoundPin() const
{
	UEdGraphPin* Pin = FindPinChecked(FindDataTableRowsByPropertyHelper::RowsNotFoundPinName);
	check(Pin->Direction == EGPD_Output);
	return Pin;
}

#undef LOCTEXT_NAMESPACE
