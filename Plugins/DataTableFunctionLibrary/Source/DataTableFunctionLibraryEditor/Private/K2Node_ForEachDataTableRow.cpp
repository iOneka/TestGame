// Copyright 2022 Just2Devs. All Rights Reserved.

#include "K2Node_ForEachDataTableRow.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "DataTableEditorUtils.h"
#include "DataTableFL.h"
#include "K2Node_AssignmentStatement.h"
#include "K2Node_CallFunction.h"
#include "K2Node_DataTableHelper.h"
#include "K2Node_ExecutionSequence.h"
#include "K2Node_IfThenElse.h"
#include "K2Node_TemporaryVariable.h"
#include "KismetCompiler.h"
#include "Kismet/KismetMathLibrary.h"

#define LOCTEXT_NAMESPACE "K2Node_FindDataTableRowByProperty"

namespace ForEachDataTableRowHelper
{
	const FName ExecPinName = "Exec";
	const FName DataTablePinName = "DataTable";
	const FName LoopBodyPinName = "Loop Body";
	const FName RowPinName = "Row";
	const FName CompletedPinName = "Completed";
	const FName IndexPinName = "Index";
}

struct FDTLForExpandNodeHelper
{
	UEdGraphPin* StartLoopExecInPin;
	UEdGraphPin* InsideLoopExecOutPin;
	UEdGraphPin* LoopCompleteOutExecPin;

	UEdGraphPin* ArrayIndexOutPin;
	UEdGraphPin* LoopCounterOutPin;
	// for(LoopCounter = 0; LoopCounter < LoopCounterLimit; ++LoopCounter)
	UEdGraphPin* LoopCounterLimitInPin;

	FDTLForExpandNodeHelper()
		: StartLoopExecInPin(nullptr)
		, InsideLoopExecOutPin(nullptr)
		, LoopCompleteOutExecPin(nullptr)
		, ArrayIndexOutPin(nullptr)
		, LoopCounterOutPin(nullptr)
		, LoopCounterLimitInPin(nullptr)
	{ }

