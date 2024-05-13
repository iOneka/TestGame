// Copyright 2022 Just2Devs. All Rights Reserved.

#include "K2Node_FindDataTableRowByProperty.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "DataTableEditorUtils.h"
#include "DataTableFL.h"
#include "K2Node_CallFunction.h"
#include "K2Node_DataTableRowBase.h"
#include "K2Node_IfThenElse.h"
#include "KismetCompiler.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/CompilerResultsLog.h"

#define LOCTEXT_NAMESPACE "K2Node_FindDataTableRowByProperty"

namespace FindDataTableRowByPropertyHelper
{
	const FName PropertyPinName = "Property";
	const FName OutRows = "OutRow";
	const FName RowFoundPinName = "Row Found";
	const FName RowNotFoundPinName = "Row Not Found";
}

void UK2Node_FindDataTableRowByProperty::AllocateDefaultPins()
{
	// Add execution pins
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
	UEdGraphPin* RowFoundPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);
	RowFoundPin->PinFriendlyName = LOCTEXT("GetDataTableRow Row Found Exec pin", "Row Found");
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, FindDataTableRowByPropertyHelper::RowNotFoundPinName);

	// Add DataTable pin
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, UDataTable::StaticClass(), DataTableRowBaseNodeHelper::DataTablePinName);

	// Property pin
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Wildcard, FindDataTableRowByPropertyHelper::PropertyPinName);

	// Result pin
	UEdGraphPin* ResultPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Wildcard, UEdGraphSchema_K2::PN_ReturnValue);
	ResultPin->PinFriendlyName = FText::FromName(FindDataTableRowByPropertyHelper::OutRows);

	Super::AllocateDefaultPins();
}

FText UK2Node_FindDataTableRowByProperty::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("FindDataTableRowByProperty_NodeTitle", "Find Data Table Row By Property");
}

FText UK2Node_FindDataTableRowByProperty::GetTooltipText() const
{
	return LOCTEXT("FindDataTableRowByProperty_TooltipText", "Find a row inside the data table which contains the property passed in.\nWARNING: The node doesn't support the use of literals connected to the property pin. Please make sure to use actual variables for the property pin.");
}

void UK2Node_FindDataTableRowByProperty::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
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
	const FName FunctionName = FindDataTableRowByPropertyFunctionName;
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
	UEdGraphPin* FunctionOutRowPin = FindDataTableRowByPropertyFunction->FindPinChecked(TEXT("OutRow"));
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
	CompilerContext.MovePinLinksToIntermediate(*GetRowNotFoundPin(), *(BranchNode->GetElsePin()));
	CompilerContext.MovePinLinksToIntermediate(*OriginalOutRowPin, *FunctionOutRowPin);

	BreakAllNodeLinks();
}

FSlateIcon UK2Node_FindDataTableRowByProperty::GetIconAndTint(FLinearColor& OutColor) const
{
	OutColor = GetNodeTitleColor();
	static FSlateIcon Icon("EditorStyle", "Kismet.AllClasses.FunctionIcon");
	return Icon;
}

void UK2Node_FindDataTableRowByProperty::PostReconstructNode()
{
	Super::PostReconstructNode();
	RefreshPropertyPin();
}

void UK2Node_FindDataTableRowByProperty::ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins)
{
	Super::ReallocatePinsDuringReconstruction(OldPins);

	if (const UEdGraphPin* DataTablePin = GetDataTablePin(&OldPins))
	{
		if (UDataTable* DataTable = Cast<UDataTable>(DataTablePin->DefaultObject))
		{
			// make sure to properly load the data-table object so that we can 
			// farm the "RowStruct" property from it (below, in GetDataTableRowStructType)
			PreloadObject(DataTable);
		}
	}
}

void UK2Node_FindDataTableRowByProperty::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	const UClass* ActionKey = GetClass();

	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);

		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

FText UK2Node_FindDataTableRowByProperty::GetMenuCategory() const
{
	return LOCTEXT("FindDataTableRowByProperty_MenuCategory", "Data Table Function Library");
}

void UK2Node_FindDataTableRowByProperty::NotifyPinConnectionListChanged(UEdGraphPin* Pin)
{
	Super::NotifyPinConnectionListChanged(Pin);
	
	if (Pin == GetPropertyPin())
	{
		RefreshPropertyPin();
	}
}

void UK2Node_FindDataTableRowByProperty::PinTypeChanged(UEdGraphPin* Pin)
{
	Super::PinTypeChanged(Pin);

	RefreshPropertyPin();
}

