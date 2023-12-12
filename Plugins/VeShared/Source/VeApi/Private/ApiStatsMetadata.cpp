// Authors: Egor A. Pristavka, Khusan T. Yadgarov. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "ApiStatsMetadata.h"

#include "ApiSpaceMetadata.h"
#include "ApiPackageMetadata.h"

typedef TSharedPtr<FJsonValue> FJsonValuePtr;
typedef TSharedPtr<FJsonObject> FJsonObjectPtr;
typedef TArray<TSharedPtr<FJsonValue>> FJsonValuePtrArray;

bool FApiStatsMetadata::FromJson(const TSharedPtr<FJsonObject> InJsonObject) {
	bool bOk = IApiMetadata::FromJson(InJsonObject);
	LatestUser = GetJsonStringField(InJsonObject, TEXT("latestUser"));

	TotalUsers = GetJsonIntField(InJsonObject, TEXT("totalUsers"));
	OnlineUsers = GetJsonIntField(InJsonObject, TEXT("onlineUsers"));
	OnlineServers = GetJsonIntField(InJsonObject, TEXT("onlineServers"));
	TotalPackages = GetJsonIntField(InJsonObject, TEXT("totalPackages"));
	TotalWorlds = GetJsonIntField(InJsonObject, TEXT("totalWorlds"));
	TotalLikes = GetJsonIntField(InJsonObject, TEXT("totalLikes"));
	TotalObjects = GetJsonIntField(InJsonObject, TEXT("totalObjects"));
	TotalNFTs = GetJsonIntField(InJsonObject, TEXT("totalNfts"));
	TotalPlaceables = GetJsonIntField(InJsonObject, TEXT("totalPlaceables"));

	{
		LatestWorld = MakeShared<FApiSpaceMetadata>();
		if (const auto JsonObject = GetJsonObjectField(InJsonObject, TEXT("latestWorld")); JsonObject.IsValid()) {
			bOk &= LatestWorld->FromJson(JsonObject);
		}
	}

	{
		LatestPackage = MakeShared<FApiPackageMetadata>();
		if (const auto JsonObject = GetJsonObjectField(InJsonObject, TEXT("latestPackage")); JsonObject.IsValid()) {
			bOk &= LatestPackage->FromJson(JsonObject);
		}
	}

	return bOk;
}
