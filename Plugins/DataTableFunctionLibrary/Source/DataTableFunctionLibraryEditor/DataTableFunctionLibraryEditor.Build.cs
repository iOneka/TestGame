// Copyright 2022 Just2Devs. All Rights Reserved.

using UnrealBuildTool;

public class DataTableFunctionLibraryEditor : ModuleRules
{
	public DataTableFunctionLibraryEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core", 
				"HTTP"
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"KismetCompiler",
				"BlueprintGraph",
				"UnrealEd",
				"DataTableFunctionLibrary",
				"EditorStyle",
				"GraphEditor"
			}
			);

	}
}
