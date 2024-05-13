// Copyright 2022 Just2Devs. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Engine/UserDefinedStruct.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DataTableFL.generated.h"

UCLASS(BlueprintType)
class DATATABLEFUNCTIONLIBRARY_API UDataTableRowObject : public UObject
{
	GENERATED_BODY()

public:
	UDataTableRowObject() {}
	
	UDataTableRowObject(UDataTable* InDataTable, void* InRowData)
		: DataTable(InDataTable),
		  RowData(InRowData)
	{}

	UPROPERTY()
	UDataTable* DataTable;

	void* RowData;
};

UENUM(BlueprintType)
enum class EDTFRowPosition : uint8
{
	Before,
	After
};

UENUM(BlueprintType)
enum class EDTFRowDirection : uint8
{
	Up,
	Down
};

DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(bool, FFindDataTableRowPredicate, UDataTableRowObject*, RowObject);

UCLASS()
class DATATABLEFUNCTIONLIBRARY_API UDataTableFL : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	friend class UDataTableRowObject;
	
public:
	/**
	 * Returns the size of the data table which is the total number of rows.
	 * @param DataTable The data table to get the size of.
	 * @return The size of the data table.
	 */
	UFUNCTION(BlueprintPure, Category = "Data Table Function Library")
	static int32 GetDataTableSize(UDataTable* DataTable);

	/**
	 * Empty the data table. EDITOR ONLY!
	 * WARNING: PLEASE BE CAREFUL WITH THIS NODE AS IT DIRECTLY MODIFIES THE DATA TABLE.
	 * @param DataTable The data table to empty.
	 * @param bSave Should the data table asset be automatically saved?
	 * @return Returns true if the data table was successfully emptied and the package was saved. Otherwise it returns false.
	 */
	UFUNCTION(BlueprintCallable, Category = "Data Table Function Library|Editor only", meta = (DisplayName = "Empty Data Table (EDITOR ONLY)", AdvancedDisplay=1))
	static bool EmptyDataTable(UDataTable* DataTable, bool bSave = false);

	/**
	 * Attempts to save the data table asset in editor.
	 * @param DataTable The data table to save.
	 * @return True if successful otherwise false.
	 */
	UFUNCTION(BlueprintCallable, Category = "Data Table Function Library|Editor only", meta = (DisplayName = "Save Data Table (EDITOR ONLY)"))
	static bool SaveDataTable(UDataTable* DataTable);
	
	/**
	 * Move a row by defining its name and where it should moved to compared to another row.
	 * @param DataTable The data table to move the row in.
	 * @param RowToMove The row to move name.
	 * @param ReferenceRow The row name used as a reference to move the row before or after.
	 * @param RowPosition The row position, either before or after the reference row.
	 * @param bSave Should the data table be automatically saved?
	 * @return True if successful otherwise false.
	 */
	UFUNCTION(BlueprintCallable, Category = "Data Table Function Library|Editor only", meta = (DisplayName = "Move Data Table Row (EDITOR ONLY)", DataTablePin="DataTable", AdvancedDisplay=4))
	static bool MoveDataTableRow(UDataTable* DataTable, FName RowToMove, FName ReferenceRow, EDTFRowPosition RowPosition, bool bSave);
	
	/**
	 * Move a row by defining its name and the index at which it should move.
	 * @param DataTable The data table to move the row in.
	 * @param RowToMove The row to move name.
	 * @param Index The index at which to move the row.
	 * @param bSave Should the data table be automatically saved?
	 * @return True if successful otherwise false.
	 */
	UFUNCTION(BlueprintCallable, Category = "Data Table Function Library|Editor only", meta = (DisplayName = "Move Data Table Row At Index (EDITOR ONLY)", DataTablePin="DataTable", AdvancedDisplay=3))
	static bool MoveDataTableRowAtIndex(UDataTable* DataTable, FName RowToMove, int32 Index, bool bSave);
	
	/**
	 * Move a row by defining its name, the number of rows it should move and the direction.
	 * @param DataTable The data table to move the row in.
	 * @param RowToMove The row to move name.
	 * @param MoveBy Move the row by this number.
	 * @param Direction The direction in which to move the row.
	 * @param bSave Should the data table by automatically saved?
	 * @return True is successful otherwise false.
	 */
	UFUNCTION(BlueprintCallable, Category = "Data Table Function Library|Editor only", meta = (DisplayName = "Move Data Table Row By Number (EDITOR ONLY)", DataTablePin="DataTable", AdvancedDisplay=4))
	static bool MoveDataTableRowByNumber(UDataTable* DataTable, FName RowToMove, int32 MoveBy = 1, EDTFRowDirection Direction = EDTFRowDirection::Down, bool bSave = false);
	
	/**
	 * Get the data table row names.
	 * @param DataTable The data table to get the row names from.
	 * @return The array of row names.
	 */
	UFUNCTION(BlueprintPure, Category = "Data Table Function Library")
	static TArray<FName> GetDataTableRowNames(const UDataTable* DataTable);