	bool BuildLoop(UK2Node* Node, FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph, UEdGraphPin* DataTablePin)
	{
		const UEdGraphSchema_K2* Schema = CompilerContext.GetSchema();
		check(Node && SourceGraph && Schema);

		bool bResult = true;

		// Create int Loop Counter
		UK2Node_TemporaryVariable* LoopCounterNode = CompilerContext.SpawnIntermediateNode<UK2Node_TemporaryVariable>(Node, SourceGraph);
		LoopCounterNode->VariableType.PinCategory = UEdGraphSchema_K2::PC_Int;
		LoopCounterNode->AllocateDefaultPins();
		LoopCounterOutPin = LoopCounterNode->GetVariablePin();
		check(LoopCounterOutPin);

		// Initialize loop counter
		UK2Node_AssignmentStatement* LoopCounterInitialize = CompilerContext.SpawnIntermediateNode<UK2Node_AssignmentStatement>(Node, SourceGraph);
		LoopCounterInitialize->AllocateDefaultPins();
		LoopCounterInitialize->GetValuePin()->DefaultValue = TEXT("0");
		bResult &= Schema->TryCreateConnection(LoopCounterOutPin, LoopCounterInitialize->GetVariablePin());
		StartLoopExecInPin = LoopCounterInitialize->GetExecPin();
		check(StartLoopExecInPin);

		// Create int Array Index
		UK2Node_TemporaryVariable* ArrayIndexNode = CompilerContext.SpawnIntermediateNode<UK2Node_TemporaryVariable>(Node, SourceGraph);
		ArrayIndexNode->VariableType.PinCategory = UEdGraphSchema_K2::PC_Int;
		ArrayIndexNode->AllocateDefaultPins();
		ArrayIndexOutPin = ArrayIndexNode->GetVariablePin();
		check(ArrayIndexOutPin);

		// Initialize array index
		UK2Node_AssignmentStatement* ArrayIndexInitialize = CompilerContext.SpawnIntermediateNode<UK2Node_AssignmentStatement>(Node, SourceGraph);
		ArrayIndexInitialize->AllocateDefaultPins();
		ArrayIndexInitialize->GetValuePin()->DefaultValue = TEXT("0");
		bResult &= Schema->TryCreateConnection(ArrayIndexOutPin, ArrayIndexInitialize->GetVariablePin());
		bResult &= Schema->TryCreateConnection(LoopCounterInitialize->GetThenPin(), ArrayIndexInitialize->GetExecPin());

		// Do loop branch
		UK2Node_IfThenElse* Branch = CompilerContext.SpawnIntermediateNode<UK2Node_IfThenElse>(Node, SourceGraph);
		Branch->AllocateDefaultPins();
		bResult &= Schema->TryCreateConnection(ArrayIndexInitialize->GetThenPin(), Branch->GetExecPin());
		LoopCompleteOutExecPin = Branch->GetElsePin();
		check(LoopCompleteOutExecPin);

		// Do loop condition
		UK2Node_CallFunction* Condition = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(Node, SourceGraph); 
		Condition->SetFromFunction(UKismetMathLibrary::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UKismetMathLibrary, Less_IntInt)));
		Condition->AllocateDefaultPins();
		bResult &= Schema->TryCreateConnection(Condition->GetReturnValuePin(), Branch->GetConditionPin());
		bResult &= Schema->TryCreateConnection(Condition->FindPinChecked(TEXT("A")), LoopCounterOutPin);
		LoopCounterLimitInPin = Condition->FindPinChecked(TEXT("B"));
		check(LoopCounterLimitInPin);

		// Validate the row index
		UK2Node_CallFunction* ValidateRowIndex = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(Node, SourceGraph); 
		ValidateRowIndex->SetFromFunction(UDataTableFL::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UDataTableFL, ValidateRowIndex)));
		ValidateRowIndex->AllocateDefaultPins();
		CompilerContext.CopyPinLinksToIntermediate(*DataTablePin, *ValidateRowIndex->FindPinChecked(ForEachDataTableRowHelper::DataTablePinName, EGPD_Input));
		bResult &= Schema->TryCreateConnection(ValidateRowIndex->FindPinChecked(ForEachDataTableRowHelper::IndexPinName), LoopCounterOutPin);

		// Array Index assigned
		UK2Node_AssignmentStatement* ArrayIndexAssign = CompilerContext.SpawnIntermediateNode<UK2Node_AssignmentStatement>(Node, SourceGraph);
		ArrayIndexAssign->AllocateDefaultPins();
		bResult &= Schema->TryCreateConnection(Branch->GetThenPin(), ArrayIndexAssign->GetExecPin());
		bResult &= Schema->TryCreateConnection(ArrayIndexAssign->GetVariablePin(), ArrayIndexOutPin);
		bResult &= Schema->TryCreateConnection(ArrayIndexAssign->GetValuePin(), ValidateRowIndex->GetReturnValuePin());

		// body sequence
		UK2Node_ExecutionSequence* Sequence = CompilerContext.SpawnIntermediateNode<UK2Node_ExecutionSequence>(Node, SourceGraph);
		Sequence->AllocateDefaultPins();
		bResult &= Schema->TryCreateConnection(ArrayIndexAssign->GetThenPin(), Sequence->GetExecPin());
		InsideLoopExecOutPin = Sequence->GetThenPinGivenIndex(0);
		check(InsideLoopExecOutPin);

		// Loop Counter increment
		UK2Node_CallFunction* Increment = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(Node, SourceGraph); 
		Increment->SetFromFunction(UKismetMathLibrary::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UKismetMathLibrary, Add_IntInt)));
		Increment->AllocateDefaultPins();
		bResult &= Schema->TryCreateConnection(Increment->FindPinChecked(TEXT("A")), LoopCounterOutPin);
		Increment->FindPinChecked(TEXT("B"))->DefaultValue = TEXT("1");

		// Loop Counter assigned
		UK2Node_AssignmentStatement* LoopCounterAssign = CompilerContext.SpawnIntermediateNode<UK2Node_AssignmentStatement>(Node, SourceGraph);
		LoopCounterAssign->AllocateDefaultPins();
		bResult &= Schema->TryCreateConnection(LoopCounterAssign->GetExecPin(), Sequence->GetThenPinGivenIndex(1));
		bResult &= Schema->TryCreateConnection(LoopCounterAssign->GetVariablePin(), LoopCounterOutPin);
		bResult &= Schema->TryCreateConnection(LoopCounterAssign->GetValuePin(), Increment->GetReturnValuePin());
		bResult &= Schema->TryCreateConnection(LoopCounterAssign->GetThenPin(), Branch->GetExecPin());

		return bResult;
	}
};

