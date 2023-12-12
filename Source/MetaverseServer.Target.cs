// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class MetaverseServerTarget : TargetRules
{
	public MetaverseServerTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Server;
		DefaultBuildSettings = BuildSettingsVersion.V2;

		ExtraModuleNames.AddRange( new string[] { "Metaverse", "VeShared", "VeDownload", "VeUI" } );

		if (Configuration == UnrealTargetConfiguration.Shipping) {
			bUseLoggingInShipping = true;
		}

		if (Platform == UnrealTargetPlatform.Linux || Platform == UnrealTargetPlatform.Mac || Platform == UnrealTargetPlatform.IOS) {
			bForceEnableExceptions = true;
		}
	}
}
