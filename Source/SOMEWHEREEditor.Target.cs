

using UnrealBuildTool;
using System.Collections.Generic;

public class SOMEWHEREEditorTarget : TargetRules
{
	public SOMEWHEREEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;

		ExtraModuleNames.AddRange( new string[] { "SOMEWHERE" } );
	}
}
