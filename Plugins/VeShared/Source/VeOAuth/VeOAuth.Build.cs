// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

using UnrealBuildTool;

public class VeOAuth : ModuleRules {
	public VeOAuth(ReadOnlyTargetRules Target) : base(Target) {
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[] {
				"Core",
				"VeShared",
				"Json",
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[] {
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore", 
				"VeApi2",
				"ApplicationCore"
			}
		);
	}
}
