// Author: Egor A. Pristavka
// Copyright (c) 2022 LE7EL AS. All right reserved.

using UnrealBuildTool;

public class VeUI : ModuleRules {
	public VeUI(ReadOnlyTargetRules Target) : base(Target) {
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		CppStandard = CppStandardVersion.Cpp17;

		PrivateDefinitions.Add("VE_LOG_CATEGORY=LogVeUI");
		PrivateDefinitions.Add("VE_LOG_MODULE=VeUI");

		PublicDependencyModuleNames.AddRange(new string[] {
			"AnimatedTexture",
			"Core",
			"CoreUObject",
			"Engine",
			"HeadMountedDisplay",
			"InputCore",
			"UMG",
			"EnhancedInput",
			"VeApi",
			"VeApi2",
			"VeDownload",
			"VeShared",
			"VeGameFramework",
			"VePak",
			"VeRpc",
			"Json"
		});

		// Uncomment if you are using Slate UI
		PrivateDependencyModuleNames.AddRange(new string[] {
			"AnimatedTexture",
			"HTTP",
			"Slate",
			"SlateCore",
			"VeApi2",
			"VeOpenSea",
			"VeQRCode",
			"VeOAuth",
		});

		if (Target.Platform == UnrealTargetPlatform.Win64 || Target.Platform == UnrealTargetPlatform.Mac || Target.Platform == UnrealTargetPlatform.Linux) {
			PrivateDependencyModuleNames.AddRange(new string[] {
				"VePlatform"
			});
		}

		if (Target.bBuildEditor) {
			PrivateDependencyModuleNames.AddRange(
				new string[] {
					"EditorStyle",
					"EngineSettings",
					"LevelEditor",
					"Sequencer",
					"UnrealEd",
					"ViewportInteraction",
					"VREditor",
				}
			);
		}

		if (Target.Platform == UnrealTargetPlatform.Android) {
			PrivateDependencyModuleNames.Add("AndroidPermission");
		}
#if UE_4_24_OR_LATER
		bUseUnity = true;
#else
		bFasterWithoutUnity = false;
#endif
	}
}
