

using UnrealBuildTool;
using System.Collections.Generic;

public class SOMEWHEREServerTarget : TargetRules
{
	public SOMEWHEREServerTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Server;
		DefaultBuildSettings = BuildSettingsVersion.V2;

		ExtraModuleNames.AddRange( new string[] { "SOMEWHERE" } );
	}
}
