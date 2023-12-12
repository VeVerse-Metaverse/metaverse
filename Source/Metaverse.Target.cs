// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class MetaverseTarget : TargetRules
{
	public MetaverseTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;

		if (Configuration == UnrealTargetConfiguration.Shipping) {
			LinkType = TargetLinkType.Monolithic;
		}

		ExtraModuleNames.AddRange( new string[] { "Metaverse", "VeShared", "VeDownload", "VeUI" } );

		if (Configuration == UnrealTargetConfiguration.Shipping && (Platform == UnrealTargetPlatform.Linux || Platform == UnrealTargetPlatform.Win64)) {
			bUseLoggingInShipping = false;
			bOverrideBuildEnvironment = false;
		}

		if (Platform == UnrealTargetPlatform.Linux || Platform == UnrealTargetPlatform.Mac || Platform == UnrealTargetPlatform.IOS) {
			bForceEnableExceptions = true;
		}
	}
}
