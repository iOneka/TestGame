// Copyright 2022 Just2Devs. All Rights Reserved.

#include "K2Node_FindDataTableRowByPredicate.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "DataTableFL.h"
#include "K2Node_CallFunction.h"
#include "K2Node_IfThenElse.h"
#include "KismetCompiler.h"

#define LOCTEXT_NAMESPACE "K2Node_FindDataTableRowByPredicate"

namespace FindDataTableRowByPredicateHelper
{
	const FName RowFoundPinName = "Row Found";
	const FName RowNotFoundPinName = "Row Not Found";
	const FName PredicatePinName = "Predicate";
}

void UK2Node_FindDataTableRowByPredicate::AllocateDefaultPins()
{
	// Add execution pins
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
	UEdGraphPin* RowFoundPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);
	RowFoundPin->PinFriendlyName = LOCTEXT("GetDataTableRow Row Found Exec pin", "Row Found");
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, FindDataTableRowByPredicateHelper::RowNotFoundPinName);

	// Add DataTable pin
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, UDataTable::StaticClass(), DataTableRowBaseNodeHelper::DataTablePinName);

	// Add predicate pin
	FCreatePinParams PinParams;
	PinParams.bIsReference = true;
	PinParams.bIsConst = true;
	UEdGraphPin* PredicatePin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Delegate, FindDataTableRowByPredicateHelper::PredicatePinName, PinParams);
	FProperty* Property = GetClass()->FindPropertyByName("Delegate");
	const FDelegateProperty* DelegateProperty = CastField<FDelegateProperty>(Property);
	FMemberReference::FillSimpleMemberReference<UFunction>(DelegateProperty->SignatureFunction, PredicatePin->PinType.PinSubCategoryMemberReference);
	
	// Result pin
	UEdGraphPin* ResultPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Wildcard, UEdGraphSchema_K2::PN_ReturnValue);
	ResultPin->PinFriendlyName = FText::FromName(DataTableRowBaseNodeHelper::RowPinName);
	
	Super::AllocateDefaultPins();
}

FText UK2Node_FindDataTableRowByPredicate::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("FindDataTableRowByPredicate_NodeTitle", "Find Data Table Row By Predicate");
}

FText UK2Node_FindDataTableRowByPredicate::GetTooltipText() const
{
	return LOCTEXT("FindDataTableRowByPredicate_Tooltip", "Find a row in the data table using a predicate. A predicate is a function that returns true or false for each row this function loops through.\nEssentially the predicate asks the question of is this the row you wanted? If yes then return true otherwise return false.\nThe predicate must be a function that takes in a DataTableRowObject and return a boolean. You can create it yourself or use the create event node to create the function for you.\nInside the predicate function use the node GetAsDataTableRow to convert the data table row object into the data table struct.\nIMPORTANT: The predicate function must name the parameters specifically RowObject and ReturnValue. If the names are any different the function won't be considered compatible with the predicate pin.");
}

