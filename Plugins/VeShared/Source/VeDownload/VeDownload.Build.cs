// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

using UnrealBuildTool;

public class VeDownload : ModuleRules {
	public VeDownload(ReadOnlyTargetRules Target) : base(Target) {
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		CppStandard = CppStandardVersion.Cpp17;

		PrivateDefinitions.Add("VE_LOG_CATEGORY=LogVeDownload");
		PrivateDefinitions.Add("VE_LOG_MODULE=VeDownload");

		PrivateDependencyModuleNames.AddRange(new string[] {"Core", "CoreUObject", "Engine", "RHI", "HTTP", "VeShared", "Json", "VeApi", "VeApi2" });
		PublicDependencyModuleNames.Add("AnimatedTexture");

#if UE_4_24_OR_LATER
		bUseUnity = false;
#else
		bFasterWithoutUnity = true;
#endif
	}
}
