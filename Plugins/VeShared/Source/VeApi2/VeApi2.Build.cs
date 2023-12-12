// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

using UnrealBuildTool;

public class VeApi2 : ModuleRules
{
    public VeApi2(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        CppStandard = CppStandardVersion.Cpp17;

        PrivateDefinitions.Add("VE_LOG_CATEGORY=LogVeApi2");
        PrivateDefinitions.Add("VE_LOG_MODULE=VeApi2");
        
        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
	            "CoreUObject",
                "Engine",
                "Json",
                "HTTP",
                "VeShared",
                "VeMonitoring", 
                "VeApi"
            }
        );

        PublicIncludePathModuleNames.AddRange(new string[] { "VeDownload" });
        PublicIncludePaths.AddRange(new string[] { ModuleDirectory });
    }
}