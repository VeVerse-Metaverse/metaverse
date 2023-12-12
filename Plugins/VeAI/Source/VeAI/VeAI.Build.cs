// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class VeAI : ModuleRules {
	public VeAI(ReadOnlyTargetRules Target) : base(Target) {
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivateDefinitions.Add("VE_LOG_CATEGORY=LogVeAI");
		PrivateDefinitions.Add("VE_LOG_MODULE=VeAI");

		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
		);


		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
		);


		PublicDependencyModuleNames.AddRange(
			new string[] {
				"Core", "AIModule", "GameplayTasks", "VeShared", "VeApi", "VeApi2", "VeUI", "UMG", "VeInteraction",
				"VeGameFramework", "ALSV4_CPP", "Json", "JsonUtilities", "AIModule", "NavigationSystem", "VeRpc",
				"HairStrandsCore",
				// ... add other public dependencies that you statically link with here ...
			}
		);


		PrivateDependencyModuleNames.AddRange(
			new string[] {
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"RuntimeAudioImporter",
				"VeDownload",
				// ... add private dependencies that you statically link with here ...	
			}
		);


		DynamicallyLoadedModuleNames.AddRange(
			new string[] {
				// ... add any modules that your module loads dynamically here ...
			}
		);
	}
}
