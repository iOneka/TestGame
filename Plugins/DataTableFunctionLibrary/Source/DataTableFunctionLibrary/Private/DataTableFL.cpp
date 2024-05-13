// Copyright 2022 Just2Devs. All Rights Reserved.

#include "DataTableFL.h"
#if WITH_EDITOR
#include "DataTableEditorUtils.h"
#endif
#include "DataTableFunctionLog.h"
#include "UObject/TextProperty.h"
#include "Kismet/DataTableFunctionLibrary.h"
#include "UObject/SavePackage.h"
#include "Launch/Resources/Version.h"
#include "CoreUObject/Public/Misc/PackageName.h"

int32 UDataTableFL::GetDataTableSize(UDataTable* DataTable)
{
	if (!DataTable)
	{
		PrintW(DTFLog, false, 0, "Data table is not valid.");
		return -1;
	}

	TArray<FName> RowNames;
	UDataTableFunctionLibrary::GetDataTableRowNames(DataTable, RowNames);
	return RowNames.Num();
}

bool UDataTableFL::EmptyDataTable(UDataTable* DataTable, bool bSave)
{
#if WITH_EDITOR
	if(!DataTable)
	{
		PrintE(DTFLog, false, 0, "Data Table is not valid.");
		return false;
	}

	// Get all the data table row names
	const TArray<FName> RowNames = DataTable->GetRowNames();
	
	// The data table is already empty
	if(RowNames.Num() == 0)
	{
		Print(DTFLog, false, 0, "The data table is already empty.");
		return false;
	}

	// Remove all the data table rows
	for (const FName RowName : RowNames)
	{
		DataTable->RemoveRow(RowName);
	}

	// Save data table
	if(bSave) SaveDataTable(DataTable);
	
	// Update data table view
	FDataTableEditorUtils::FDataTableEditorManager::Get().PostChange(DataTable, FDataTableEditorUtils::EDataTableChangeInfo::RowList);
	
	return true;
#else
	PrintE(DTFLog, false, 0, "Empty Data Table is an editor only function. Do not vall at runtime in package.");
	return false;
#endif
}

bool UDataTableFL::SaveDataTable(UDataTable* DataTable)
{
#if WITH_EDITOR
	if(!DataTable)
	{
		PrintE(DTFLog, false, 0, "Data table is not valid.");
		return false;
	}
	
	bool bMarkedDirty = DataTable->MarkPackageDirty();
	const FString PackageName = DataTable->GetPathName();
	UPackage* Package = LoadPackage(nullptr, *PackageName, LOAD_None);
	const FString PackageFileName = FPackageName::LongPackageNameToFilename(Package->GetName(), FPackageName::GetAssetPackageExtension());
#if ENGINE_MAJOR_VERSION == 5
	FSavePackageArgs SavePackageArgs;
	SavePackageArgs.TopLevelFlags = EObjectFlags::RF_Public | EObjectFlags::RF_Standalone;
	SavePackageArgs.Error = GError;
	SavePackageArgs.SaveFlags = SAVE_NoError;
	SavePackageArgs.bWarnOfLongFilename = true;
	return UPackage::SavePackage(Package, DataTable, *PackageFileName, SavePackageArgs);
#else
	return UPackage::SavePackage(Package, DataTable, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone, *PackageFileName, GError, nullptr, false, true, SAVE_NoError);
#endif
#else
	PrintE(DTFLog, false, 0, "Save Data Table is an editor only function. Do not call at runtime in package.");
	return false;
#endif
}

bool UDataTableFL::MoveDataTableRow(UDataTable* DataTable, FName RowToMove, FName ReferenceRow, EDTFRowPosition RowPosition, bool bSave)
{
#if WITH_EDITOR
	if(!DataTable)
	{
		PrintE(DTFLog, false, 0, "Data table is not valid.");
		return false;
	}

	if(RowToMove == NAME_None)
	{
		PrintW(DTFLog, false, 0, "Cannot move data table row with row to move name equal to none.");
		return false;
	}

	if(ReferenceRow == NAME_None)
	{
		PrintE(DTFLog, false, 0, "Cannot move data table row with reference row name equal to none");
		return false;
	}

	if(!DataTable->GetRowNames().Contains(RowToMove))
	{
		PrintWF(DTFLog, false, 0, TEXT("Data table %s does not contain row to move %s"), *DataTable->GetName(), *RowToMove.ToString());
		return false;
	}

	if(!DataTable->GetRowNames().Contains(ReferenceRow))
	{
		PrintWF(DTFLog, false, 0, TEXT("Data table %s does not contain reference row %s"), *DataTable->GetName(), *ReferenceRow.ToString());
		return false;
	}

	// Get row names and indices for rows
	const TArray<FName> RowNames = DataTable->GetRowNames();
	const int32 RowToMoveIndex = RowNames.Find(RowToMove);
	const int32 ReferenceRowIndex = RowNames.Find(ReferenceRow);

	// Check one final time that the row indices are valid.
	if(ReferenceRowIndex != INDEX_NONE && RowToMoveIndex != INDEX_NONE)
	{
		// Get the row move direction based on whether the row to move is before or after the reference row
		FDataTableEditorUtils::ERowMoveDirection RowMoveDirection;
		if(RowToMoveIndex < ReferenceRowIndex) RowMoveDirection = FDataTableEditorUtils::ERowMoveDirection::Down;
		else RowMoveDirection = FDataTableEditorUtils::ERowMoveDirection::Up;
		
		// Depending on the moving direction and the row positioning we want to either have an extra offset of 0 or 1
		int32 IndexOffset = 0;
		if(RowMoveDirection == FDataTableEditorUtils::ERowMoveDirection::Up && RowPosition == EDTFRowPosition::After) IndexOffset = 1;
		if(RowMoveDirection == FDataTableEditorUtils::ERowMoveDirection::Down && RowPosition == EDTFRowPosition::Before) IndexOffset = 1;

		// Calculate number of rows to move row by subtracting the row names indices and subtracting the index offset
		int32 MoveBy = FMath::Abs(RowToMoveIndex - ReferenceRowIndex);
		MoveBy -= IndexOffset;

		// Actually move row
		FDataTableEditorUtils::MoveRow(DataTable, RowToMove, RowMoveDirection, MoveBy);

		// Save data table asset
		if(bSave) SaveDataTable(DataTable);
		
		// Update data table view
		FDataTableEditorUtils::FDataTableEditorManager::Get().PostChange(DataTable, FDataTableEditorUtils::EDataTableChangeInfo::RowList);
		
		return true;
	}

	PrintE(DTFLog, false, 0, "Could not get row to move or reference row index.");
	return false;
#else
	PrintE(DTFLog, false, 0, "Move Data Table Row is an editor only function. Do not call at runtime in package.");
	return false;
#endif
}

bool UDataTableFL::MoveDataTableRowAtIndex(UDataTable* DataTable, FName RowToMove, int32 Index, bool bSave)
{
#if WITH_EDITOR
	if(!DataTable)
	{
		PrintE(DTFLog, false, 0, "Data table is not valid.");
		return false;
	}

	if(RowToMove == NAME_None)
	{
		PrintW(DTFLog, false, 0, "Cannot move data table row with row to move name equal to none.");
		return false;
	}

	if(Index < 0)
	{
		PrintE(DTFLog, false, 0, "Cannot move data table row at negative index.");
		return false;
	}

	if(!DataTable->GetRowNames().Contains(RowToMove))
	{
		PrintWF(DTFLog, false, 0, TEXT("Data table %s does not contain row to move %s"), *DataTable->GetName(), *RowToMove.ToString());
		return false;
	}

	if(!DataTable->GetRowNames().IsValidIndex(Index))
	{
		PrintWF(DTFLog, false, 0, TEXT("Cannot move row %s at index %i since it does not exist."), *RowToMove.ToString(), Index);
	}

	// Get row names and indices for rows
	const TArray<FName> RowNames = DataTable->GetRowNames();
	const int32 RowToMoveIndex = RowNames.Find(RowToMove);
	const int32 ReferenceRowIndex = Index;

	// Check one final time that the row indices are valid.
	if(ReferenceRowIndex != INDEX_NONE && RowToMoveIndex != INDEX_NONE)
	{
		// Get the row move direction based on whether the row to move is before or after the reference row
		FDataTableEditorUtils::ERowMoveDirection RowMoveDirection;
		if(RowToMoveIndex < ReferenceRowIndex) RowMoveDirection = FDataTableEditorUtils::ERowMoveDirection::Down;
		else RowMoveDirection = FDataTableEditorUtils::ERowMoveDirection::Up;
		
		// Calculate number of rows to move row by subtracting the row names indices and subtracting the index offset
		const int32 MoveBy = FMath::Abs(RowToMoveIndex - ReferenceRowIndex);

		// Actually move row
		FDataTableEditorUtils::MoveRow(DataTable, RowToMove, RowMoveDirection, MoveBy);

		// Save data table asset
		if(bSave) SaveDataTable(DataTable);
		
		// Update data table view
		FDataTableEditorUtils::FDataTableEditorManager::Get().PostChange(DataTable, FDataTableEditorUtils::EDataTableChangeInfo::RowList);
		
		return true;
	}

	PrintE(DTFLog, false, 0, "Could not get row to move or reference row index.");
	return false;
#else
	PrintE(DTFLog, false, 0, "Move Data Table Row At Index is an editor only function. Do not call at runtime in package.");
	return false;
#endif
}

