// Author: Egor A. Pristavka

using UnrealBuildTool;

public class VePak : ModuleRules {
	public VePak(ReadOnlyTargetRules Target) : base(Target) {
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivateDependencyModuleNames.AddRange(
			new string[] {
				"Core",
				"CoreUObject",
				"Engine",
				"PakFile",
				"VeShared",
				"VeApi",
				"VeMonitoring",
				"PakLoader"
			}
		);
	}
}
