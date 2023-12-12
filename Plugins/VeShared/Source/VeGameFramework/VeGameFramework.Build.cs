// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


using System.IO;
using UnrealBuildTool;

public class VeGameFramework : ModuleRules {
	public VeGameFramework(ReadOnlyTargetRules Target) : base(Target) {
		PublicDependencyModuleNames.AddRange(new string[] { });
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		CppStandard = CppStandardVersion.Cpp17;

		PrivateDefinitions.Add("VE_LOG_CATEGORY=LogVeGameFramework");
		PrivateDefinitions.Add("VE_LOG_MODULE=VeGameFramework");
		
		PrivateDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"Json",
			"HTTP",
			"UMG",
			"EnhancedInput",
			"VeShared",
			"VeApi",
			"VeApi2",
			"VeDownload",
			"VePak",
			"PakLoader",
			"VeMonitoring",
			"AIModule"
		});

		// PublicIncludePathModuleNames.AddRange(new string[] { "VeDownload" });
		// PublicIncludePaths.AddRange(new string[] { ModuleDirectory });

		PrivateDependencyModuleNames.AddRange(new string[] { });

		bool EnableUserActions = Target.Type != TargetType.Server;
		if (EnableUserActions) {
			PublicDefinitions.Add("WITH_USER_ACTIONS=1");
		} else {
			PublicDefinitions.Add("WITH_USER_ACTIONS=0");
		}
		
#if UE_4_24_OR_LATER
		bUseUnity = false;
#else
		bFasterWithoutUnity = true;
#endif
	}
}