#pragma region AddDataTableRow
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, CustomThunk, meta = (CustomStructureParam="Row", DisplayName = "Add Data Table Row (Editor Only)"))
	static bool AddDataTableRow(UDataTable* DataTable, FTableRowBase Row, FName RowName, bool bSave);

	static bool Generic_AddDataTableRow(UDataTable* DataTable, void* Row, FName RowName, bool bSave);
	
	DECLARE_FUNCTION(execAddDataTableRow);
#pragma endregion

#pragma region AddDataTableRows
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, CustomThunk, Category = "Data Table Function Library|Editor only", meta = (CustomStructureParam = "Rows"))
	static bool AddDataTableRows(UDataTable* DataTable, TArray<FTableRowBase> Rows, bool bReverse, bool bSave);

	static bool Generic_AddDataTableRows(UDataTable* DataTable, const FArrayProperty* ArrayProperty, void* Rows, bool bReverse, bool bSave);
	
	DECLARE_FUNCTION(execAddDataTableRows);
#pragma endregion

#pragma region RemoveDataTableRow
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, CustomThunk, Category = "Data Table Function Library|Editor only", meta = (CustomStructureParam="Row"))
	static bool RemoveDataTableRow(UDataTable* DataTable, FTableRowBase Row, bool bSave);

	static bool Generic_RemoveDataTableRow(UDataTable* DataTable, void* Row, bool bSave);

	DECLARE_FUNCTION(execRemoveDataTableRow);
#pragma endregion

#pragma region ReplaceDataTableRow
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, CustomThunk, Category = "Data Table Function Library|Editor Only", meta = (DataTablePin="DataTable", CustomStructureParam="Row"))
	static bool ReplaceDataTableRow(UDataTable* DataTable, FName RowName, FTableRowBase Row, bool bSave);

	static bool Generic_ReplaceDataTableRow(UDataTable* DataTable, FName RowName, void* Row, bool bSave);

	DECLARE_FUNCTION(execReplaceDataTableRow);
#pragma endregion

#pragma region GetDataTableRowName
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, CustomThunk, Category = "Data Table Function Library", meta = (CustomStructureParam="Row"))
	static FName GetDataTableRowName(UDataTable* DataTable, FTableRowBase Row);

	static FName Generic_GetDataTableRowName(UDataTable* DataTable, void* Row);

	DECLARE_FUNCTION(execGetDataTableRowName);
#pragma endregion

#pragma region DoesDataTableContainRow
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, CustomThunk, Category = "Data Table Function Library", meta = (CustomStructureParam="Row"))
	static bool DoesDataTableContainRow(UDataTable* DataTable, FTableRowBase Row);

	static bool Generic_DoesDataTableContainRow(UDataTable* DataTable, void* Row);

	DECLARE_FUNCTION(execDoesDataTableContainRow);
#pragma endregion

#pragma region ForEachDataTableRow
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly, Category = "Data Table Function Library")
	static int32 ValidateRowIndex(UDataTable* DataTable, int32 Index);
	
	UFUNCTION(BlueprintPure, CustomThunk, BlueprintInternalUseOnly, Category = "Data Table Function Library", meta = (CustomStructureParam = "Row"))
	static void GetDataTableRowAtIndex(UDataTable* DataTable, int32 Index, FTableRowBase& Row);

	static void Generic_GetDataTableRowAtIndex(UDataTable* DataTable, int32 Index, void* Row);
	
	DECLARE_FUNCTION(execGetDataTableRowAtIndex);
#pragma endregion
	
#pragma region GetDataTableAsArrayStruct
	UFUNCTION(BlueprintCallable, CustomThunk, BlueprintInternalUseOnly, Category = "Data Table Function Library", meta = (CustomStructureParam="Row"))
	static void GetDataTableAsArrayStruct(UDataTable* DataTable, TArray<int32>& Row);

	static void Generic_GetDataTableAsArrayStruct(const UDataTable* DataTable, const FArrayProperty* ArrayProperty, void* OutRowPtr);

    DECLARE_FUNCTION(execGetDataTableAsArrayStruct);
