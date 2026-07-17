// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class AdvancedSteamSession : ModuleRules
{
	public AdvancedSteamSession(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"AdvancedSteamSession",
			"AdvancedSteamSession/Variant_Platforming",
			"AdvancedSteamSession/Variant_Platforming/Animation",
			"AdvancedSteamSession/Variant_Combat",
			"AdvancedSteamSession/Variant_Combat/AI",
			"AdvancedSteamSession/Variant_Combat/Animation",
			"AdvancedSteamSession/Variant_Combat/Gameplay",
			"AdvancedSteamSession/Variant_Combat/Interfaces",
			"AdvancedSteamSession/Variant_Combat/UI",
			"AdvancedSteamSession/Variant_SideScrolling",
			"AdvancedSteamSession/Variant_SideScrolling/AI",
			"AdvancedSteamSession/Variant_SideScrolling/Gameplay",
			"AdvancedSteamSession/Variant_SideScrolling/Interfaces",
			"AdvancedSteamSession/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