void UK2Node_FindDataTableRowByPredicate::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);
	
	UEdGraphPin* OriginalDataTableInPin = GetDataTablePin();
	UDataTable* Table = (OriginalDataTableInPin != nullptr) ? Cast<UDataTable>(OriginalDataTableInPin->DefaultObject) : nullptr;
	if ((nullptr == OriginalDataTableInPin) || (0 == OriginalDataTableInPin->LinkedTo.Num() && nullptr == Table))
	{
		CompilerContext.MessageLog.Error(*LOCTEXT("FindDataTableRowByPredicateNoDataTable_Error", "FindDataTableRowByPredicate must have a DataTable specified.").ToString(), this);
		// we break exec links so this is the only error we get
		BreakAllNodeLinks();
		return;
	}

	// FUNCTION NODE
	const FName FunctionName = GET_FUNCTION_NAME_CHECKED(UDataTableFL, FindDataTableRowByPredicate);
	UK2Node_CallFunction* FindDataTableRowByPredicateFunction = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	if (!FindDataTableRowByPredicateFunction)
	{
		CompilerContext.MessageLog.Error(*LOCTEXT("InvalidFunctionName", "Property connected may not be valid. @@").ToString(), this);
		return;
	}
	
	FindDataTableRowByPredicateFunction->FunctionReference.SetExternalMember(FunctionName, UDataTableFL::StaticClass());
	FindDataTableRowByPredicateFunction->AllocateDefaultPins();
	CompilerContext.MovePinLinksToIntermediate(*GetExecPin(), *(FindDataTableRowByPredicateFunction->GetExecPin()));

	// Connect the input of our GetDataTableRow to the Input of our Function pin
	UEdGraphPin* DataTableInPin = FindDataTableRowByPredicateFunction->FindPinChecked(DataTableRowBaseNodeHelper::DataTablePinName);
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

	// Get some pins to work with
	UEdGraphPin* OriginalOutRowPin = FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue);
	UEdGraphPin* FunctionOutRowPin = FindDataTableRowByPredicateFunction->FindPinChecked(DataTableRowBaseNodeHelper::RowPinName);
	UEdGraphPin* FunctionReturnPin = FindDataTableRowByPredicateFunction->FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue);
	UEdGraphPin* FunctionThenPin = FindDataTableRowByPredicateFunction->GetThenPin();

	// Set the type of the OutRow pin on this expanded mode to match original
	FunctionOutRowPin->PinType = OriginalOutRowPin->PinType;
	FunctionOutRowPin->PinType.PinSubCategoryObject = OriginalOutRowPin->PinType.PinSubCategoryObject;

	UEdGraphPin* FunctionPredicatePin = FindDataTableRowByPredicateFunction->FindPinChecked(FindDataTableRowByPredicateHelper::PredicatePinName);
	FunctionPredicatePin->PinType.bIsConst = true;
	FunctionPredicatePin->PinType.bIsReference = true;
	FProperty* Property = GetClass()->FindPropertyByName("Delegate");
	const FDelegateProperty* DelegateProperty = CastField<FDelegateProperty>(Property);
	FMemberReference::FillSimpleMemberReference<UFunction>(DelegateProperty->SignatureFunction, FunctionPredicatePin->PinType.PinSubCategoryMemberReference);
	
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
	CompilerContext.MovePinLinksToIntermediate(*GetPredicatePin(), *FunctionPredicatePin);

	BreakAllNodeLinks();
}

FSlateIcon UK2Node_FindDataTableRowByPredicate::GetIconAndTint(FLinearColor& OutColor) const
{
	OutColor = GetNodeTitleColor();
	static FSlateIcon Icon("EditorStyle", "Kismet.AllClasses.FunctionIcon");
	return Icon;
}

void UK2Node_FindDataTableRowByPredicate::GetPinHoverText(const UEdGraphPin& Pin, FString& HoverTextOut) const
{
	Super::GetPinHoverText(Pin, HoverTextOut);

	if(Pin.PinName == DataTableRowBaseNodeHelper::DataTablePinName)
	{
		HoverTextOut = "The data table to traverse and test each row against the predicate.";
	}
	else if(Pin.PinFriendlyName.EqualTo(FText::FromName(DataTableRowBaseNodeHelper::RowPinName)))
	{
		HoverTextOut = "The row found using the predicate.";
	}
	else if(Pin.PinName == FindDataTableRowByPredicateHelper::RowNotFoundPinName)
	{
		HoverTextOut = "Pin fired if the row was not found.";
	}
	else if(Pin.PinFriendlyName.EqualTo(FText::FromName(FindDataTableRowByPredicateHelper::RowFoundPinName)))
	{
		HoverTextOut = "Pin fired if the row was found.";
	}
	else if(Pin.PinName == FindDataTableRowByPredicateHelper::PredicatePinName)
	{
		HoverTextOut = "The predicate used to find a row. A predicate is a function that takes in an DataTableRowObject and returns a bool.";
	}
}

void UK2Node_FindDataTableRowByPredicate::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	const UClass* ActionKey = GetClass();

	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);

		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

FText UK2Node_FindDataTableRowByPredicate::GetMenuCategory() const
{
	return LOCTEXT("FindDataTableRowByPredicate_MenuCategory", "Data Table Function Library");
}

UEdGraphPin* UK2Node_FindDataTableRowByPredicate::GetThenPin() const
{
	UEdGraphPin* Pin = FindPinChecked(UEdGraphSchema_K2::PN_Then);
	check(Pin->Direction == EGPD_Output);
	return Pin;
}

UEdGraphPin* UK2Node_FindDataTableRowByPredicate::GetRowNotFoundPin() const
{
	UEdGraphPin* Pin = FindPinChecked(FindDataTableRowByPredicateHelper::RowNotFoundPinName);
	check(Pin->Direction == EGPD_Output);
	return Pin;
}

UEdGraphPin* UK2Node_FindDataTableRowByPredicate::GetPredicatePin() const
{
	UEdGraphPin* Pin = FindPinChecked(FindDataTableRowByPredicateHelper::PredicatePinName);
	check(Pin->Direction == EGPD_Input);
	return Pin;
}

#undef LOCTEXT_NAMESPACE