void UK2Node_ForEachDataTableRow::AllocateDefaultPins()
{
	// Add Execution pin
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute)->PinFriendlyName = FText::FromName(ForEachDataTableRowHelper::ExecPinName);
	
	// Add DataTable pin
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, UDataTable::StaticClass(), ForEachDataTableRowHelper::DataTablePinName);
	
	// Created loop body pin
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, ForEachDataTableRowHelper::LoopBodyPinName);
	
	// Result pin
	UEdGraphPin* ResultPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Wildcard, UEdGraphSchema_K2::PN_ReturnValue);
	ResultPin->PinFriendlyName = FText::FromName(ForEachDataTableRowHelper::RowPinName);

	// Created Completed pin 
	if(UEdGraphPin* CompletedPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then))
	{
		CompletedPin->PinFriendlyName = LOCTEXT("Completed", "Completed");
	}
	
	Super::AllocateDefaultPins();
}

FText UK2Node_ForEachDataTableRow::GetTooltipText() const
{
	return LOCTEXT("ForEachDataTableRow_Tooltip", "Loops through each of the rows inside the data table.");
}

FText UK2Node_ForEachDataTableRow::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("ForEachDataTableRow_NodeTitle", "For Each Data Table Row");
}

FSlateIcon UK2Node_ForEachDataTableRow::GetIconAndTint(FLinearColor& OutColor) const
{
	static FSlateIcon Icon("EditorStyle", "GraphEditor.Macro.Loop_16x");
	return Icon;
}

void UK2Node_ForEachDataTableRow::GetPinHoverText(const UEdGraphPin& Pin, FString& HoverTextOut) const
{
	Super::GetPinHoverText(Pin, HoverTextOut);

	if(Pin.PinFriendlyName.EqualTo(FText::FromName(ForEachDataTableRowHelper::ExecPinName)))
	{
		HoverTextOut = "Execute the node";
	}
	else if(Pin.PinName == ForEachDataTableRowHelper::DataTablePinName)
	{
		HoverTextOut = "The data table the node loops through.";
	}
	else if(Pin.PinName == ForEachDataTableRowHelper::LoopBodyPinName)
	{
		HoverTextOut = "The loop body runs for each row found in the data table.";
	}
	else if(Pin.PinFriendlyName.EqualTo(FText::FromName(ForEachDataTableRowHelper::RowPinName)))
	{
		HoverTextOut = "The current row from the data table.";
	}
	else if(Pin.PinFriendlyName.EqualTo(FText::FromName("Completed")))
	{
		HoverTextOut = "The loop finishes and runs this pin.";
	}
}

void UK2Node_ForEachDataTableRow::ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins)
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

