// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

using System.IO;
using UnrealBuildTool;

public class VeInteraction : ModuleRules {
	public VeInteraction(ReadOnlyTargetRules Target) : base(Target) {
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		CppStandard = CppStandardVersion.Cpp17;

		PrivateDefinitions.Add("VE_LOG_CATEGORY=LogVeInteraction");
		PrivateDefinitions.Add("VE_LOG_MODULE=VeInteraction");

		PublicDependencyModuleNames.AddRange(new[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"Slate",
			"UMG",
			"HeadMountedDisplay",
			"NavigationSystem",
			"EnhancedInput",
			"VeShared",
			"VeGameFramework",
			"ALSV4_CPP"
		});

		// VR support.
		bool EnableVR = Target.Platform == UnrealTargetPlatform.Android || Target.Platform == UnrealTargetPlatform.Win64 || Target.Platform == UnrealTargetPlatform.Mac;

		if (EnableVR) {
			PublicDependencyModuleNames.AddRange(new string[] {
				// "HeadMountedDisplay",
				// "NavigationSystem"
			});

			if (Target.Platform == UnrealTargetPlatform.Win64) {
				PublicDependencyModuleNames.AddRange(new[] {
					"SteamVR",
					"SteamVRInput"
				});
			} else {
				PublicDefinitions.Add("STEAMVR_SUPPORTED_PLATFORMS=0");
			}
		} else {
			PublicDefinitions.Add("STEAMVR_SUPPORTED_PLATFORMS=0");
		}

		PublicIncludePaths.Add(ModuleDirectory);
	}
}