#pragma endregion 

#pragma region FindDataTableRowsByProperty
	UFUNCTION(BlueprintCallable, CustomThunk, BlueprintInternalUseOnly, Category = "Data Table Function Library", meta = (CustomStructureParam = "OutRows"))
	static bool FindDataTableRowsByProperty_Int(UDataTable* DataTable, int32 Property, TArray<int32>& OutRows);

	UFUNCTION(BlueprintCallable, CustomThunk, BlueprintInternalUseOnly, Category = "Data Table Function Library", meta = (CustomStructureParam = "OutRows"))
	static bool FindDataTableRowsByProperty_Int64(UDataTable* DataTable, int64 Property, TArray<int32>& OutRows);
	
	UFUNCTION(BlueprintCallable, CustomThunk, BlueprintInternalUseOnly, Category = "Data Table Function Library", meta = (CustomStructureParam = "OutRows"))
	static bool FindDataTableRowsByProperty_Float(UDataTable* DataTable, float Property, TArray<int32>& OutRows);
	
	UFUNCTION(BlueprintCallable, CustomThunk, BlueprintInternalUseOnly, Category = "Data Table Function Library", meta = (CustomStructureParam = "OutRows"))
	static bool FindDataTableRowsByProperty_String(UDataTable* DataTable, FString Property, TArray<int32>& OutRows);
	
	UFUNCTION(BlueprintCallable, CustomThunk, BlueprintInternalUseOnly, Category = "Data Table Function Library", meta = (CustomStructureParam = "OutRows"))
	static bool FindDataTableRowsByProperty_Object(UDataTable* DataTable, UObject* Property, TArray<int32>& OutRows);
	
	UFUNCTION(BlueprintCallable, CustomThunk, BlueprintInternalUseOnly, Category = "Data Table Function Library", meta = (CustomStructureParam = "OutRows"))
	static bool FindDataTableRowsByProperty_Text(UDataTable* DataTable, FText Property, TArray<int32>& OutRows);
	
	UFUNCTION(BlueprintCallable, CustomThunk, BlueprintInternalUseOnly, Category = "Data Table Function Library", meta = (CustomStructureParam = "OutRows"))
	static bool FindDataTableRowsByProperty_Name(UDataTable* DataTable, FName Property, TArray<int32>& OutRows);
	
	UFUNCTION(BlueprintCallable, CustomThunk, BlueprintInternalUseOnly, Category = "Data Table Function Library", meta = (CustomStructureParam = "OutRows"))
	static bool FindDataTableRowsByProperty_Bool(UDataTable* DataTable, bool Property, TArray<int32>& OutRows);
	
	UFUNCTION(BlueprintCallable, CustomThunk, BlueprintInternalUseOnly, Category = "Data Table Function Library", meta = (CustomStructureParam = "OutRows"))
	static bool FindDataTableRowsByProperty_Enum(UDataTable* DataTable, uint8 Property, TArray<int32>& OutRows);
	
	UFUNCTION(BlueprintCallable, CustomThunk, BlueprintInternalUseOnly, Category = "Data Table Function Library", meta = (CustomStructureParam = "OutRows"))
	static bool FindDataTableRowsByProperty_FVector(UDataTable* DataTable, FVector Property, TArray<int32>& OutRows);
	
	UFUNCTION(BlueprintCallable, CustomThunk, BlueprintInternalUseOnly, Category = "Data Table Function Library", meta = (CustomStructureParam = "OutRows"))
	static bool FindDataTableRowsByProperty_FTransform(UDataTable* DataTable, FTransform Property, TArray<int32>& OutRows);
	
	UFUNCTION(BlueprintCallable, CustomThunk, BlueprintInternalUseOnly, Category = "Data Table Function Library", meta = (CustomStructureParam = "OutRows"))
	static bool FindDataTableRowsByProperty_FRotator(UDataTable* DataTable, FRotator Property, TArray<int32>& OutRows);
	
	static bool Generic_FindDataTableRowsByProperty(UObject* WorldContext, UDataTable* DataTable, FProperty* Property, void* PropertyAddress, const FArrayProperty* ArrayProperty, void* OutRows);
	
	DECLARE_FUNCTION(execFindDataTableRowsByProperty_Int);
	DECLARE_FUNCTION(execFindDataTableRowsByProperty_Int64);
	DECLARE_FUNCTION(execFindDataTableRowsByProperty_Float);
	DECLARE_FUNCTION(execFindDataTableRowsByProperty_String);
	DECLARE_FUNCTION(execFindDataTableRowsByProperty_Object);
	DECLARE_FUNCTION(execFindDataTableRowsByProperty_Text);
	DECLARE_FUNCTION(execFindDataTableRowsByProperty_Name);
	DECLARE_FUNCTION(execFindDataTableRowsByProperty_Bool);
	DECLARE_FUNCTION(execFindDataTableRowsByProperty_Enum);
	DECLARE_FUNCTION(execFindDataTableRowsByProperty_FVector);
	DECLARE_FUNCTION(execFindDataTableRowsByProperty_FTransform);
	DECLARE_FUNCTION(execFindDataTableRowsByProperty_FRotator);
	
	static void Generic_ExecFindDataTableRowsByProperty(UObject* Context, FFrame& Stack, void*const Z_Param__Result);
