// Author: Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

using UnrealBuildTool;

public class Metaverse : ModuleRules {
	public Metaverse(ReadOnlyTargetRules Target) : base(Target) {
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		CppStandard = CppStandardVersion.Cpp17;

		PrivateDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "VeShared", "VeApi", "VeApi2", "VeUI" });
		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		if (Target.Platform == UnrealTargetPlatform.IOS || Target.Platform == UnrealTargetPlatform.Mac || Target.Platform == UnrealTargetPlatform.Android) {
			PrivateDependencyModuleNames.Add("Bugsnag");
		}

		if (Target.Platform == UnrealTargetPlatform.Android) {
			PrivateDependencyModuleNames.Add("AndroidPermission");
		}

#if UE_4_24_OR_LATER
		bUseUnity = false;
#else
		bFasterWithoutUnity = true;
#endif
	}
}
