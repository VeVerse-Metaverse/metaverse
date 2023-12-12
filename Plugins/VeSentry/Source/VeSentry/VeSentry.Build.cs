// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

using UnrealBuildTool;

public class VeSentry : ModuleRules {
	public VeSentry(ReadOnlyTargetRules Target) : base(Target) {
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		PrecompileForTargets = PrecompileTargetsType.Any;
		
		// PublicIncludePaths.AddRange(
		// 	new string[] {
		// 		string.Format("{0}/Public", ModuleDirectory),
		// 	}
		// );
		//
		// PrivateIncludePaths.AddRange(
		// 	new string[] {
		// 		string.Format("{0}/Private", ModuleDirectory),
		// 	}
		// );


		PublicDependencyModuleNames.AddRange(
			new string[] {
				"Core",
				"CoreUObject",
				"Engine",
				"Projects",
				"SentryLibrary",
			}
		);


		PrivateDependencyModuleNames.AddRange(
			new string[] {
				// "Asio"
				// ... add private dependencies that you statically link with here ...
			}
		);


		DynamicallyLoadedModuleNames.AddRange(
			new string[] {
			}
		);

		if (Target.Platform == UnrealTargetPlatform.Linux || Target.Platform == UnrealTargetPlatform.Win64) {
			PublicDefinitions.Add("WITH_SENTRY=1");
		} else {
			PublicDefinitions.Add("WITH_SENTRY=0");
		}

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            string sentryDll = string.Format("{0}\\..\\ThirdParty\\SentryLibrary\\Windows\\sentry.dll", ModuleDirectory);

            if (!System.IO.File.Exists(sentryDll))
            {
                System.Diagnostics.Debug.WriteLine(string.Format("Warning: {0} does not exist", sentryDll));
            }

            RuntimeDependencies.Add(sentryDll);
        }
        else if (Target.Platform == UnrealTargetPlatform.Linux) 
        {
	        string sentrySo = string.Format("{0}/../ThirdParty/SentryLibrary/Linux/sentry.so", ModuleDirectory);

	        if (!System.IO.File.Exists(sentrySo)) 
	        {
		        System.Diagnostics.Debug.WriteLine(string.Format("Warning: {0} does not exist", sentrySo));
	        }
        }

        // get base folder
		// string ModulePath = ModuleDirectory.Replace('\\', '/');
		
		// add sdk include paths
		// string IncludeDir = Path.Combine(ModulePath, "sdk", "include");
		// PublicIncludePaths.Add(IncludeDir);
		
		// // add sdk src paths (source code license only)
		// string SrcDir = Path.Combine(ModulePath, "sdk", "src");
		// PrivateIncludePaths.Add(SrcDir);
	}
}
