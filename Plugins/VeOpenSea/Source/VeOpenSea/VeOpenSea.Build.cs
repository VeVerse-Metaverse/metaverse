// Author: Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

using UnrealBuildTool;

public class VeOpenSea : ModuleRules {
	public VeOpenSea(ReadOnlyTargetRules Target) : base(Target) {
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		CppStandard = CppStandardVersion.Cpp17;

		PrivateDefinitions.Add("VE_LOG_CATEGORY=LogVeOpenSea");
		PrivateDefinitions.Add("VE_LOG_MODULE=VeOpenSea");

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"HTTP",
			"VeShared",
			"VeApi",
			"glTFRuntime",
			"Json",
			"DeveloperSettings",
			"VeDownload"
		});

		if (Target.bBuildEditor) {
			PublicDependencyModuleNames.Add("UnrealEd");
		}
		
		if (Target.Platform == UnrealTargetPlatform.Android) {
			PrivateDependencyModuleNames.Add("AndroidPermission");
		}
		// PrivateDependencyModuleNames.AddRange(new string[] {
		// 	"YourQRCode"
		// });
	}
}
