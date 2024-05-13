

using UnrealBuildTool;
using System.Collections.Generic;

public class SOMEWHERETarget : TargetRules
{
	public SOMEWHERETarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		
		ExtraModuleNames.AddRange( new string[] { "SOMEWHERE" } );
	}
}
