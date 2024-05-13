// Copyright (c) 2020 Nineva Studios

using UnrealBuildTool;
using System.Collections.Generic;
using System.IO;
using System;

#if UE_5_0_OR_LATER
using EpicGames.Core;
#else
using Tools.DotNETCommon;
#endif

public class MonetizationGoodies : ModuleRules
{
	public MonetizationGoodies(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(new[] {Path.Combine(ModuleDirectory, "Public")});
		PrivateIncludePaths.AddRange(new[] {Path.Combine(ModuleDirectory, "Private")});

		PublicDependencyModuleNames.AddRange(new[] {"Core"});
		PrivateDependencyModuleNames.AddRange(new[] {"CoreUObject", "Engine", "Projects"});

		CheckConflictingPlugins(Target);

		var PluginPath = Utils.MakePathRelativeTo(ModuleDirectory, Target.RelativeEnginePath);

		if (Target.Platform == UnrealTargetPlatform.Android)
		{
			PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Private/Android"));
			PublicDependencyModuleNames.Add("Launch");

			AdditionalPropertiesForReceipt.Add("AndroidPlugin", Path.Combine(PluginPath, "MonetizationGoodies_Android_UPL.xml"));
		}

		// Additional routine for iOS
		if (Target.Platform == UnrealTargetPlatform.IOS)
		{
			PublicFrameworks.AddRange(new[] {"StoreKit",});

			var EnableInAppPurchasesIos = false;

			if (Target.ProjectFile != null)
			{
				var ProjectPath = Path.GetDirectoryName(Target.ProjectFile.ToString());
				var Ini = ConfigCache.ReadHierarchy(ConfigHierarchyType.Engine, new DirectoryReference(ProjectPath), Target.Platform);

				bool TempBool;
				if (Ini.TryGetValue("/Script/MonetizationGoodies.MonetizationGoodiesSettings", "EnableInAppPurchases", out TempBool))
					EnableInAppPurchasesIos = TempBool;

				PublicDefinitions.Add("MG_ENABLE_PURCHASES_IOS=" + (EnableInAppPurchasesIos ? "1" : "0"));
			}

			PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Private/IOS"));

			AdditionalPropertiesForReceipt.Add("IOSPlugin", Path.Combine(PluginPath, "MonetizationGoodies_IOS_UPL.xml"));
		}
	}

	void CheckConflictingPlugins(ReadOnlyTargetRules Target)
	{
		Console.WriteLine("Engine Directory:" + EngineDirectory);

		var pluginList = new List<PluginInfo>();
		pluginList.AddRange(Plugins.ReadEnginePlugins(new DirectoryReference(EngineDirectory)));

		if (Target.ProjectFile != null)
		{
			Console.WriteLine("Project Directory:" + Target.ProjectFile.Directory);
			pluginList.AddRange(Plugins.ReadEnginePlugins(Target.ProjectFile.Directory));
		}

		string onlineSubsystemPluginName;
		if (Target.Platform == UnrealTargetPlatform.Android)
		{
			onlineSubsystemPluginName = "OnlineSubsystemGooglePlay";
		}
		else if (Target.Platform == UnrealTargetPlatform.IOS)
		{
			onlineSubsystemPluginName = "OnlineSubsystemApple";
		}
		else
		{
			return;
		}

		ProjectDescriptor projectDescriptor = null;
		if (Target.ProjectFile != null)
		{
			projectDescriptor = ProjectDescriptor.FromFile(Target.ProjectFile);
		}

		foreach (var plugin in pluginList)
		{
			if (plugin.Name != onlineSubsystemPluginName)
			{
				continue;
			}

			if (Plugins.IsPluginEnabledForTarget(plugin, projectDescriptor, Target.Platform, Target.Configuration, Target.Type))
			{
				Log.TraceWarning("Monetization Goodies plugin is incompatible with UE4 built-in Online Subsystem. Please, disable the " + onlineSubsystemPluginName + " plugin.");
			}
		}
	}
}