bool UDataTableFL::MoveDataTableRowByNumber(UDataTable* DataTable, FName RowToMove, int32 MoveBy, EDTFRowDirection Direction, bool bSave)
{
#if WITH_EDITOR
	if(!DataTable)
	{
		PrintE(DTFLog, false, 0, "Data table is not valid.");
		return false;
	}

	if(RowToMove == NAME_None)
	{
		PrintW(DTFLog, false, 0, "Cannot move data table row with row to move name equal to none.");
		return false;
	}

	if(MoveBy < 0)
	{
		PrintWF(DTFLog, false, 0, TEXT("Cannot move row by negative number %i."), MoveBy);
		return false;
	}
	
	if(!DataTable->GetRowNames().Contains(RowToMove))
	{
		PrintWF(DTFLog, false, 0, TEXT("Data table %s does not contain row to move %s"), *DataTable->GetName(), *RowToMove.ToString());
		return false;
	}

	// Get row names and indices for rows
	const TArray<FName> RowNames = DataTable->GetRowNames();
	const int32 RowToMoveIndex = RowNames.Find(RowToMove);

	// Check one final time that the row indices are valid.
	if(RowToMoveIndex != INDEX_NONE)
	{
		// Create row direction enum
		FDataTableEditorUtils::ERowMoveDirection RowMoveDirection;
		if(Direction == EDTFRowDirection::Up) RowMoveDirection = FDataTableEditorUtils::ERowMoveDirection::Up;
		else RowMoveDirection = FDataTableEditorUtils::ERowMoveDirection::Down;
		
		// Actually move row
		FDataTableEditorUtils::MoveRow(DataTable, RowToMove, RowMoveDirection, MoveBy);

		// Save data table asset
		if(bSave) SaveDataTable(DataTable);
		
		// Update data table view
		FDataTableEditorUtils::FDataTableEditorManager::Get().PostChange(DataTable, FDataTableEditorUtils::EDataTableChangeInfo::RowList);
		
		return true;
	}

	PrintE(DTFLog, false, 0, "Could not get row to move.");
	return false;
#else
	PrintE(DTFLog, false, 0, "Move Data Table Row By Number is an editor only function. Do not call at runtime in package.");
	return false;
#endif
}

TArray<FName> UDataTableFL::GetDataTableRowNames(const UDataTable* DataTable)
{
	if(!DataTable)
	{
		PrintE(DTFLog, false, 0, "Data Table is not valid.");
		return TArray<FName>();
	}

	return DataTable->GetRowNames();
}

FName UDataTableFL::GetValidDataTableRowName(const UDataTable* DataTable)
{
	// Base row name to add
	FName NewRowName = DataTableUtils::MakeValidName(TEXT("NewRow"));

	// Keep adding 1 to the postfix number if the name already exists
	while (DataTable->GetRowMap().Contains(NewRowName))
	{
		 NewRowName.SetNumber(NewRowName.GetNumber() + 1);
	}
	
	return NewRowName;
}

#pragma region AddDataTableRow
bool UDataTableFL::AddDataTableRow(UDataTable* DataTable, FTableRowBase Row, FName RowName, bool bSave)
{
	// This should never run since the function uses a custom thunk
	check(0);
	return false;
}

bool UDataTableFL::Generic_AddDataTableRow(UDataTable* DataTable, void* Row, FName RowName, bool bSave)
{
#if WITH_EDITOR
	if (!DataTable)
	{
		PrintW(DTFLog, false, 0, "Data table is not valid.");
		return false;
	}

	if (Row)
	{
		// Base row name to add
		const FName NewRowName = RowName == NAME_None || RowName == FName("") ? GetValidDataTableRowName(DataTable) : RowName;

		// Cast row to table row base
		const FTableRowBase* TableRowBase = static_cast<FTableRowBase*>(Row);

		// Add row
		DataTable->AddRow(NewRowName, *TableRowBase);

		// Save data table asset
		if(bSave) SaveDataTable(DataTable);
		
		// Update data table view
		FDataTableEditorUtils::FDataTableEditorManager::Get().PostChange(DataTable, FDataTableEditorUtils::EDataTableChangeInfo::RowList);
		
		return true;
	}

	PrintE(DTFLog, false, 0, "Row is not valid.");
	return false;

#else
	PrintE(DTFLog, false, 0, "Add Data Table Row is an editor only function. Do not call at runtime in package.");
	return false;
#endif
}

DEFINE_FUNCTION(UDataTableFL::execAddDataTableRow)
{
	P_GET_OBJECT(UDataTable, DataTable);
	Stack.StepCompiledIn<FStructProperty>(nullptr);
	void* RowPtr = Stack.MostRecentPropertyAddress;
	const FStructProperty* StructProp = CastField<FStructProperty>(Stack.MostRecentProperty);
	P_GET_PROPERTY(FNameProperty, RowName);
	P_GET_UBOOL(bSave);
	P_FINISH;

	if (!DataTable)
	{
		const FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AccessViolation,
			NSLOCTEXT("AddDataTableRow", "MissingTableInput", "Failed to resolve the table input. Be sure the DataTable is valid.")
		);
		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
	}
	else if (StructProp && RowPtr)
	{
		const UScriptStruct* OutputType = StructProp->Struct;
		const UScriptStruct* TableType = DataTable->GetRowStruct();

		const bool bCompatible = (OutputType == TableType) || (OutputType->IsChildOf(TableType) && FStructUtils::TheSameLayout(OutputType, TableType));
		if (bCompatible)
		{
			P_NATIVE_BEGIN;
			*(bool*)RESULT_PARAM = Generic_AddDataTableRow(DataTable, RowPtr, RowName, bSave);
			P_NATIVE_END;
		}
		else
		{
			const FBlueprintExceptionInfo ExceptionInfo(
				EBlueprintExceptionType::AccessViolation,
				NSLOCTEXT("AddDataTableRow", "IncompatibleProperty", "Incompatible output parameter; the data table's type is not the same as the return type.")
			);
			FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
		}
	}
	else
	{
		const FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AccessViolation,
			NSLOCTEXT("AddDataTableRow", "MissingOutputProperty", "Failed to resolve the output parameter for AddDataTableRow.")
		);
		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
	}
}
#pragma endregion

#pragma region AddDataTableRows
bool UDataTableFL::AddDataTableRows(UDataTable* DataTable, TArray<FTableRowBase> Rows, bool bReverse, bool bSave)
{
	// This should never run since the function uses a custom thunk
	check(0);
	return false;
}

bool UDataTableFL::Generic_AddDataTableRows(UDataTable* DataTable, const FArrayProperty* ArrayProperty, void* Rows, bool bReverse, bool bSave)
{
#if WITH_EDITOR
	if(!DataTable)
	{
		PrintE(DTFLog, false, 0, "Data table is not valid.");
		return false;
	}

	FScriptArrayHelper ArrayHelper(ArrayProperty, Rows);

	if (Rows)
	{
		// Get the row names so that we can use FindRowUnchecked
		TArray<FName> RowNames;
		UDataTableFunctionLibrary::GetDataTableRowNames(DataTable, RowNames);

		// Get the data table struct needed for initialising the array indices memory
		const UScriptStruct* StructType = DataTable->GetRowStruct();
		if (StructType == nullptr) return false;

		// Get the array size
		const int32 ArraySize = ArrayHelper.Num();

		if(bReverse)
		{
			// Loop through the array in reverse and add each struct to the data table
			for(int32 i = ArraySize-1; i >= 0; i--)
			{
				 // Get new row name
				 const FName NewRowName = GetValidDataTableRowName(DataTable);

				 // Cast row to table row base
				 const FTableRowBase* TableRowBase = static_cast<FTableRowBase*>(static_cast<void*>(ArrayHelper.GetRawPtr(i)));

				 // Add row
				 DataTable->AddRow(NewRowName, *TableRowBase);
			}
		}
		else
		{
			// Loop through the array and add each struct to the data table
			for(int32 i = 0; i < ArraySize; i++)
			{
				 // Get new row name
				 const FName NewRowName = GetValidDataTableRowName(DataTable);

				 // Cast row to table row base
				 const FTableRowBase* TableRowBase = static_cast<FTableRowBase*>(static_cast<void*>(ArrayHelper.GetRawPtr(i)));

				 // Add row
				 DataTable->AddRow(NewRowName, *TableRowBase);
			}
		}

		// Save data table asset
		if(bSave) SaveDataTable(DataTable);
		
		// Update data table view
		FDataTableEditorUtils::FDataTableEditorManager::Get().PostChange(DataTable, FDataTableEditorUtils::EDataTableChangeInfo::RowList);
		
		return true;
	}
	else
	{
		PrintE(DTFLog, false, 0, "Rows pointer is not valid.");
		return false;
	}
	
#else
	PrintE(DTFLog, false, 0, "Add Data Table Rows is an editor only function. Do not call at runtime in package.");
	return false;
#endif
}

