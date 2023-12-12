using System.IO;
using System.Reflection;
using UnrealBuildTool;

public class SentryLibrary : ModuleRules {
	public SentryLibrary(ReadOnlyTargetRules Target) : base(Target) {
		Type = ModuleType.External;

		if (Target.Platform == UnrealTargetPlatform.Win64) {
			// Add the Win64 specific Include and Library paths
			PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "include"));
			string SentryLibraryPath = Path.Combine(ModuleDirectory, "Windows");

			PublicAdditionalLibraries.Add(Path.Combine(SentryLibraryPath, "sentry.lib"));

			// Delay-load the DLL, so we can load it from the right place
			PublicDelayLoadDLLs.Add("sentry.dll");
			RuntimeDependencies.Add(Path.Combine(SentryLibraryPath, "sentry.dll"));
		} else if (Target.Platform == UnrealTargetPlatform.Linux) {
			PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "include"));
			string SentryLibraryPath = Path.Combine(ModuleDirectory, "Linux");

			PublicAdditionalLibraries.Add(Path.Combine(SentryLibraryPath, "libsentry.a"));

			PublicDelayLoadDLLs.Add("libsentry.so");
			RuntimeDependencies.Add(Path.Combine(SentryLibraryPath, "libsentry.so"));
		}
	}
}