bool UK2Node_ForEachDataTableRow::IsConnectionDisallowed(const UEdGraphPin* MyPin, const UEdGraphPin* OtherPin, FString& OutReason) const
{
	if (MyPin == GetResultPin() && MyPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Wildcard)
	{
		bool bDisallowed = true;
		if (OtherPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Struct)
		{
			if (const UScriptStruct* ConnectionType = Cast<UScriptStruct>(OtherPin->PinType.PinSubCategoryObject.Get()))
			{
				bDisallowed = !FDataTableEditorUtils::IsValidTableStruct(ConnectionType);
			}
		}
		else if (OtherPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Wildcard)
		{
			bDisallowed = false;
		}

		if (bDisallowed)
		{
			OutReason = TEXT("Must be a struct that can be used in a DataTable");
		}
		return bDisallowed;
	}
	return false;
}

void UK2Node_ForEachDataTableRow::EarlyValidation(FCompilerResultsLog& MessageLog) const
{
	Super::EarlyValidation(MessageLog);

	const UEdGraphPin* DataTablePin = GetDataTablePin();
	if (!DataTablePin)
	{
		MessageLog.Error(*LOCTEXT("MissingPins", "Missing pins in @@").ToString(), this);
		return;
	}

	if (DataTablePin->LinkedTo.Num() == 0)
	{
		const UDataTable* DataTable = Cast<UDataTable>(DataTablePin->DefaultObject);
		if (!DataTable)
		{
			MessageLog.Error(*LOCTEXT("NoDataTable", "No DataTable in @@").ToString(), this);
			return;
		}
	}
}

void UK2Node_ForEachDataTableRow::PreloadRequiredAssets()
{
	if (const UEdGraphPin* DataTablePin = GetDataTablePin())
	{
		if (UDataTable* DataTable = Cast<UDataTable>(DataTablePin->DefaultObject))
		{
			// make sure to properly load the data-table object so that we can 
			// farm the "RowStruct" property from it (below, in GetDataTableRowStructType)
			PreloadObject(DataTable);
		}
	}
	return Super::PreloadRequiredAssets();
}

void UK2Node_ForEachDataTableRow::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);

	const UEdGraphSchema_K2* Schema = CompilerContext.GetSchema();
	
	FDTLForExpandNodeHelper ForLoop;
	if (!ForLoop.BuildLoop(this, CompilerContext, SourceGraph, GetDataTablePin()))
	{
		CompilerContext.MessageLog.Error(*NSLOCTEXT("K2Node", "ForEachDataTableRow_ForError", "For Expand error in @@").ToString(), this);
	}

	// Get number of rows
	UK2Node_CallFunction* GetNumChildrenFunction = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	GetNumChildrenFunction->SetFromFunction(UDataTableFL::StaticClass()->FindFunctionByName(GET_MEMBER_NAME_CHECKED(UDataTableFL, GetDataTableSize)));
	GetNumChildrenFunction->AllocateDefaultPins();
	CompilerContext.CopyPinLinksToIntermediate(*GetDataTablePin(), *GetNumChildrenFunction->FindPinChecked(ForEachDataTableRowHelper::DataTablePinName, EGPD_Input));
	bool bResult = Schema->TryCreateConnection(GetNumChildrenFunction->FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue), ForLoop.LoopCounterLimitInPin);

	// Get row at index
	UK2Node_CallFunction* GetDataTableRowAtIndex = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	GetDataTableRowAtIndex->SetFromFunction(UDataTableFL::StaticClass()->FindFunctionByName(GET_MEMBER_NAME_CHECKED(UDataTableFL, GetDataTableRowAtIndex)));
	GetDataTableRowAtIndex->AllocateDefaultPins();
	CompilerContext.CopyPinLinksToIntermediate(*GetDataTablePin(), *GetDataTableRowAtIndex->FindPinChecked(ForEachDataTableRowHelper::DataTablePinName, EGPD_Input));
	bResult &= Schema->TryCreateConnection(GetDataTableRowAtIndex->FindPinChecked(ForEachDataTableRowHelper::IndexPinName), ForLoop.ArrayIndexOutPin);

	// Set the internal function pin type to the result pin derived type
	UEdGraphPin* FunctionOutRowPin = GetDataTableRowAtIndex->FindPinChecked(ForEachDataTableRowHelper::RowPinName);
	FunctionOutRowPin->PinType = GetResultPin()->PinType;
	FunctionOutRowPin->PinType.PinSubCategoryObject = GetResultPin()->PinType.PinSubCategoryObject;
	
	CompilerContext.MovePinLinksToIntermediate(*GetExecPin(), *ForLoop.StartLoopExecInPin);
	CompilerContext.MovePinLinksToIntermediate(*FindPinChecked(UEdGraphSchema_K2::PN_Then), *ForLoop.LoopCompleteOutExecPin);
	CompilerContext.MovePinLinksToIntermediate(*FindPinChecked(ForEachDataTableRowHelper::LoopBodyPinName), *ForLoop.InsideLoopExecOutPin);
	// Schema->TryCreateConnection(GetDataTableRowAtIndex->FindPinChecked(ForEachDataTableRowHelper::RowPinName), GetResultPin());
	CompilerContext.MovePinLinksToIntermediate(*GetResultPin(), *GetDataTableRowAtIndex->FindPinChecked(ForEachDataTableRowHelper::RowPinName));
	
	if (!bResult)
	{
		CompilerContext.MessageLog.Error(*NSLOCTEXT("K2Node", "ForEachDataTableRow_ExpandError", "Expand error in @@").ToString(), this);
	}
	
	BreakAllNodeLinks();
}

