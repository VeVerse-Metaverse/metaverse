// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "PortalLib.h"

#include "VeGameFramework.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "PakPakSubsystem.h"
#include "VeApi.h"
#include "ApiSpaceSubsystem.h"


bool UPortalLib::IsAssetExists(const FString& PackageName) {
	auto& Registry = FAssetRegistryModule::GetRegistry();

	TArray<FAssetData> OutAssetData;
	if (Registry.GetAssetsByPackageName(FName(PackageName), OutAssetData)) {
		return static_cast<bool>(OutAssetData.Num());
	}

	return false;
}