DEFINE_FUNCTION(UDataTableFL::execAddDataTableRows)
{
	P_GET_OBJECT(UDataTable, Table);
	Stack.StepCompiledIn<FArrayProperty>(nullptr);
	void* RowsPtr = Stack.MostRecentPropertyAddress;
	const FArrayProperty* ArrayProperty = CastField<FArrayProperty>(Stack.MostRecentProperty);
	if (!ArrayProperty)
	{
		const FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AccessViolation,
			NSLOCTEXT("AddDataTableRows", "FailedArrayContext", "Failed to get array property.")
		);
		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
		Stack.bArrayContextFailed = true;
		return;
	}
	P_GET_UBOOL(bReverse);
	P_GET_UBOOL(bSave);

	P_FINISH;

	const FStructProperty* StructProp = CastField<FStructProperty>(ArrayProperty->Inner);
	
	if (!Table)
	{
		const FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AccessViolation,
			NSLOCTEXT("AddDataTableRows", "MissingTableInput", "Failed to resolve the table input. Be sure the DataTable is valid.")
		);
		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
	}
	else if (StructProp && RowsPtr)
	{
		const UScriptStruct* OutputType = StructProp->Struct;
		const UScriptStruct* TableType = Table->GetRowStruct();

		const bool bCompatible = (OutputType == TableType) || (OutputType->IsChildOf(TableType) && FStructUtils::TheSameLayout(OutputType, TableType));
		if (bCompatible)
		{
			P_NATIVE_BEGIN;
			*(bool*)RESULT_PARAM = Generic_AddDataTableRows(Table, ArrayProperty, RowsPtr, bReverse, bSave);
			P_NATIVE_END;
		}
		else
		{
			const FBlueprintExceptionInfo ExceptionInfo(
				EBlueprintExceptionType::AccessViolation,
				NSLOCTEXT("AddDataTableRow", "IncompatibleProperty", "Incompatible output parameter; the data table's type is not the same as the input type.")
			);
			FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
		}
	}
	else
	{
		const FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AccessViolation,
			NSLOCTEXT("AddDataTableRows", "MissingOutputProperty", "Failed to resolve the input parameter for AddDataTableRows.")
		);
		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
	}
}
#pragma endregion

#pragma region RemoveDataTableRow
bool UDataTableFL::RemoveDataTableRow(UDataTable* DataTable, FTableRowBase Row, bool bSave)
{
	check(0);
	return false;
}

bool UDataTableFL::Generic_RemoveDataTableRow(UDataTable* DataTable, void* Row, bool bSave)
{
#if WITH_EDITOR
	if (!DataTable)
	{
		PrintW(DTFLog, false, 0, "Data table is not valid.");
		return false;
	}

	if (Row)
	{
		FName RowToRemoveName = NAME_None;
		const TArray<FName> RowNames = DataTable->GetRowNames();
		const FString RemoveDataTableRowContext = "RemoveDataTableRowContextString";

		// Cast row to data table row base
		const FTableRowBase* TableRowBaseToRemove = static_cast<FTableRowBase*>(Row);
		if (!TableRowBaseToRemove)
		{
			PrintE(DTFLog, false, 0, "Could not cast row to table row base.");
			return false;
		}

		// Get data table row to remove name
		for (const FName RowName : RowNames)
		{
			const void* TableRowBase = DataTable->FindRowUnchecked(RowName);
			if (DataTable->RowStruct->CompareScriptStruct(Row, TableRowBase, PPF_None))
			{
				RowToRemoveName = RowName;
			}
		}

		// Could not find row
		if (RowToRemoveName == NAME_None)
		{
			Print(DTFLog, false, 0, "Could not find row to remove");
			return false;
		}

		// Remove row
		DataTable->RemoveRow(RowToRemoveName);

		// Save asset
		if(bSave) SaveDataTable(DataTable);

		// Update data table view
		FDataTableEditorUtils::FDataTableEditorManager::Get().PostChange(DataTable, FDataTableEditorUtils::EDataTableChangeInfo::RowList);
		
		return true;
	}

	PrintE(DTFLog, false, 0, "Row is not valid.");
	return false;
#else
	PrintE(DTFLog, false, 0, "Remove Data Table Row is an editor only function. Do not call at runtime in package.");
	return false;
#endif
}

DEFINE_FUNCTION(UDataTableFL::execRemoveDataTableRow)
{
	P_GET_OBJECT(UDataTable, DataTable);
	Stack.StepCompiledIn<FStructProperty>(nullptr);
	void* RowPtr = Stack.MostRecentPropertyAddress;
	P_GET_UBOOL(bSave);
	P_FINISH;

	FStructProperty* StructProp = CastField<FStructProperty>(Stack.MostRecentProperty);
	if (!DataTable)
	{
		const FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AccessViolation,
			NSLOCTEXT("RemoveDataTableRow", "MissingTableInput", "Failed to resolve the table input. Be sure the DataTable is valid.")
		);
		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
	}
	else if (StructProp && RowPtr)
	{
		UScriptStruct* OutputType = StructProp->Struct;
		const UScriptStruct* TableType = DataTable->GetRowStruct();

		const bool bCompatible = (OutputType == TableType) || (OutputType->IsChildOf(TableType) && FStructUtils::TheSameLayout(OutputType, TableType));
		if (bCompatible)
		{
			P_NATIVE_BEGIN;
			*(bool*)RESULT_PARAM = Generic_RemoveDataTableRow(DataTable, RowPtr, bSave);
			P_NATIVE_END;
		}
		else
		{
			const FBlueprintExceptionInfo ExceptionInfo(
				EBlueprintExceptionType::AccessViolation,
				NSLOCTEXT("RemoveDataTableRow", "IncompatibleProperty", "Incompatible output parameter; the data table's type is not the same as the return type.")
			);
			FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
		}
	}
	else
	{
		const FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AccessViolation,
			NSLOCTEXT("RemoveDataTableRow", "MissingOutputProperty", "Failed to resolve the output parameter for RemoveDataTableRow.")
		);
		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
	}
}
#pragma endregion

#pragma region ReplaceDataTableRow
bool UDataTableFL::ReplaceDataTableRow(UDataTable* DataTable, FName RowName, FTableRowBase Row, bool bSave)
{
	check(0);
	return false;
}

bool UDataTableFL::Generic_ReplaceDataTableRow(UDataTable* DataTable, FName RowName, void* Row, bool bSave)
{
#if WITH_EDITOR
	if (!DataTable)
	{
		PrintW(DTFLog, false, 0, "Data table is not valid.");
		return false;
	}

	if(!DataTable->GetRowNames().Contains(RowName))
	{
		PrintWF(DTFLog, false, 0, TEXT("Could not find row to replace using row name: %s"), *RowName.ToString());
		return false;
	}
	
	if (Row)
	{
		// Cast row to data table row base
		const FTableRowBase* TableRowBase= static_cast<FTableRowBase*>(Row);
		if (!TableRowBase)
		{
			PrintE(DTFLog, false, 0, "Could not cast row to table row base.");
			return false;
		}

		// Add the data table row using the row name will replace the current row
		DataTable->AddRow(RowName, *TableRowBase);

		// Save asset
		if(bSave) SaveDataTable(DataTable);

		// Update data table view
		FDataTableEditorUtils::FDataTableEditorManager::Get().PostChange(DataTable, FDataTableEditorUtils::EDataTableChangeInfo::RowList);
		
		return true;
	}

	PrintE(DTFLog, false, 0, "Row is not valid.");
	return false;
#else
	PrintE(DTFLog, false, 0, "Replace Data Table Row is an editor only function. Do not call at runtime in package.");
	return false;
#endif
}

DEFINE_FUNCTION(UDataTableFL::execReplaceDataTableRow)
{
	P_GET_OBJECT(UDataTable, DataTable);
	P_GET_PROPERTY(FNameProperty, RowName);
	Stack.StepCompiledIn<FStructProperty>(nullptr);
	void* RowPtr = Stack.MostRecentPropertyAddress;
	P_GET_UBOOL(bSave);
	P_FINISH;

	const FStructProperty* StructProp = CastField<FStructProperty>(Stack.MostRecentProperty);
	if (!DataTable)
	{
		const FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AccessViolation,
			NSLOCTEXT("ReplaceDataTableRow", "MissingTableInput", "Failed to resolve the table input. Be sure the DataTable is valid.")
		);
		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
	}
	else if (StructProp && RowPtr)
	{
		const UScriptStruct* OutputType = StructProp->Struct;
		const UScriptStruct* TableType = DataTable->GetRowStruct();

		const bool bCompatible = (OutputType == TableType) || (OutputType->IsChildOf(TableType) && FStructUtils::TheSameLayout(OutputType, TableType));
		if (bCompatible)
		{
			P_NATIVE_BEGIN;
			*(bool*)RESULT_PARAM = Generic_ReplaceDataTableRow(DataTable, RowName, RowPtr, bSave);
			P_NATIVE_END;
		}
		else
		{
			const FBlueprintExceptionInfo ExceptionInfo(
				EBlueprintExceptionType::AccessViolation,
				NSLOCTEXT("ReplaceDataTableRow", "IncompatibleProperty", "Incompatible output parameter; the data table's type is not the same as the return type.")
			);
			FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
		}
	}
	else
	{
		const FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AccessViolation,
			NSLOCTEXT("ReplaceDataTableRow", "MissingOutputProperty", "Failed to resolve the output parameter for ReplaceDataTableRow.")
		);
		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
	}
}
#pragma endregion

