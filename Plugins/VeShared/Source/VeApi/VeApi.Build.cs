// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

using System.IO;
using UnrealBuildTool;

public class VeApi : ModuleRules {
	public VeApi(ReadOnlyTargetRules Target) : base(Target) {
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		CppStandard = CppStandardVersion.Cpp17;

		PrivateDefinitions.Add("VE_LOG_CATEGORY=LogVeApi");
		PrivateDefinitions.Add("VE_LOG_MODULE=VeApi");

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"Json",
			"HTTP",
			"VeShared",
			"VeMonitoring"
		});

		PublicIncludePathModuleNames.AddRange(new string[] { "VeDownload" });
		PublicIncludePaths.AddRange(new string[] { ModuleDirectory });

		PrivateDependencyModuleNames.AddRange(new string[] { });

		bool EnableUserActions = Target.Type != TargetType.Server;
		if (EnableUserActions) {
			PublicDefinitions.Add("WITH_USER_ACTIONS=1");
		} else {
			PublicDefinitions.Add("WITH_USER_ACTIONS=0");
		}
	}
}
