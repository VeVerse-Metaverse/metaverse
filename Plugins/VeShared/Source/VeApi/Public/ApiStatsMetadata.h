// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "ApiEntityMetadata.h"

/** Space metadata descriptor. */
class VEAPI_API FApiStatsMetadata final : public IApiMetadata {
public:
	TSharedPtr<class FApiSpaceMetadata> LatestWorld = nullptr;
	TSharedPtr<class FApiPackageMetadata> LatestPackage = nullptr;
	FString LatestUser = {};
	int32 TotalUsers = 0;
	int32 OnlineUsers = 0;
	int32 OnlineServers = 0;
	int32 TotalPackages = 0;
	int32 TotalWorlds = 0;
	int32 TotalLikes = 0;
	int32 TotalObjects = 0;
	int32 TotalNFTs = 0;
	int32 TotalPlaceables = 0;

	virtual bool FromJson(const TSharedPtr<FJsonObject> InJsonObject) override;
};
