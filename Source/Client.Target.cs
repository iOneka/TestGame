using UnrealBuildTool;
using System.Collections.Generic;

public class SOMEWHEREClientTarget : TargetRules
{
	public SOMEWHEREClientTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Client;
		DefaultBuildSettings = BuildSettingsVersion.V2;

		ExtraModuleNames.AddRange(new string[] { "SOMEWHERE" });
	}
}