#pragma region GetDataTableRowName
FName UDataTableFL::GetDataTableRowName(UDataTable* DataTable, FTableRowBase Row)
{
	check(0);
	return NAME_None;
}

FName UDataTableFL::Generic_GetDataTableRowName(UDataTable* DataTable, void* Row)
{
	if (!DataTable)
	{
		PrintW(DTFLog, false, 0, "Data table is not valid.");
		return NAME_None;
	}
	
	if (Row)
	{
		// Cast row to data table row base
		const FTableRowBase* TableRowBaseToRemove = static_cast<FTableRowBase*>(Row);
		if (!TableRowBaseToRemove)
		{
			PrintE(DTFLog, false, 0, "Could not cast row to table row base.");
			return NAME_None;
		}

		// Get data table row to remove name
		const TArray<FName> RowNames = DataTable->GetRowNames();
		for (const FName RowName : RowNames)
		{
			const void* TableRowBase = DataTable->FindRowUnchecked(RowName);
			if (DataTable->RowStruct->CompareScriptStruct(Row, TableRowBase, PPF_None))
			{
				return RowName;
			}
		}
	}

	return NAME_None;
}

DEFINE_FUNCTION(UDataTableFL::execGetDataTableRowName)
{
	P_GET_OBJECT(UDataTable, DataTable);
	Stack.StepCompiledIn<FStructProperty>(nullptr);
	void* RowPtr = Stack.MostRecentPropertyAddress;
	P_FINISH;

	const FStructProperty* StructProp = CastField<FStructProperty>(Stack.MostRecentProperty);
	if (!DataTable)
	{
		const FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AccessViolation,
			NSLOCTEXT("GetDataTableRowName", "MissingTableInput", "Failed to resolve the table input. Be sure the DataTable is valid.")
		);
		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
	}
	else if (StructProp && RowPtr)
	{
		const UScriptStruct* OutputType = StructProp->Struct;
		const UScriptStruct* TableType = DataTable->GetRowStruct();

		const bool bCompatible = (OutputType == TableType) || (OutputType->IsChildOf(TableType) && FStructUtils::TheSameLayout(OutputType, TableType));
		if (bCompatible)
		{
			P_NATIVE_BEGIN;
			*(FName*)RESULT_PARAM = Generic_GetDataTableRowName(DataTable, RowPtr);
			P_NATIVE_END;
		}
		else
		{
			const FBlueprintExceptionInfo ExceptionInfo(
				EBlueprintExceptionType::AccessViolation,
				NSLOCTEXT("GetDataTableRowName", "IncompatibleProperty", "Incompatible output parameter; the data table's type is not the same as the return type.")
			);
			FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
		}
	}
	else
	{
		const FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AccessViolation,
			NSLOCTEXT("GetDataTableRowName", "MissingOutputProperty", "Failed to resolve the output parameter for GetDataTableRowName.")
		);
		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
	}
}
#pragma endregion

#pragma region DoesDataTableContainRow
bool UDataTableFL::DoesDataTableContainRow(UDataTable* DataTable, FTableRowBase Row)
{
	check(0);
	return false;
}

bool UDataTableFL::Generic_DoesDataTableContainRow(UDataTable* DataTable, void* Row)
{
	if (!DataTable)
	{
		PrintW(DTFLog, false, 0, "Data table is not valid.");
		return false;
	}
	
	if (Row)
	{
		// Cast row to data table row base
		const FTableRowBase* TableRowBaseToRemove = static_cast<FTableRowBase*>(Row);
		if (!TableRowBaseToRemove)
		{
			PrintE(DTFLog, false, 0, "Could not cast row to table row base.");
			return false;
		}

		// Get data table row to remove name
		const TArray<FName> RowNames = DataTable->GetRowNames();
		for (const FName RowName : RowNames)
		{
			const void* TableRowBase = DataTable->FindRowUnchecked(RowName);
			if (DataTable->RowStruct->CompareScriptStruct(Row, TableRowBase, PPF_None))
			{
				return true;
			}
		}
	}

	return false;
}

DEFINE_FUNCTION(UDataTableFL::execDoesDataTableContainRow)
{
	P_GET_OBJECT(UDataTable, DataTable);
	Stack.StepCompiledIn<FStructProperty>(nullptr);
	void* RowPtr = Stack.MostRecentPropertyAddress;
	P_FINISH;

	const FStructProperty* StructProp = CastField<FStructProperty>(Stack.MostRecentProperty);
	if (!DataTable)
	{
		const FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AccessViolation,
			NSLOCTEXT("DoesDataTableContainRow", "MissingTableInput", "Failed to resolve the table input. Be sure the DataTable is valid.")
		);
		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
	}
	else if (StructProp && RowPtr)
	{
		const UScriptStruct* OutputType = StructProp->Struct;
		const UScriptStruct* TableType = DataTable->GetRowStruct();

		const bool bCompatible = (OutputType == TableType) || (OutputType->IsChildOf(TableType) && FStructUtils::TheSameLayout(OutputType, TableType));
		if (bCompatible)
		{
			P_NATIVE_BEGIN;
			*(bool*)RESULT_PARAM = Generic_DoesDataTableContainRow(DataTable, RowPtr);
			P_NATIVE_END;
		}
		else
		{
			const FBlueprintExceptionInfo ExceptionInfo(
				EBlueprintExceptionType::AccessViolation,
				NSLOCTEXT("DoesDataTableContainRow", "IncompatibleProperty", "Incompatible output parameter; the data table's type is not the same as the return type.")
			);
			FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
		}
	}
	else
	{
		const FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AccessViolation,
			NSLOCTEXT("DoesDataTableContainRow", "MissingOutputProperty", "Failed to resolve the output parameter for DoesDataTableContainRow.")
		);
		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
	}
}
#pragma endregion

#pragma region ForEachDataTableRow
int32 UDataTableFL::ValidateRowIndex(UDataTable* DataTable, int32 Index)
{
	if (!DataTable)
	{
		PrintW(DTFLog, false, 0, "Data table is not valid.");
		return INDEX_NONE;
	}

	return (GetDataTableSize(DataTable) - 1) >= Index ? Index : INDEX_NONE;
}

void UDataTableFL::GetDataTableRowAtIndex(UDataTable* DataTable, int32 Index, FTableRowBase& Row)
{
	// This should never run since the function uses a custom thunk
	check(0);
}

void UDataTableFL::Generic_GetDataTableRowAtIndex(UDataTable* DataTable, int32 Index, void* Row)
{
	const UScriptStruct* StructType = DataTable->GetRowStruct();

	if (!DataTable)
	{
		// Initialise empty values and return
		StructType->InitializeStruct(Row);
		PrintW(DTFLog, false, 0, "Data table is not valid.");
		return;
	}

	TArray<FName> RowNames;
	UDataTableFunctionLibrary::GetDataTableRowNames(DataTable, RowNames);

	if (!RowNames.IsValidIndex(Index))
	{
		// Initialise empty values and return
		StructType->InitializeStruct(Row);
		PrintWF(DTFLog, false, 0, TEXT("Data base size is %i Index is %i. Cannot find row in data base with the given index."), RowNames.Num(), Index);
		return;
	}

	const FName RowName = RowNames[Index];

	if (Row && DataTable)
	{
		void* RowPtr = DataTable->FindRowUnchecked(RowName);

		if (RowPtr != nullptr)
		{
			if (StructType != nullptr)
			{
				StructType->CopyScriptStruct(Row, RowPtr);
				return;
			}
		}
	}

	// Initialise empty values
	StructType->InitializeStruct(Row);
	PrintE(DTFLog, false, 0, "Reached end of function which should not happen.");
}

DEFINE_FUNCTION(UDataTableFL::execGetDataTableRowAtIndex)
{
	P_GET_OBJECT(UDataTable, Table);
	P_GET_PROPERTY(FIntProperty, Index);

	Stack.StepCompiledIn<FStructProperty>(nullptr);
	void* OutRowPtr = Stack.MostRecentPropertyAddress;
	P_FINISH;

	FStructProperty* StructProp = CastField<FStructProperty>(Stack.MostRecentProperty);
	if (!Table)
	{
		const FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AccessViolation,
			NSLOCTEXT("GetDataTableRowAtIndex", "MissingTableInput", "Failed to resolve the table input. Be sure the DataTable is valid.")
		);
		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
	}
	else if (StructProp && OutRowPtr)
	{
		UScriptStruct* OutputType = StructProp->Struct;
		const UScriptStruct* TableType = Table->GetRowStruct();

		const bool bCompatible = (OutputType == TableType) ||
			(OutputType->IsChildOf(TableType) && FStructUtils::TheSameLayout(OutputType, TableType));
		if (bCompatible)
		{
			P_NATIVE_BEGIN;
				Generic_GetDataTableRowAtIndex(Table, Index, OutRowPtr);
			P_NATIVE_END;
		}
		else
		{
			const FBlueprintExceptionInfo ExceptionInfo(
				EBlueprintExceptionType::AccessViolation,
				NSLOCTEXT("GetDataTableRowAtIndex", "IncompatibleProperty", "Incompatible output parameter; the data table's type is not the same as the return type.")
			);
			FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
		}
	}
	else
	{
		const FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AccessViolation,
			NSLOCTEXT("GetDataTableRowAtIndex", "MissingOutputProperty", "Failed to resolve the output parameter for GetDataTableRow.")
		);
		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
	}
}
#pragma endregion

