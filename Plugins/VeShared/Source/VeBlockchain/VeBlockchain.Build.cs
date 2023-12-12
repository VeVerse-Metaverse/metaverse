// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

using System.IO;
using UnrealBuildTool;

public class VeBlockchain : ModuleRules {
	public VeBlockchain(ReadOnlyTargetRules Target) : base(Target) {
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		CppStandard = CppStandardVersion.Cpp17;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"Json",
			"HTTP",
			"VeShared",
			"VeDownload",
			"UMG"
		});

		PublicIncludePaths.AddRange(new string[] { ModuleDirectory });

		PrivateDependencyModuleNames.AddRange(new string[] { "VeApi" });

#if UE_4_24_OR_LATER
		bUseUnity = false;
#else
		bFasterWithoutUnity = true;
#endif
	}
}
