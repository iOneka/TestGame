// Copyright 2022 Just2Devs. All Rights Reserved.

#include "K2Node_FindDataTableRowsByPredicate.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "K2Node_CallFunction.h"
#include "K2Node_IfThenElse.h"
#include "KismetCompiler.h"
#include "Engine/MemberReference.h"

#define LOCTEXT_NAMESPACE "K2Node_FindDataTableRowsByPredicate"

namespace FindDataTableRowsByPredicateHelper
{
	const FName RowFoundPinName = "Row Found";
	const FName RowNotFoundPinName = "Row Not Found";
	const FName PredicatePinName = "Predicate";
	const FName OutRowsPinName = "OutRows";
}

void UK2Node_FindDataTableRowsByPredicate::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();
	
	// Add execution pins
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
	UEdGraphPin* RowFoundPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);
	RowFoundPin->PinFriendlyName = LOCTEXT("GetDataTableRow Row Found Exec pin", "Row Found");
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, FindDataTableRowsByPredicateHelper::RowNotFoundPinName);

	// Add DataTable pin
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, UDataTable::StaticClass(), DataTableRowBaseNodeHelper::DataTablePinName);

	// Add predicate pin
	FCreatePinParams PinParams;
	PinParams.bIsReference = true;
	PinParams.bIsConst = true;
	UEdGraphPin* PredicatePin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Delegate, FindDataTableRowsByPredicateHelper::PredicatePinName, PinParams);
	FProperty* Property = GetClass()->FindPropertyByName("Delegate");
	const FDelegateProperty* DelegateProperty = CastField<FDelegateProperty>(Property);
	FMemberReference::FillSimpleMemberReference<UFunction>(DelegateProperty->SignatureFunction, PredicatePin->PinType.PinSubCategoryMemberReference);
	
	// Result pin
	FCreatePinParams ArrayPinParams;
	ArrayPinParams.ContainerType = EPinContainerType::Array;
	UEdGraphPin* ResultPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Wildcard, UEdGraphSchema_K2::PN_ReturnValue, ArrayPinParams);
	ResultPin->PinFriendlyName = FText::FromName(FindDataTableRowsByPredicateHelper::OutRowsPinName);
	
	Super::AllocateDefaultPins();
}

FText UK2Node_FindDataTableRowsByPredicate::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("FindDataTableRowsByPredicate_NodeTitle", "Find Data Table Rows By Predicate");
}

FText UK2Node_FindDataTableRowsByPredicate::GetTooltipText() const
{
	return LOCTEXT("FindDataTableRowsByPredicate_Tooltip", "Find all the rows in the data table that match a predicate. A predicate is a function that returns true or false for each row this function loops through.\nEssentially the predicate asks the question of is this the row you wanted? If yes then return true otherwise return false.\nThe predicate must be a function that takes in a DataTableRowObject and return a boolean. You can create it yourself or use the create event node to create the function for you.\nInside the predicate function use the node GetAsDataTableRow to convert the data table row object into the data table struct.\nIMPORTANT: The predicate function must name the parameters specifically RowObject and ReturnValue. If the names are any different the function won't be considered compatible with the predicate pin." );
}