#pragma region GetDataTableAsArrayStruct
void UDataTableFL::GetDataTableAsArrayStruct(UDataTable* DataTable, TArray<int32>& Row)
{
	// This should never run since the function uses a custom thunk
	check(0);
}

void UDataTableFL::Generic_GetDataTableAsArrayStruct(const UDataTable* DataTable, const FArrayProperty* ArrayProperty, void* OutRowPtr)
{
	if (!DataTable)
	{
		PrintW(DTFLog, false, 0, "Data table is not valid.");
		return;
	}

	FScriptArrayHelper ArrayHelper(ArrayProperty, OutRowPtr);

	if (OutRowPtr)
	{
		// Get the row names so that we can use FindRowUnchecked
		TArray<FName> RowNames;
		UDataTableFunctionLibrary::GetDataTableRowNames(const_cast<UDataTable*>(DataTable), RowNames);

		// Get the data table struct needed for initialising the array indices memory
		const UScriptStruct* StructType = DataTable->GetRowStruct();
		if (StructType == nullptr) return;

		// Resize the array to the size of the data table
		ArrayHelper.Resize(RowNames.Num());

		for (int32 i = 0; i < RowNames.Num(); i++)
		{
			// Get the row data pointer
			void* RowPtr = DataTable->FindRowUnchecked(RowNames[i]);

			if (RowPtr != nullptr)
			{
				// Initialise the array memory using the struct and write the row into it
				StructType->InitializeStruct(ArrayHelper.GetRawPtr(i));
				ArrayProperty->Inner->CopySingleValueFromScriptVM(ArrayHelper.GetRawPtr(i), RowPtr);
			}
		}

		return;
	}

	PrintE(DTFLog, false, 0, "Out Row Ptr is not valid. This should not happen.");
}

DEFINE_FUNCTION(UDataTableFL::execGetDataTableAsArrayStruct)
{
	P_GET_OBJECT(UDataTable, Table);
	Stack.StepCompiledIn<FArrayProperty>(nullptr);
	void* OutRowPtr = Stack.MostRecentPropertyAddress;
	const FArrayProperty* ArrayProperty = CastField<FArrayProperty>(Stack.MostRecentProperty);

	if (!ArrayProperty)
	{
		const FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AccessViolation,
			NSLOCTEXT("GetDataTableAsArrayStruct", "FailedArrayContext", "Failed to get array property.")
		);
		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
		Stack.bArrayContextFailed = true;
		return;
	}

	P_FINISH;

	if (!Table)
	{
		const FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AccessViolation,
			NSLOCTEXT("GetDataTableAsArrayStruct", "MissingTableInput", "Failed to resolve the table input. Be sure the DataTable is valid.")
		);
		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
	}
	else if (OutRowPtr)
	{
		P_NATIVE_BEGIN;
			Generic_GetDataTableAsArrayStruct(Table, ArrayProperty, OutRowPtr);
		P_NATIVE_END;
	}
	else
	{
		const FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AccessViolation,
			NSLOCTEXT("GetDataTableAsArrayStruct", "MissingOutputProperty", "Failed to resolve the output parameter for GetDataTableAsArrayStruct.")
		);
		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
	}
}
#pragma endregion

#pragma region FindDataTableRowsByProperty
bool UDataTableFL::FindDataTableRowsByProperty_Int(UDataTable* DataTable, int32 Property, TArray<int32>& OutRows)
{
	// This should never run since the function uses a custom thunk
	check(0);
	return false;
}

bool UDataTableFL::FindDataTableRowsByProperty_Int64(UDataTable* DataTable, int64 Property, TArray<int32>& OutRows)
{
	// This should never run since the function uses a custom thunk
	check(0);
	return false;
}

bool UDataTableFL::FindDataTableRowsByProperty_Float(UDataTable* DataTable, float Property, TArray<int32>& OutRows)
{
	// This should never run since the function uses a custom thunk
	check(0);
	return false;
}

bool UDataTableFL::FindDataTableRowsByProperty_String(UDataTable* DataTable, FString Property, TArray<int32>& OutRows)
{
	// This should never run since the function uses a custom thunk
	check(0);
	return false;
}

bool UDataTableFL::FindDataTableRowsByProperty_Object(UDataTable* DataTable, UObject* Property, TArray<int32>& OutRows)
{
	// This should never run since the function uses a custom thunk
	check(0);
	return false;
}

bool UDataTableFL::FindDataTableRowsByProperty_Text(UDataTable* DataTable, FText Property, TArray<int32>& OutRows)
{
	// This should never run since the function uses a custom thunk
	check(0);
	return false;
}

bool UDataTableFL::FindDataTableRowsByProperty_Name(UDataTable* DataTable, FName Property, TArray<int32>& OutRows)
{
	// This should never run since the function uses a custom thunk
	check(0);
	return false;
}

bool UDataTableFL::FindDataTableRowsByProperty_Bool(UDataTable* DataTable, bool Property, TArray<int32>& OutRows)
{
	// This should never run since the function uses a custom thunk
	check(0);
	return false;
}

bool UDataTableFL::FindDataTableRowsByProperty_Enum(UDataTable* DataTable, uint8 Property, TArray<int32>& OutRows)
{
	// This should never run since the function uses a custom thunk
	check(0);
	return false;
}

bool UDataTableFL::FindDataTableRowsByProperty_FVector(UDataTable* DataTable, FVector Property, TArray<int32>& OutRows)
{
	// This should never run since the function uses a custom thunk
	check(0);
	return false;
}

bool UDataTableFL::FindDataTableRowsByProperty_FTransform(UDataTable* DataTable, FTransform Property, TArray<int32>& OutRows)
{
	// This should never run since the function uses a custom thunk
	check(0);
	return false;
}

bool UDataTableFL::FindDataTableRowsByProperty_FRotator(UDataTable* DataTable, FRotator Property, TArray<int32>& OutRows)
{
	// This should never run since the function uses a custom thunk
	check(0);
	return false;
}

bool UDataTableFL::Generic_FindDataTableRowsByProperty(UObject* WorldContext, UDataTable* DataTable, FProperty* Property, void* PropertyAddress, const FArrayProperty* ArrayProperty, void* OutRows)
{
	if (!DataTable)
	{
		PrintW(DTFLog, false, 0, "Data table is not valid.");
		return false;
	}

	if (!WorldContext)
	{
		PrintE(DTFLog, false, 0, "World context is not valid. This should not happen.");
		return false;
	}
	
	FScriptArrayHelper ArrayHelper(ArrayProperty, OutRows);
	bool bFoundRow = false;

	if (OutRows)
	{
		const UScriptStruct* DataTableStruct = const_cast<UScriptStruct*>(DataTable->GetRowStruct());

		const FString DataTableContextString = "FindDataTableRowsByPropertyContext";

		TArray<FProperty*> StructProps;
		for (TFieldIterator<FProperty> It(DataTableStruct); It; ++It)
		{
			FProperty* Prop = *It;
			check(Prop != nullptr);
			StructProps.Add(Prop);
		}

		TArray<uint8*> OutRowData;

		const auto RowMap = DataTable->GetRowMap();
		for (auto RowIt = RowMap.CreateConstIterator(); RowIt; ++RowIt)
		{
			uint8* RowData = RowIt.Value();
			for (int32 PropIdx = 0; PropIdx < StructProps.Num(); PropIdx++)
			{
				const void* AValuePtr = StructProps[PropIdx]->ContainerPtrToValuePtr<void>(RowData, StructProps[PropIdx]->ArrayDim == 1 ? 0 : StructProps[PropIdx]->ArrayDim);
				void* BValuePtr;
				if(Property->GetOwner<UFunction>() && PropertyAddress) BValuePtr = PropertyAddress;
				else BValuePtr = Property->ContainerPtrToValuePtr<void>(WorldContext);

				const bool bLocalFoundRow = ArePropertiesEqual(StructProps[PropIdx], AValuePtr, Property, BValuePtr);

				if (bLocalFoundRow)
				{
					OutRowData.Add(RowData);
					bFoundRow = true;
					break;
				}
			}
		}

		// Resize the array to the size of the data table
		ArrayHelper.Resize(OutRowData.Num());

		for (int32 i = 0; i < OutRowData.Num(); i++)
		{
			DataTableStruct->InitializeStruct(ArrayHelper.GetRawPtr(i));
			ArrayProperty->Inner->CopySingleValueFromScriptVM(ArrayHelper.GetRawPtr(i), OutRowData[i]);
		}
	}

	return bFoundRow;
}

DEFINE_FUNCTION(UDataTableFL::execFindDataTableRowsByProperty_Int)
{
	Generic_ExecFindDataTableRowsByProperty(Context, Stack, RESULT_PARAM);
}

DEFINE_FUNCTION(UDataTableFL::execFindDataTableRowsByProperty_Int64)
{
	Generic_ExecFindDataTableRowsByProperty(Context, Stack, RESULT_PARAM);
}

