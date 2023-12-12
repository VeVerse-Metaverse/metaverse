// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

using System.Linq;
using UnrealBuildTool;

public class VeShared : ModuleRules {
	public VeShared(ReadOnlyTargetRules Target) : base(Target) {
		PublicDependencyModuleNames.AddRange(new string[] { "HTTP" });
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		CppStandard = CppStandardVersion.Cpp17;

		PrivateDefinitions.Add("VE_LOG_CATEGORY=LogVeShared");
		PrivateDefinitions.Add("VE_LOG_MODULE=VeShared");

		PrivateDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"RHI",
			"HTTP",
			"Slate",
		});

		if (Target.Platform == UnrealTargetPlatform.Android) {
			PublicDependencyModuleNames.Add("AndroidPermission");
		}

		if (Target.bBuildEditor) {
			PrivateDependencyModuleNames.Add("UnrealEd");
		}

		PublicDefinitions.Add("WITH_SSL=1");

		PrivateDefinitions.Add("LOG_HTTP_REQUEST_CONTENT=0");

		// Voice support, enabled for desktop and mobile platforms, disabled in editor and server builds.
		bool EnableVivox = (Target.Platform == UnrealTargetPlatform.Win64
		                    || Target.Platform == UnrealTargetPlatform.Mac
			                   // || Target.Platform == UnrealTargetPlatform.Android
			                   // || Target.Platform == UnrealTargetPlatform.IOS
		                   ) && Target.Type != TargetType.Server &&
		                   Target.Type != TargetType.Editor;

		// Disable QR code generator support on servers.
		bool EnableQrCode = Target.Type != TargetType.Server;

		// Disable pak support for servers.
		bool EnablePak = Target.Type != TargetType.Editor;

		// Disable proprietary plugins when building SDK
		if (Target.ProjectDefinitions.Contains("BuildSDK")) {
			EnableVivox = false;
			EnableQrCode = false;
		}

		PublicDefinitions.Add($"WITH_VIVOX={(EnableVivox ? 1 : 0)}");
		PublicDefinitions.Add($"WITH_QRCODE={(EnableQrCode ? 1 : 0)}");
		PublicDefinitions.Add($"WITH_PAK={(EnablePak ? 1 : 0)}");
	}
}
