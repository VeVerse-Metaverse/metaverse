// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


using UnrealBuildTool;

public class VeMonitoring : ModuleRules {
	public VeMonitoring(ReadOnlyTargetRules Target) : base(Target) {
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		CppStandard = CppStandardVersion.Cpp17;

		PrivateDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "HTTP", "VeShared", "Json" });

		if (Target.Platform == UnrealTargetPlatform.IOS || Target.Platform == UnrealTargetPlatform.Mac || Target.Platform == UnrealTargetPlatform.Android) {
			PrivateDependencyModuleNames.Add("Bugsnag");
		}

		if (Target.Platform == UnrealTargetPlatform.Win64 || Target.Platform == UnrealTargetPlatform.Mac || Target.Platform == UnrealTargetPlatform.Linux) {
			PrivateDependencyModuleNames.AddRange(new string[] { "VeSentry" });
		}
	}
}