DEFINE_FUNCTION(UDataTableFL::execFindDataTableRowsByProperty_Float)
{
	Generic_ExecFindDataTableRowsByProperty(Context, Stack, RESULT_PARAM);
}

DEFINE_FUNCTION(UDataTableFL::execFindDataTableRowsByProperty_String)
{
	Generic_ExecFindDataTableRowsByProperty(Context, Stack, RESULT_PARAM);
}

DEFINE_FUNCTION(UDataTableFL::execFindDataTableRowsByProperty_Object)
{
	Generic_ExecFindDataTableRowsByProperty(Context, Stack, RESULT_PARAM);
}

DEFINE_FUNCTION(UDataTableFL::execFindDataTableRowsByProperty_Text)
{
	Generic_ExecFindDataTableRowsByProperty(Context, Stack, RESULT_PARAM);
}

DEFINE_FUNCTION(UDataTableFL::execFindDataTableRowsByProperty_Name)
{
	Generic_ExecFindDataTableRowsByProperty(Context, Stack, RESULT_PARAM);
}

DEFINE_FUNCTION(UDataTableFL::execFindDataTableRowsByProperty_Bool)
{
	Generic_ExecFindDataTableRowsByProperty(Context, Stack, RESULT_PARAM);
}

DEFINE_FUNCTION(UDataTableFL::execFindDataTableRowsByProperty_Enum)
{
	Generic_ExecFindDataTableRowsByProperty(Context, Stack, RESULT_PARAM);
}

DEFINE_FUNCTION(UDataTableFL::execFindDataTableRowsByProperty_FVector)
{
	Generic_ExecFindDataTableRowsByProperty(Context, Stack, RESULT_PARAM);
}

DEFINE_FUNCTION(UDataTableFL::execFindDataTableRowsByProperty_FTransform)
{
	Generic_ExecFindDataTableRowsByProperty(Context, Stack, RESULT_PARAM);
}

DEFINE_FUNCTION(UDataTableFL::execFindDataTableRowsByProperty_FRotator)
{
	Generic_ExecFindDataTableRowsByProperty(Context, Stack, RESULT_PARAM);
}

void UDataTableFL::Generic_ExecFindDataTableRowsByProperty(UObject* Context, FFrame& Stack, void* const Z_Param__Result)
{
	P_GET_OBJECT(UDataTable, Table);

	Stack.StepCompiledIn<FProperty>(nullptr);
	FProperty* Property = CastField<FProperty>(Stack.MostRecentProperty);
	void* PropertyPtr = Stack.MostRecentPropertyAddress;

	Stack.StepCompiledIn<FArrayProperty>(nullptr);
	void* OutRowPtr = Stack.MostRecentPropertyAddress;
	const FArrayProperty* ArrayProperty = CastField<FArrayProperty>(Stack.MostRecentProperty);

	if (!ArrayProperty)
	{
		const FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AccessViolation,
			NSLOCTEXT("FindDataTableRowsByProperty", "FailedArrayContext", "Failed to get array property.")
		);
		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
		Stack.bArrayContextFailed = true;
		return;
	}

	P_FINISH;

	if (!Table)
	{
		const FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AccessViolation,
			NSLOCTEXT("FindDataTableRowByProperty", "MissingTableInput", "Failed to resolve the table input. Be sure the DataTable is valid.")
		);
		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
	}
	else if (!Property)
	{
		const FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AccessViolation,
			NSLOCTEXT("FindDataTableRowByProperty", "PropertyError", "Failed to resolve the property.")
		);
		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
	}
	else if (OutRowPtr)
	{
		P_NATIVE_BEGIN;
			*(bool*)Z_Param__Result = UDataTableFL::Generic_FindDataTableRowsByProperty(Stack.Object, Table, Property, PropertyPtr, ArrayProperty, OutRowPtr);
		P_NATIVE_END;
	}
	else
	{
		const FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AccessViolation,
			NSLOCTEXT("FindDataTableRowByProperty", "MissingOutputProperty", "Failed to resolve the output parameter for FindDataTableRowByProperty.")
		);
		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
	}
}
#pragma endregion

#pragma region FindDataTableRowByProperty
bool UDataTableFL::FindDataTableRowByProperty_Int(UDataTable* DataTable, int32 Property, FTableRowBase& OutRow)
{
	// This should never run since the function uses a custom thunk
	check(0);
	return false;
}

bool UDataTableFL::FindDataTableRowByProperty_Int64(UDataTable* DataTable, int64 Property, FTableRowBase& OutRow)
{
	// This should never run since the function uses a custom thunk
	check(0);
	return false;
}

bool UDataTableFL::FindDataTableRowByProperty_Float(UDataTable* DataTable, float Property, FTableRowBase& OutRow)
{
	// This should never run since the function uses a custom thunk
	check(0);
	return false;
}

bool UDataTableFL::FindDataTableRowByProperty_String(UDataTable* DataTable, FString Property, FTableRowBase& OutRow)
{
	// This should never run since the function uses a custom thunk
	check(0);
	return false;
}

bool UDataTableFL::FindDataTableRowByProperty_Object(UDataTable* DataTable, UObject* Property, FTableRowBase& OutRow)
{
	// This should never run since the function uses a custom thunk
	check(0);
	return false;
}

bool UDataTableFL::FindDataTableRowByProperty_Text(UDataTable* DataTable, FText Property, FTableRowBase& OutRow)
{
	// This should never run since the function uses a custom thunk
	check(0);
	return false;
}

bool UDataTableFL::FindDataTableRowByProperty_Name(UDataTable* DataTable, FName Property, FTableRowBase& OutRow)
{
	// This should never run since the function uses a custom thunk
	check(0);
	return false;
}

bool UDataTableFL::FindDataTableRowByProperty_Bool(UDataTable* DataTable, bool Property, FTableRowBase& OutRow)
{
	// This should never run since the function uses a custom thunk
	check(0);
	return false;
}

bool UDataTableFL::FindDataTableRowByProperty_Enum(UDataTable* DataTable, uint8 Property, FTableRowBase& OutRow)
{
	// This should never run since the function uses a custom thunk
	check(0);
	return false;
}

bool UDataTableFL::FindDataTableRowByProperty_FVector(UDataTable* DataTable, FVector Property, FTableRowBase& OutRow)
{
	// This should never run since the function uses a custom thunk
	check(0);
	return false;
}

bool UDataTableFL::FindDataTableRowByProperty_FTransform(UDataTable* DataTable, FTransform Property, FTableRowBase& OutRow)
{
	// This should never run since the function uses a custom thunk
	check(0);
	return false;
}

bool UDataTableFL::FindDataTableRowByProperty_FRotator(UDataTable* DataTable, FRotator Property, FTableRowBase& OutRow)
{
	// This should never run since the function uses a custom thunk
	check(0);
	return false;
}

bool UDataTableFL::Generic_FindDataTableRowByProperty(UObject* WorldContext, UDataTable* DataTable, FProperty* Property, void* PropertyAddress, void* OutRow)
{
	if (!DataTable)
	{
		PrintW(DTFLog, false, 0, "Data table is not valid.");
		return false;
	}

	if (!WorldContext)
	{
		PrintE(DTFLog, false, 0, "World context is not valid. This should not happen.");
		return false;
	}

	// if (Property->GetOwner<UFunction>())
	// {
	// 	PrintW(DTFLog, false, 0, "Find Data Table Row By Property does not support literals for its property pin. Use a variable please.");
	// 	return false;
	// }
	
	bool bFoundRow = false;

	if (OutRow)
	{
		const UScriptStruct* DataTableStruct = const_cast<UScriptStruct*>(DataTable->GetRowStruct());

		const FString DataTableContextString = "FindDataTableRowByPropertyContext";

		TArray<FProperty*> StructProps;
		for (TFieldIterator<FProperty> It(DataTableStruct); It; ++It)
		{
			FProperty* Prop = *It;
			check(Prop != nullptr);
			StructProps.Add(Prop);
		}

		const auto RowMap = DataTable->GetRowMap();
		for (auto RowIt = RowMap.CreateConstIterator(); RowIt; ++RowIt)
		{
			uint8* RowData = RowIt.Value();
			for (int32 PropIdx = 0; PropIdx < StructProps.Num(); PropIdx++)
			{
				const void* AValuePtr = StructProps[PropIdx]->ContainerPtrToValuePtr<void>(RowData, StructProps[PropIdx]->ArrayDim == 1 ? 0 : StructProps[PropIdx]->ArrayDim);

				void* BValuePtr;
				if (Property->GetOwner<UFunction>() && PropertyAddress) BValuePtr = PropertyAddress;
				else BValuePtr = Property->ContainerPtrToValuePtr<void>(WorldContext);
				
				bFoundRow = ArePropertiesEqual(StructProps[PropIdx], AValuePtr, Property, BValuePtr);

				if (bFoundRow)
				{
					DataTableStruct->CopyScriptStruct(OutRow, RowData);
					return bFoundRow;
				}
			}
		}
	}

	return bFoundRow;
}

DEFINE_FUNCTION(UDataTableFL::execFindDataTableRowByProperty_Int)
{
	Generic_ExecFindDataTableRowByProperty(Context, Stack, RESULT_PARAM);
}