void UK2Node_FindDataTableRowsByPredicate::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);
	
	UEdGraphPin* OriginalDataTableInPin = GetDataTablePin();
	UDataTable* Table = (OriginalDataTableInPin != nullptr) ? Cast<UDataTable>(OriginalDataTableInPin->DefaultObject) : nullptr;
	if ((nullptr == OriginalDataTableInPin) || (0 == OriginalDataTableInPin->LinkedTo.Num() && nullptr == Table))
	{
		CompilerContext.MessageLog.Error(*LOCTEXT("FindDataTableRowsByPredicateNoDataTable_Error", "FindDataTableRowsByPredicate must have a DataTable specified.").ToString(), this);
		// we break exec links so this is the only error we get
		BreakAllNodeLinks();
		return;
	}

	// FUNCTION NODE
	const FName FunctionName = GET_FUNCTION_NAME_CHECKED(UDataTableFL, FindDataTableRowsByPredicate);
	UK2Node_CallFunction* FindDataTableRowsByPredicateFunction = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	if (!FindDataTableRowsByPredicateFunction)
	{
		CompilerContext.MessageLog.Error(*LOCTEXT("InvalidFunctionName", "Property connected may not be valid. @@").ToString(), this);
		return;
	}
	
	FindDataTableRowsByPredicateFunction->FunctionReference.SetExternalMember(FunctionName, UDataTableFL::StaticClass());
	FindDataTableRowsByPredicateFunction->AllocateDefaultPins();
	CompilerContext.MovePinLinksToIntermediate(*GetExecPin(), *(FindDataTableRowsByPredicateFunction->GetExecPin()));

	// Connect the input of our GetDataTableRow to the Input of our Function pin
	UEdGraphPin* DataTableInPin = FindDataTableRowsByPredicateFunction->FindPinChecked(DataTableRowBaseNodeHelper::DataTablePinName);
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
	UEdGraphPin* FunctionOutRowPin = FindDataTableRowsByPredicateFunction->FindPinChecked(FindDataTableRowsByPredicateHelper::OutRowsPinName);
	UEdGraphPin* FunctionReturnPin = FindDataTableRowsByPredicateFunction->FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue);
	UEdGraphPin* FunctionThenPin = FindDataTableRowsByPredicateFunction->GetThenPin();

	// Set the type of the OutRow pin on this expanded mode to match original
	FunctionOutRowPin->PinType = OriginalOutRowPin->PinType;
	FunctionOutRowPin->PinType.PinSubCategoryObject = OriginalOutRowPin->PinType.PinSubCategoryObject;

	UEdGraphPin* FunctionPredicatePin = FindDataTableRowsByPredicateFunction->FindPinChecked(FindDataTableRowsByPredicateHelper::PredicatePinName);
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

FSlateIcon UK2Node_FindDataTableRowsByPredicate::GetIconAndTint(FLinearColor& OutColor) const
{
	OutColor = GetNodeTitleColor();
	static FSlateIcon Icon("EditorStyle", "Kismet.AllClasses.FunctionIcon");
	return Icon;
}

void UK2Node_FindDataTableRowsByPredicate::GetPinHoverText(const UEdGraphPin& Pin, FString& HoverTextOut) const
{
	Super::GetPinHoverText(Pin, HoverTextOut);
	
	if(Pin.PinName == DataTableRowBaseNodeHelper::DataTablePinName)
	{
		HoverTextOut = "The data table to traverse and test each row against the predicate.";
	}
	else if(Pin.PinFriendlyName.EqualTo(FText::FromName(FindDataTableRowsByPredicateHelper::OutRowsPinName)))
	{
		HoverTextOut = "The rows found using the predicate.";
	}
	else if(Pin.PinName == FindDataTableRowsByPredicateHelper::RowNotFoundPinName)
	{
		HoverTextOut = "Pin fired if no row was found.";
	}
	else if(Pin.PinFriendlyName.EqualTo(FText::FromName(FindDataTableRowsByPredicateHelper::RowFoundPinName)))
	{
		HoverTextOut = "Pin fired if rows were found.";
	}
	else if(Pin.PinName == FindDataTableRowsByPredicateHelper::PredicatePinName)
	{
		HoverTextOut = "The predicate used to find all the rows. A predicate is a function that takes in an DataTableRowObject and returns a bool.";
	}
}

void UK2Node_FindDataTableRowsByPredicate::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	const UClass* ActionKey = GetClass();

	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);

		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

FText UK2Node_FindDataTableRowsByPredicate::GetMenuCategory() const
{
	return LOCTEXT("FindDataTableRowsByPredicate_MenuCategory", "Data Table Function Library");
}

UEdGraphPin* UK2Node_FindDataTableRowsByPredicate::GetThenPin() const
{
	UEdGraphPin* Pin = FindPinChecked(UEdGraphSchema_K2::PN_Then);
	check(Pin->Direction == EGPD_Output);
	return Pin;
}

UEdGraphPin* UK2Node_FindDataTableRowsByPredicate::GetRowNotFoundPin() const
{
	UEdGraphPin* Pin = FindPinChecked(FindDataTableRowsByPredicateHelper::RowNotFoundPinName);
	check(Pin->Direction == EGPD_Output);
	return Pin;
}

UEdGraphPin* UK2Node_FindDataTableRowsByPredicate::GetPredicatePin() const
{
	UEdGraphPin* Pin = FindPinChecked(FindDataTableRowsByPredicateHelper::PredicatePinName);
	check(Pin->Direction == EGPD_Input);
	return Pin;
}

#undef LOCTEXT_NAMESPACE
