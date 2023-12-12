// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

using UnrealBuildTool;

public class VeSocial : ModuleRules {
	public VeSocial(ReadOnlyTargetRules Target) : base(Target) {
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		CppStandard = CppStandardVersion.Cpp17;
		OptimizeCode = CodeOptimization.Never;
		
		PrivateDefinitions.Add("VE_LOG_CATEGORY=LogVeShared");
		PrivateDefinitions.Add("VE_LOG_MODULE=VeSocial");

		PublicDependencyModuleNames.AddRange(new string[] {
			  "Core"
			, "CoreUObject"
			, "Engine"
			, "VeShared"
			, "VeApi"
			, "VeApi2"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });
	}
}