DEFINE_FUNCTION(UDataTableFL::execFindDataTableRowByProperty_Int64)
{
	Generic_ExecFindDataTableRowByProperty(Context, Stack, RESULT_PARAM);
}

DEFINE_FUNCTION(UDataTableFL::execFindDataTableRowByProperty_Float)
{
	Generic_ExecFindDataTableRowByProperty(Context, Stack, RESULT_PARAM);
}

DEFINE_FUNCTION(UDataTableFL::execFindDataTableRowByProperty_String)
{
	Generic_ExecFindDataTableRowByProperty(Context, Stack, RESULT_PARAM);
}

DEFINE_FUNCTION(UDataTableFL::execFindDataTableRowByProperty_Object)
{
	Generic_ExecFindDataTableRowByProperty(Context, Stack, RESULT_PARAM);
}

DEFINE_FUNCTION(UDataTableFL::execFindDataTableRowByProperty_Text)
{
	Generic_ExecFindDataTableRowByProperty(Context, Stack, RESULT_PARAM);
}

DEFINE_FUNCTION(UDataTableFL::execFindDataTableRowByProperty_Name)
{
	Generic_ExecFindDataTableRowByProperty(Context, Stack, RESULT_PARAM);
}

DEFINE_FUNCTION(UDataTableFL::execFindDataTableRowByProperty_Bool)
{
	Generic_ExecFindDataTableRowByProperty(Context, Stack, RESULT_PARAM);
}

DEFINE_FUNCTION(UDataTableFL::execFindDataTableRowByProperty_Enum)
{
	Generic_ExecFindDataTableRowByProperty(Context, Stack, RESULT_PARAM);
}

DEFINE_FUNCTION(UDataTableFL::execFindDataTableRowByProperty_FVector)
{
	Generic_ExecFindDataTableRowByProperty(Context, Stack, RESULT_PARAM);
}

DEFINE_FUNCTION(UDataTableFL::execFindDataTableRowByProperty_FTransform)
{
	Generic_ExecFindDataTableRowByProperty(Context, Stack, RESULT_PARAM);
}

DEFINE_FUNCTION(UDataTableFL::execFindDataTableRowByProperty_FRotator)
{
	Generic_ExecFindDataTableRowByProperty(Context, Stack, RESULT_PARAM);
}

void UDataTableFL::Generic_ExecFindDataTableRowByProperty(UObject* Context, FFrame& Stack, void* const Z_Param__Result)
{
	P_GET_OBJECT(UDataTable, Table);

	Stack.StepCompiledIn<FProperty>(nullptr);
	FProperty* Property = CastField<FProperty>(Stack.MostRecentProperty);
	void* PropertyPtr = Stack.MostRecentPropertyAddress;

	Stack.StepCompiledIn<FStructProperty>(nullptr);
	void* OutRowPtr = Stack.MostRecentPropertyAddress;

	P_FINISH;

	if (!Table)
	{
		const FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AccessViolation,
			NSLOCTEXT("FindDataTableRowByProperty", "MissingTableInput", "Failed to resolve the table input. Be sure the DataTable is valid.")
		);
		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
	}
	else if (!Property)
	{
		const FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AccessViolation,
			NSLOCTEXT("FindDataTableRowByProperty", "PropertyError", "Failed to resolve the property.")
		);
		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
	}
	else if (OutRowPtr)
	{
		P_NATIVE_BEGIN;
	    *(bool*)Z_Param__Result = Generic_FindDataTableRowByProperty(Stack.Object, Table, Property, PropertyPtr, OutRowPtr);
		P_NATIVE_END;
	}
	else
	{
		const FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AccessViolation,
			NSLOCTEXT("FindDataTableRowByProperty", "MissingOutputProperty", "Failed to resolve the output parameter for FindDataTableRowByProperty.")
		);
		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
	}
}

bool UDataTableFL::ArePropertiesEqual(FProperty* A, const void* AValuePtr, FProperty* B, const void* BValuePtr)
{
	if (!A || !B) return false;

	if (CastField<FFloatProperty>(A) && CastField<FFloatProperty>(B))
	{
		const float FloatA = CastField<FFloatProperty>(A)->GetFloatingPointPropertyValue(AValuePtr);
		const float FloatB = CastField<FFloatProperty>(B)->GetFloatingPointPropertyValue(BValuePtr);
		return FloatA == FloatB;
	}

	if (CastField<FIntProperty>(A) && CastField<FIntProperty>(B))
	{
		const int32 IntA = CastField<FIntProperty>(A)->GetSignedIntPropertyValue(AValuePtr);
		const int32 IntB = CastField<FIntProperty>(B)->GetSignedIntPropertyValue(BValuePtr);
		return IntA == IntB;
	}

	if (CastField<FInt64Property>(A) && CastField<FInt64Property>(B))
	{
		const int64 IntA = CastField<FInt64Property>(A)->GetSignedIntPropertyValue(AValuePtr);
		const int64 IntB = CastField<FInt64Property>(B)->GetSignedIntPropertyValue(BValuePtr);
		return IntA == IntB;
	}

	if (CastField<FStrProperty>(A) && CastField<FStrProperty>(B))
	{
		const FString StringA = CastField<FStrProperty>(A)->GetPropertyValue(AValuePtr);
		const FString StringB = CastField<FStrProperty>(B)->GetPropertyValue(BValuePtr);
		return StringA == StringB;
	}

	if (CastField<FObjectProperty>(A) && CastField<FObjectProperty>(B))
	{
		const UObject* ObjectA = CastField<FObjectProperty>(A)->GetObjectPropertyValue(AValuePtr);
		const UObject* ObjectB = CastField<FObjectProperty>(B)->GetObjectPropertyValue(BValuePtr);
		return ObjectA == ObjectB;
	}

	if (CastField<FTextProperty>(A) && CastField<FTextProperty>(B))
	{
		const FText TextA = CastField<FTextProperty>(A)->GetPropertyValue(AValuePtr);
		const FText TextB = CastField<FTextProperty>(B)->GetPropertyValue(BValuePtr);
		return TextA.EqualTo(TextB);
	}

	if (CastField<FNameProperty>(A) && CastField<FNameProperty>(B))
	{
		const FName NameA = CastField<FNameProperty>(A)->GetPropertyValue(AValuePtr);
		const FName NameB = CastField<FNameProperty>(B)->GetPropertyValue(BValuePtr);
		return NameA.IsEqual(NameB);
	}

	if (CastField<FBoolProperty>(A) && CastField<FBoolProperty>(B))
	{
		const bool BoolA = CastField<FBoolProperty>(A)->GetPropertyValue(AValuePtr);
		const bool BoolB = CastField<FBoolProperty>(B)->GetPropertyValue(BValuePtr);

		return BoolA == BoolB;
	}

	if (CastField<FEnumProperty>(A) && CastField<FEnumProperty>(B))
	{
		const FEnumProperty* EnumInternalPropA = CastField<FEnumProperty>(A);
		const FNumericProperty* EnumNumericPropA = EnumInternalPropA->GetUnderlyingProperty();
		const int64 EnumValueA = EnumNumericPropA->GetSignedIntPropertyValue(AValuePtr);

		const FEnumProperty* EnumInternalPropB = CastField<FEnumProperty>(B);
		const FNumericProperty* EnumNumericPropB = EnumInternalPropB->GetUnderlyingProperty();
		const int64 EnumValueB = EnumNumericPropB->GetSignedIntPropertyValue(BValuePtr);

		return EnumValueA == EnumValueB;
	}

	if (CastField<FByteProperty>(A) && CastField<FByteProperty>(B))
	{
		const FByteProperty* EnumPropertyA = CastField<FByteProperty>(A);
		const uint8 EnumValueA = EnumPropertyA->GetPropertyValue(AValuePtr);

		const FByteProperty* EnumPropertyB = CastField<FByteProperty>(B);
		const uint8 EnumValueB = EnumPropertyB->GetPropertyValue(BValuePtr);

		return EnumValueA == EnumValueB;
	}

	if (CastField<FStructProperty>(A) && CastField<FStructProperty>(B))
	{
		// Get the struct properties. This would include vector, transform rotators and so on
		const FStructProperty* StructA = CastField<FStructProperty>(A);
		const FStructProperty* StructB = CastField<FStructProperty>(B);

		// If the two struct are not of the same type return false
		if (StructA->Struct != StructB->Struct) return false;

		// We then get a full list of both struct FProperties. E.g. Vector contains 3 FFloatProperty X, Y, Z
		TArray<FProperty*> StructAProperties;
		TArray<FProperty*> StructBProperties;

		for (TFieldIterator<FProperty> StructFieldItA(StructA->Struct); StructFieldItA; ++StructFieldItA)
		{
			StructAProperties.Add(*StructFieldItA);
		}

		for (TFieldIterator<FProperty> StructFieldItB(StructB->Struct); StructFieldItB; ++StructFieldItB)
		{
			StructBProperties.Add(*StructFieldItB);
		}

		// We then loop through all the struct properties of both structs to compare each one of them
		for (int32 i = 0; i < StructAProperties.Num(); i++)
		{
			if (StructAProperties.IsValidIndex(i) && StructBProperties.IsValidIndex(i))
			{
				// For each we need to get the new value ptr since we are now inside of the struct this cannot be the same as the one passed
				// The new value ptr can be derived from the container pointer which is the one passed in
				const void* StructAPropertyValuePtr = StructAProperties[i]->ContainerPtrToValuePtr<void>(AValuePtr);
				const void* StructBPropertyValuePtr = StructBProperties[i]->ContainerPtrToValuePtr<void>(BValuePtr);

				// If any of the properties are not the same return false
				if (!ArePropertiesEqual(StructAProperties[i], StructAPropertyValuePtr, StructBProperties[i], StructBPropertyValuePtr)) return false;
			}
		}

		// If we got here it means all the properties were exactly the same
		return true;
	}

	return false;
}
#pragma endregion