#pragma endregion
	
#pragma region FindDataTableRowByProperty
	UFUNCTION(BlueprintCallable, CustomThunk, BlueprintInternalUseOnly, Category = "Data Table Function Library", meta = (CustomStructureParam = "OutRow"))
	static bool FindDataTableRowByProperty_Int(UDataTable* DataTable, int32 Property, FTableRowBase& OutRow);
	
	UFUNCTION(BlueprintCallable, CustomThunk, BlueprintInternalUseOnly, Category = "Data Table Function Library", meta = (CustomStructureParam = "OutRow"))
	static bool FindDataTableRowByProperty_Int64(UDataTable* DataTable, int64 Property, FTableRowBase& OutRow);
	
	UFUNCTION(BlueprintCallable, CustomThunk, BlueprintInternalUseOnly, Category = "Data Table Function Library", meta = (CustomStructureParam = "OutRow"))
	static bool FindDataTableRowByProperty_Float(UDataTable* DataTable, float Property, FTableRowBase& OutRow);
	
	UFUNCTION(BlueprintCallable, CustomThunk, BlueprintInternalUseOnly, Category = "Data Table Function Library", meta = (CustomStructureParam = "OutRow"))
	static bool FindDataTableRowByProperty_String(UDataTable* DataTable, FString Property, FTableRowBase& OutRow);
	
	UFUNCTION(BlueprintCallable, CustomThunk, BlueprintInternalUseOnly, Category = "Data Table Function Library", meta = (CustomStructureParam = "OutRow"))
	static bool FindDataTableRowByProperty_Object(UDataTable* DataTable, UObject* Property, FTableRowBase& OutRow);
	
	UFUNCTION(BlueprintCallable, CustomThunk, BlueprintInternalUseOnly, Category = "Data Table Function Library", meta = (CustomStructureParam = "OutRow"))
	static bool FindDataTableRowByProperty_Text(UDataTable* DataTable, FText Property, FTableRowBase& OutRow);
	
	UFUNCTION(BlueprintCallable, CustomThunk, BlueprintInternalUseOnly, Category = "Data Table Function Library", meta = (CustomStructureParam = "OutRow"))
	static bool FindDataTableRowByProperty_Name(UDataTable* DataTable, FName Property, FTableRowBase& OutRow);
	
	UFUNCTION(BlueprintCallable, CustomThunk, BlueprintInternalUseOnly, Category = "Data Table Function Library", meta = (CustomStructureParam = "OutRow"))
	static bool FindDataTableRowByProperty_Bool(UDataTable* DataTable, bool Property, FTableRowBase& OutRow);
	
	UFUNCTION(BlueprintCallable, CustomThunk, BlueprintInternalUseOnly, Category = "Data Table Function Library", meta = (CustomStructureParam = "OutRow"))
	static bool FindDataTableRowByProperty_Enum(UDataTable* DataTable, uint8 Property, FTableRowBase& OutRow);
	
	UFUNCTION(BlueprintCallable, CustomThunk, BlueprintInternalUseOnly, Category = "Data Table Function Library", meta = (CustomStructureParam = "OutRow"))
	static bool FindDataTableRowByProperty_FVector(UDataTable* DataTable, FVector Property, FTableRowBase& OutRow);
	
	UFUNCTION(BlueprintCallable, CustomThunk, BlueprintInternalUseOnly, Category = "Data Table Function Library", meta = (CustomStructureParam = "OutRow"))
	static bool FindDataTableRowByProperty_FTransform(UDataTable* DataTable, FTransform Property, FTableRowBase& OutRow);
	
	UFUNCTION(BlueprintCallable, CustomThunk, BlueprintInternalUseOnly, Category = "Data Table Function Library", meta = (CustomStructureParam = "OutRow"))
	static bool FindDataTableRowByProperty_FRotator(UDataTable* DataTable, FRotator Property, FTableRowBase& OutRow);
	
	static bool Generic_FindDataTableRowByProperty(UObject* WorldContext, UDataTable* DataTable, FProperty* Property, void* PropertyAddress, void* OutRow);

	DECLARE_FUNCTION(execFindDataTableRowByProperty_Int);
	DECLARE_FUNCTION(execFindDataTableRowByProperty_Int64);
	DECLARE_FUNCTION(execFindDataTableRowByProperty_Float);
	DECLARE_FUNCTION(execFindDataTableRowByProperty_String);
	DECLARE_FUNCTION(execFindDataTableRowByProperty_Object);
	DECLARE_FUNCTION(execFindDataTableRowByProperty_Text);
	DECLARE_FUNCTION(execFindDataTableRowByProperty_Name);
	DECLARE_FUNCTION(execFindDataTableRowByProperty_Bool);
	DECLARE_FUNCTION(execFindDataTableRowByProperty_Enum);
	DECLARE_FUNCTION(execFindDataTableRowByProperty_FVector);
	DECLARE_FUNCTION(execFindDataTableRowByProperty_FTransform);
	DECLARE_FUNCTION(execFindDataTableRowByProperty_FRotator);

	static void Generic_ExecFindDataTableRowByProperty(UObject* Context, FFrame& Stack, void*const Z_Param__Result);
	static bool ArePropertiesEqual(FProperty* A, const void* AValuePtr, FProperty* B, const void* BValuePtr);