void UK2Node_FindDataTableRowByProperty::GetPinHoverText(const UEdGraphPin& Pin, FString& HoverTextOut) const
{
	Super::GetPinHoverText(Pin, HoverTextOut);

	if(Pin.PinFriendlyName.EqualTo(FText::FromName(FindDataTableRowByPropertyHelper::RowFoundPinName)))
	{
		HoverTextOut = "Executed if the row is found.";
	}
	else if(Pin.PinName == FindDataTableRowByPropertyHelper::RowNotFoundPinName)
	{
		HoverTextOut = "Executed if the row is not found.";
	}
	else if(Pin.PinName == FindDataTableRowByPropertyHelper::PropertyPinName)
	{
		HoverTextOut = "The property to look for inside the data table.";
	}
	else if(Pin.PinName == DataTableRowBaseNodeHelper::DataTablePinName)
	{
		HoverTextOut = "The data table where to look for the property.";
	}
	else if(Pin.PinFriendlyName.EqualTo(FText::FromName(FindDataTableRowByPropertyHelper::OutRows)))
	{
		HoverTextOut = "The row found in the data table.";
	}
}

void UK2Node_FindDataTableRowByProperty::RefreshPropertyPin()
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
			FindDataTableRowByPropertyFunctionName = "FindDataTableRowByProperty_Float";
		}
		else if(PinCategory == UEdGraphSchema_K2::PC_Int)
		{
			FindDataTableRowByPropertyFunctionName = "FindDataTableRowByProperty_Int";
		}
		else if(PinCategory == UEdGraphSchema_K2::PC_Int64)
		{
			FindDataTableRowByPropertyFunctionName = "FindDataTableRowByProperty_Int64";
		}
		else if(PinCategory == UEdGraphSchema_K2::PC_String)
		{
			FindDataTableRowByPropertyFunctionName = "FindDataTableRowByProperty_String";
		}
		else if(PinCategory == UEdGraphSchema_K2::PC_Object)
		{
			FindDataTableRowByPropertyFunctionName = "FindDataTableRowByProperty_Object";
		}
		else if(PinCategory == UEdGraphSchema_K2::PC_Text)
		{
			FindDataTableRowByPropertyFunctionName = "FindDataTableRowByProperty_Text";
		}
		else if(PinCategory == UEdGraphSchema_K2::PC_Name)
		{
			FindDataTableRowByPropertyFunctionName = "FindDataTableRowByProperty_Name";
		}
		else if(PinCategory == UEdGraphSchema_K2::PC_Boolean)
		{
			FindDataTableRowByPropertyFunctionName = "FindDataTableRowByProperty_Bool";
		}
		else if(PinCategory == UEdGraphSchema_K2::PC_Byte)
		{
			FindDataTableRowByPropertyFunctionName = "FindDataTableRowByProperty_Enum";
		}
		else if(PinCategory == UEdGraphSchema_K2::PC_Struct)
		{
			if(Pin->PinType.PinSubCategoryObject.Get()->GetName() == "Vector") FindDataTableRowByPropertyFunctionName = "FindDataTableRowByProperty_FVector";
			if(Pin->PinType.PinSubCategoryObject.Get()->GetName() == "Transform") FindDataTableRowByPropertyFunctionName = "FindDataTableRowByProperty_FTransform";
			if(Pin->PinType.PinSubCategoryObject.Get()->GetName() == "Rotator") FindDataTableRowByPropertyFunctionName = "FindDataTableRowByProperty_FRotator";
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

UEdGraphPin* UK2Node_FindDataTableRowByProperty::GetPropertyPin() const
{
	UEdGraphPin* Pin = FindPinChecked(FindDataTableRowByPropertyHelper::PropertyPinName);
	check(Pin->Direction == EGPD_Input);
	return Pin;
}

UEdGraphPin* UK2Node_FindDataTableRowByProperty::GetThenPin() const
{
	UEdGraphPin* Pin = FindPinChecked(UEdGraphSchema_K2::PN_Then);
	check(Pin->Direction == EGPD_Output);
	return Pin;
}

UEdGraphPin* UK2Node_FindDataTableRowByProperty::GetRowNotFoundPin() const
{
	UEdGraphPin* Pin = FindPinChecked(FindDataTableRowByPropertyHelper::RowNotFoundPinName);
	check(Pin->Direction == EGPD_Output);
	return Pin;
}

#undef LOCTEXT_NAMESPACE
