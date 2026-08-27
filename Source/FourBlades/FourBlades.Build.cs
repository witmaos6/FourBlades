// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class FourBlades : ModuleRules
{
	public FourBlades(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"EnhancedInput",
			//GAS
			"GameplayTags",
			"GameplayTasks",
			"GameplayAbilities",
			// Game Features
			"ModularGameplayActors",
            "ModularGameplay",
			"GameFeatures",
			// CommonUser
            "CommonUser",
			// CommonGame
			"CommonGame",
			//CommonUI
			"CommonUI",
			"UMG",
			//UIExtension
			"UIExtension",
			//Slate
			"Slate",
			"SlateCore",
			// Niagara
			"Niagara",
		});

		PrivateDependencyModuleNames.AddRange(new string[] { "NavigationSystem", "AIModule" });

		PublicIncludePaths.AddRange(new string[] { "FourBlades" });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
