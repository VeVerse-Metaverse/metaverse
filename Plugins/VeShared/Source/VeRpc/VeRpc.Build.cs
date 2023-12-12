// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

using UnrealBuildTool;

public class VeRpc : ModuleRules {
	public VeRpc(ReadOnlyTargetRules Target) : base(Target) {
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		CppStandard = CppStandardVersion.Cpp17;

		PrivateDefinitions.Add("VE_LOG_CATEGORY=LogVeShared");
		PrivateDefinitions.Add("VE_LOG_MODULE=VeRpc");

		PublicDependencyModuleNames.AddRange(new string[] {
			  "Core"
			, "CoreUObject"
			, "Engine"
			, "VeShared"
			, "VeApi"
			, "VeApi2"
			, "VeSocial"
			, "VeGameFramework"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		bool EnableWebsocketRpc = Target.Type == TargetType.Editor || Target.Type == TargetType.Server || Target.Type == TargetType.Game || Target.Type == TargetType.Client;
		if (EnableWebsocketRpc) {
			PublicDefinitions.Add("WITH_SSL=1");
			PublicDefinitions.Add("WITH_WEBSOCKETS=1");
			PublicDefinitions.Add("WITH_LIBWEBSOCKETS=1");
			PublicDefinitions.Add("WITH_RPC=1");

			if (Target.Type != TargetType.Editor) {
				PublicDefinitions.Add("USE_VERPC_VIVOX=1");
			} else {
				PublicDefinitions.Add("USE_VERPC_VIVOX=0");
			}

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

	}
}