void UK2Node_ForEachDataTableRow::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	const UClass* Action = GetClass();

	if (ActionRegistrar.IsOpenForRegistration(Action))
	{
		UBlueprintNodeSpawner* Spawner = UBlueprintNodeSpawner::Create(GetClass());
		check(Spawner != nullptr);

		ActionRegistrar.AddBlueprintAction(Action, Spawner);
	}	
}

FText UK2Node_ForEachDataTableRow::GetMenuCategory() const
{
	return LOCTEXT("ForEachDataTableRow_MenuCategory", "Data Table Function Library");
}

UEdGraphPin* UK2Node_ForEachDataTableRow::GetResultPin() const
{
	UEdGraphPin* Pin = FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue);
	check(Pin->Direction == EGPD_Output);
	return Pin;
}

UEdGraphPin* UK2Node_ForEachDataTableRow::GetDataTablePin(const TArray<UEdGraphPin*>* InPinsToSearch) const
{
	const TArray<UEdGraphPin*>* PinsToSearch = InPinsToSearch ? InPinsToSearch : &Pins;

	UEdGraphPin* Pin = nullptr;
	for (UEdGraphPin* TestPin : *PinsToSearch)
	{
		if (TestPin && TestPin->PinName == ForEachDataTableRowHelper::DataTablePinName)
		{
			Pin = TestPin;
			break;
		}
	}
	check(Pin == nullptr || Pin->Direction == EGPD_Input);
	return Pin;
}

void UK2Node_ForEachDataTableRow::NotifyPinConnectionListChanged(UEdGraphPin* Pin)
{
	Super::NotifyPinConnectionListChanged(Pin);

	FK2Node_DataTableHelper::NotifyPinConnectionListChanged(Pin, GetDataTablePin(), GetResultPin(), GetSchema());
}

void UK2Node_ForEachDataTableRow::PinDefaultValueChanged(UEdGraphPin* Pin)
{
	if (Pin && Pin->PinName == ForEachDataTableRowHelper::DataTablePinName)
	{
		FK2Node_DataTableHelper::RefreshOutputPinType(GetDataTablePin(), GetResultPin(), GetSchema());
	}
}

void UK2Node_ForEachDataTableRow::PostReconstructNode()
{
	Super::PostReconstructNode();
	FK2Node_DataTableHelper::RefreshOutputPinType(GetDataTablePin(), GetResultPin(), GetSchema());
}

#undef LOCTEXT_NAMESPACE