#pragma region FindDataTableRowByPredicate
bool UDataTableFL::FindDataTableRowByPredicate(UDataTable* DataTable, FFindDataTableRowPredicate Predicate, FTableRowBase& Row)
{
	check(0);
	return false;
}

bool UDataTableFL::Generic_FindDataTableRowByPredicate(UDataTable* DataTable, FFindDataTableRowPredicate Predicate, void* Row)
{
	if (!DataTable)
	{
		PrintE(DTFLog, false, 0, "Data table is not valid.");
		return false;
	}

	const UScriptStruct* DataTableStruct = DataTable->RowStruct;
	if (!DataTableStruct)
	{
		PrintE(DTFLog, false, 0, "Data table struct is not valid.");
		return false;
	}

	UDataTableRowObject* DataTableRowObject = NewObject<UDataTableRowObject>();
	DataTableRowObject->DataTable = DataTable;

	TArray<FName> RowNames = DataTable->GetRowNames();
	for (const FName RowName : RowNames)
	{
		void* RowPtr = DataTable->FindRowUnchecked(RowName);
		DataTableRowObject->RowData = RowPtr;

		if (Predicate.Execute(DataTableRowObject))
		{
			DataTableStruct->InitializeStruct(Row);
			DataTableStruct->CopyScriptStruct(Row, RowPtr);
			return true;
		}
	}

	return false;
}

DEFINE_FUNCTION(UDataTableFL::execFindDataTableRowByPredicate)
{
	P_GET_OBJECT(UDataTable, DataTable);
	P_GET_PROPERTY(FDelegateProperty, DelegateProperty);

	FFindDataTableRowPredicate FindDataTableRowPredicate;
	FindDataTableRowPredicate.BindUFunction(DelegateProperty.GetUObject(), DelegateProperty.GetFunctionName());

	Stack.StepCompiledIn<FStructProperty>(nullptr);
	void* RowPtr = Stack.MostRecentPropertyAddress;
	P_FINISH;

	if (!DataTable)
	{
		const FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AccessViolation,
			NSLOCTEXT("FindDataTableRowByPredicate", "MissingTableInput", "Failed to resolve the table input. Be sure the DataTable is valid.")
		);
		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
	}
	if (DataTable && RowPtr)
	{
		P_NATIVE_BEGIN;
			*(bool*)RESULT_PARAM = Generic_FindDataTableRowByPredicate(DataTable, FindDataTableRowPredicate, RowPtr);
		P_NATIVE_END;
	}
	else
	{
		const FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AccessViolation,
			NSLOCTEXT("FindDataTableRowByPredicate", "MissingOutputProperty", "Failed to resolve the output parameter for FindDataTableRowByPredicate.")
		);
		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
	}
}

void UDataTableFL::GetAsDataTableRow(UDataTableRowObject* RowObject, FTableRowBase& Row)
{
	check(0);
}

void UDataTableFL::Generic_GetAsDataTableRow(UDataTableRowObject* RowObject, void* Row)
{
	if (!RowObject)
	{
		PrintW(DTFLog, false, 0, "Row object is not valid.");
		return;
	}

	if (!RowObject->DataTable)
	{
		PrintW(DTFLog, false, 0, "Row object data table is not valid. This should not happen.");
		return;
	}

	if (!RowObject->RowData)
	{
		PrintW(DTFLog, false, 0, "Row object row data is not valid. This should not happen.");
		return;
	}

	if (RowObject && RowObject->DataTable && RowObject->RowData && Row)
	{
		UScriptStruct* ScriptType = RowObject->DataTable->RowStruct;
		if (ScriptType)
		{
			ScriptType->InitializeStruct(Row);
			ScriptType->CopyScriptStruct(Row, RowObject->RowData);
		}
	}
}

DEFINE_FUNCTION(UDataTableFL::execGetAsDataTableRow)
{
	P_GET_OBJECT(UDataTableRowObject, RowObject);

	Stack.StepCompiledIn<FStructProperty>(nullptr);
	void* RowPtr = Stack.MostRecentPropertyAddress;
	P_FINISH;

	if (!RowObject)
	{
		const FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AccessViolation,
			NSLOCTEXT("GetAsDataTableRow", "RowObjectInvalid", "Failed to get row object.")
		);
		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
	}
	if (RowPtr)
	{
		P_NATIVE_BEGIN;
			Generic_GetAsDataTableRow(RowObject, RowPtr);
		P_NATIVE_END;
	}
	else
	{
		const FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AccessViolation,
			NSLOCTEXT("GetAsDataTableRow", "MissingOutputProperty", "Failed to resolve the output parameter for GetAsDataTableRow.")
		);
		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
	}
}
#pragma endregion

#pragma region FindDataTableRowsByPredicate
bool UDataTableFL::FindDataTableRowsByPredicate(UDataTable* DataTable, FFindDataTableRowPredicate Predicate, TArray<int32>& OutRows)
{
	check(0);
	return false;
}

bool UDataTableFL::Generic_FindDataTableRowsByPredicate(UDataTable* DataTable, FFindDataTableRowPredicate Predicate, const FArrayProperty* ArrayProperty, void* OutRows)
{
	if (!DataTable)
	{
		PrintW(DTFLog, false, 0, "Data table is not valid.");
		return false;
	}

	if (!ArrayProperty)
	{
		PrintW(DTFLog, false, 0, "Array property is not valid.");
		return false;
	}

	FScriptArrayHelper ArrayHelper(ArrayProperty, OutRows);
	bool bFoundRow = false;

	if (OutRows && DataTable)
	{
		const UScriptStruct* DataTableStruct = const_cast<UScriptStruct*>(DataTable->GetRowStruct());
		if (!DataTableStruct)
		{
			PrintW(DTFLog, false, 0, "Data table struct is not valid. This should not happen.");
			return false;
		}

		UDataTableRowObject* DataTableRowObject = NewObject<UDataTableRowObject>();
		DataTableRowObject->DataTable = DataTable;

		TArray<uint8*> OutRowData;

		const auto RowMap = DataTable->GetRowMap();
		for (auto RowIt = RowMap.CreateConstIterator(); RowIt; ++RowIt)
		{
			uint8* RowData = RowIt.Value();
			DataTableRowObject->RowData = RowData;
			if (Predicate.Execute(DataTableRowObject))
			{
				OutRowData.Add(RowData);
				bFoundRow = true;
			}
		}

		// Resize the array to the size of the data table
		ArrayHelper.Resize(OutRowData.Num());

		for (int32 i = 0; i < OutRowData.Num(); i++)
		{
			DataTableStruct->InitializeStruct(ArrayHelper.GetRawPtr(i));
			ArrayProperty->Inner->CopySingleValueFromScriptVM(ArrayHelper.GetRawPtr(i), OutRowData[i]);
		}
	}

	return bFoundRow;
}

DEFINE_FUNCTION(UDataTableFL::execFindDataTableRowsByPredicate)
{
	P_GET_OBJECT(UDataTable, DataTable);
	P_GET_PROPERTY(FDelegateProperty, DelegateProperty);

	FFindDataTableRowPredicate FindDataTableRowPredicate;
	FindDataTableRowPredicate.BindUFunction(DelegateProperty.GetUObject(), DelegateProperty.GetFunctionName());

	Stack.StepCompiledIn<FArrayProperty>(nullptr);
	void* OutRowPtr = Stack.MostRecentPropertyAddress;
	const FArrayProperty* ArrayProperty = CastField<FArrayProperty>(Stack.MostRecentProperty);

	if (!ArrayProperty)
	{
		const FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AccessViolation,
			NSLOCTEXT("FindDataTableRowsByPredicate", "FailedArrayContext", "Failed to get array property.")
		);
		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
	}

	P_FINISH;

	if (!DataTable)
	{
		const FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AccessViolation,
			NSLOCTEXT("FindDataTableRowsByPredicate", "MissingTableInput", "Failed to resolve the table input. Be sure the DataTable is valid.")
		);
		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
	}
	else if (OutRowPtr)
	{
		P_NATIVE_BEGIN;
			*(bool*)RESULT_PARAM = Generic_FindDataTableRowsByPredicate(DataTable, FindDataTableRowPredicate, ArrayProperty, OutRowPtr);
		P_NATIVE_END;
	}
	else
	{
		const FBlueprintExceptionInfo ExceptionInfo(
			EBlueprintExceptionType::AccessViolation,
			NSLOCTEXT("FindDataTableRowsByPredicate", "MissingOutputProperty", "Failed to resolve the output parameter for FindDataTableRowsByPredicate.")
		);
		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
	}
}
#pragma endregion