#pragma endregion 

#pragma region FindDataTableRowByPredicate
	UFUNCTION(BlueprintCallable, CustomThunk, BlueprintInternalUseOnly, Category = "Data Table Function Library", meta = (CustomStructureParam = "Row"))
	static bool FindDataTableRowByPredicate(UDataTable* DataTable, FFindDataTableRowPredicate Predicate, FTableRowBase& Row);
	
	static bool Generic_FindDataTableRowByPredicate(UDataTable* DataTable, FFindDataTableRowPredicate Predicate, void* Row);

	DECLARE_FUNCTION(execFindDataTableRowByPredicate);

	/**
	 * The GetAsDataTableRow utility function allows you to convert a data table row object into the struct of the data table you are working with.
	 * The node by default will show a wildcard as an output, however, if you drag the pin and open the blueprint context menu you can break this pin
	 * into any struct. Doing so will convert the pin into the type you required, in this case the data table struct.
	 *
	 * NOTE: You will need to use this node only if you are working with the Find Data Table Row/s By Predicate nodes. Read those nodes descriptions to know more.
	 * @param RowObject The data table row object to convert into the data table struct.
	 * @param Row The row struct. This is a wildcard by default but you can drag off the pin and break the struct into the data table struct to convert the pin.
	 */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "Data Table Function Library", meta = (CustomStructureParam="Row"))
	static void GetAsDataTableRow(UDataTableRowObject* RowObject, FTableRowBase& Row);

	static void Generic_GetAsDataTableRow(UDataTableRowObject* RowObject, void* Row);

	DECLARE_FUNCTION(execGetAsDataTableRow);
#pragma endregion

#pragma region FindDataTableRowsByPredicate
	UFUNCTION(BlueprintCallable, CustomThunk, BlueprintInternalUseOnly, Category = "Data Table Function Library", meta = (CustomStructureParam = "OutRows"))
	static bool FindDataTableRowsByPredicate(UDataTable* DataTable, FFindDataTableRowPredicate Predicate, TArray<int32>& OutRows);

	static bool Generic_FindDataTableRowsByPredicate(UDataTable* DataTable, FFindDataTableRowPredicate Predicate, const FArrayProperty* ArrayProperty, void* OutRows);

	DECLARE_FUNCTION(execFindDataTableRowsByPredicate);
#pragma endregion

private:
	static FName GetValidDataTableRowName(const UDataTable* DataTable);
};

