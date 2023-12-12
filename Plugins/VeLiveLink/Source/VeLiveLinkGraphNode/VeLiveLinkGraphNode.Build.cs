// Author: Nikolay Bulatov, Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

using UnrealBuildTool;

public class VeLiveLinkGraphNode : ModuleRules
{
    public VeLiveLinkGraphNode(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {

            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "AnimationCore",
                "AnimGraphRuntime",
                "Core",
                "CoreUObject",
                "Engine",
                "InputCore",
                "KismetCompiler",
                "LiveLink",
                // "LiveLinkAnimationCore",
                // "LiveLinkInterface",
                "Persona",
                "SlateCore",
                "VeLiveLink",
            }
        );

        if (Target.bBuildEditor == true)
        {
            PublicDependencyModuleNames.AddRange(
                new string[]
                {
                    "AnimGraph",
                }
            );

            PrivateDependencyModuleNames.AddRange(
                new string[]
                {
                    "EditorFramework",
                    "UnrealEd",
                    "Kismet",
                    "BlueprintGraph",
                }
            );
        }
    }
}