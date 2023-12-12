#include "VePathUtils.h"

#include "Misc/App.h"
#include "Misc/Paths.h"

const FString& FVePathUtils::SpaceMapToPackageName(const FString& SpaceMap) {
	return SpaceMap;
}

FPackageNameInfo FVePathUtils::GetPackageNameInfo(const FString& PackageName) {
	if (PackageName.IsEmpty() || PackageName[0] != '/') {
		return {};
	}
		
	int32 ModulePosStop = PackageName.Find(TEXT("/"), ESearchCase::IgnoreCase, ESearchDir::FromStart, 1);

	return FPackageNameInfo(
			PackageName.Mid(1, ModulePosStop - 1),
			PackageName.Right(PackageName.Len() - ModulePosStop)
		);
}

FString FVePathUtils::GetPackageName(const FString& ModuleName, FString AssetPath) {
	AssetPath.RemoveFromStart(TEXT("/"));
	return FString::Printf(TEXT("/%s/%s"), *ModuleName, *FPaths::GetBaseFilename(AssetPath, false));
}

FString FVePathUtils::PackageNameToAssetPath(const FString& PackageName) {
	int32 ModuleStop = PackageName.Find(TEXT("/"), ESearchCase::IgnoreCase, ESearchDir::FromStart, 1);

	return FPaths::GetBaseFilename(PackageName.Right(PackageName.Len() - ModuleStop), false);
}
