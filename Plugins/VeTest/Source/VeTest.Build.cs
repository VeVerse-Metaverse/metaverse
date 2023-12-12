// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

using UnrealBuildTool;

public class VeTest : ModuleRules {
	public VeTest(ReadOnlyTargetRules Target) : base(Target) {
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		CppStandard = CppStandardVersion.Cpp17;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"UMG",
			"VeInteraction",
			"VeShared",
			"VeApi",
			"VeUI",
			"PakLoader",
			"VeGame",
			"ALSV4_CPP",
			"VeGameFramework"
		});

		PrivateDependencyModuleNames.Add("Gauntlet");

		PrivateDependencyModuleNames.AddRange(new string[] { "VeApi2" });
	}
}
