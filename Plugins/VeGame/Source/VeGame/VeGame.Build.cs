// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

using System.Linq;
using UnrealBuildTool;

public class VeGame : ModuleRules {
	public VeGame(ReadOnlyTargetRules Target) : base(Target) {
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		CppStandard = CppStandardVersion.Cpp17;

		PrivateDefinitions.Add("VE_LOG_CATEGORY=LogVeGame");
		PrivateDefinitions.Add("VE_LOG_MODULE=VeGame");

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"HTTP",
			"VeApi",
			"VeDownload",
			"VeInteraction",
			"VeGameFramework",
			"VeUI",
			"UMG",
			"glTFRuntime",
			"Niagara",
			"VePak",
			"VeBlockchain",
			"VeShared",
			"SlateCore",
			"Slate",
			"Projects",
			"VeOpenSea",
			"MediaAssets",
			"VeRpc",
			"InputCore",
			"ALSV4_CPP",
			"EnhancedInput",
			"AudioMixer",
			"VeLiveLink",
			"AIModule",
		});

		if (!Target.ProjectDefinitions.Contains("BuildSDK")) {
			PublicDependencyModuleNames.AddRange(new string[] {
				"ReadyPlayerMe",
			});
		}

		if (Target.bBuildEditor) {
			PublicDependencyModuleNames.AddRange(new string[] {
				"UnrealEd",
				//"VeSDK",
				"EditorStyle"
			});
		}

		PrivateDependencyModuleNames.AddRange(new string[] {
			"VeApi2",
			"VeQRCode",
			"VeAI",
			"AudioCapture"
		});

		if (Target.Platform == UnrealTargetPlatform.Win64 || Target.Platform == UnrealTargetPlatform.Mac ||
		    Target.Platform == UnrealTargetPlatform.Linux) {
			PrivateDependencyModuleNames.AddRange(new string[] {
				"VePlatform"
			});
		}

		// // Voice support.
		// bool EnableVivox = Target.Platform == UnrealTargetPlatform.Win64 ||
		//                    Target.Platform == UnrealTargetPlatform.Android ||
		//                    Target.Platform == UnrealTargetPlatform.Mac ||
		//                    Target.Platform == UnrealTargetPlatform.IOS;
		//
		// if (EnableVivox) {
		// 	PublicDependencyModuleNames.AddRange(new[] {
		// 		"VivoxCore"
		// 	});
		// 	PublicDefinitions.Add("WITH_VIVOX=1");
		// }


		// Artheon BEGIN
		bool EnableWebsocketRpc = Target.Type == TargetType.Editor || Target.Type == TargetType.Server ||
		                          Target.Type == TargetType.Game || Target.Type == TargetType.Client;

		PublicDefinitions.Add("WITH_SSL=1");

		if (EnableWebsocketRpc) {
			PublicDefinitions.Add("WITH_WEBSOCKETS=1");
			PublicDefinitions.Add("WITH_LIBWEBSOCKETS=1");
			PublicDefinitions.Add("WITH_RPC=1");

			PublicDependencyModuleNames.AddRange(new[] {
				"WebSockets",
				"Json"
			});
		} else {
			// PublicDefinitions.Add("WITH_SSL=0");
			// PublicDefinitions.Add("WITH_WEBSOCKETS=0");
			// PublicDefinitions.Add("WITH_LIBWEBSOCKETS=0");
			PublicDefinitions.Add("WITH_RPC=0");
		}

		// Voice support.
		bool EnableVivox = (Target.Platform == UnrealTargetPlatform.Win64
		                    || Target.Platform == UnrealTargetPlatform.Mac
				// || Target.Platform == UnrealTargetPlatform.Android
				// || Target.Platform == UnrealTargetPlatform.IOS
			) && Target.Type != TargetType.Server && !Target.bBuildEditor;

		if (EnableVivox) {
			PublicDependencyModuleNames.AddRange(new[] {
				"VivoxCore"
			});
			// PublicDefinitions.Add("WITH_VIVOX=1");
		} else {
			// PublicDefinitions.Add("WITH_VIVOX=0");
		}

		// bool EnableChat = Target.Type != TargetType.Server;
		// if (EnableChat) {
		// 	PublicDefinitions.Add("WITH_RPC_CHAT=1");
		// } else {
		// 	PublicDefinitions.Add("WITH_RPC_CHAT=0");
		// }
		// Artheon END

		// VR support.
		bool EnableVR = Target.Platform == UnrealTargetPlatform.Android ||
		                Target.Platform == UnrealTargetPlatform.Win64 || Target.Platform == UnrealTargetPlatform.Mac;

		if (EnableVR) {
			PublicDependencyModuleNames.AddRange(new[] {
				"NavigationSystem",
				"RHI",
				"HeadMountedDisplay"
			});

			if (Target.Platform == UnrealTargetPlatform.Win64) {
				PublicDependencyModuleNames.AddRange(new[] {
					"SteamVR",
					"SteamVRInput",
					"OculusHMD"
				});
			}
		}

		// Android support.
		if (Target.Platform == UnrealTargetPlatform.Android) {
			PublicDependencyModuleNames.AddRange(new[] {
				"AndroidPermission"
			});
		}
	}
}
