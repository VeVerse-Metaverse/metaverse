#pragma once

#include "CoreMinimal.h"


struct VESHARED_API FPackageNameInfo {
	FString PluginName;
	FString AssetPath;

	FPackageNameInfo() = default;
	FPackageNameInfo(const FString& InPluginName, const FString& InAssetPath)
		: PluginName(InPluginName), AssetPath(InAssetPath)
	{}
};


class VESHARED_API FVePathUtils {
public:
	/** "/<PluginName>/<AssetPath>" to "/<PluginName>/<AssetPath>" */
	static const FString& SpaceMapToPackageName(const FString& SpaceMap);
	
	/** Parse "/<PluginName>/<AssetPath>" */
	static FPackageNameInfo GetPackageNameInfo(const FString& PackageName);

	/** To "/<PluginName>/<AssetPath>" */
	static FString GetPackageName(const FString& ModuleName, FString AssetPath);

	/** "/<PluginName>/<AssetPath>" to "/<AssetPath>" (without extension) */
	static FString PackageNameToAssetPath(const FString& PackageName);

};
