// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once
#include "ApiEntityMetadata.h"
#include "ApiFileMetadata.h"
#include "ApiLinkMetadata.h"
#include "ApiUserMetadata.h"

class FApiSpaceMetadata;
class UApiPackageMetadataObject;

enum class EApiPackagePlatforms : uint8 {
	Unknown,
	Windows,
	Mac,
	Linux,
	SteamVR,
	OculusVR,
	OculusQuest,
	IOS,
	Android
};

class VEAPI_API FApiPackageMetadata final : public FApiEntityMetadata {
public:
	/** List of files of the metaverse, can include previews, pak files, etc. */
	TArray<FApiFileMetadata> Files;

	/** List of tags of the metaverse. */
	TArray<FString> Tags;

	/** List of links to the website, social networks, community, etc. */
	TArray<FApiLinkMetadata> Links;

	/** List of supported platforms. */
	TArray<EApiPackagePlatforms> Platforms;

	/** Owner and developer of the metaverse, user who uploaded the metaverse. */
	TSharedPtr<FApiUserMetadata> Owner;

	/** Name of the metaverse (directory). */
	FString Name = TEXT("");

	/** Name of the metaverse (directory). */
	FString Title = TEXT("");

	/** Short description of the metaverse. */
	FString Summary = TEXT("");

	/** Full markdown description of the metaverse. */
	FString Description = TEXT("");

	/** Base release version name. */
	FString ReleaseName = TEXT("1.0.0");

	FString Map = TEXT("");

	/** Current version of the metaverse. */
	FString Version;

	/** Metaverse release date. */
	FDateTime ReleasedAt = FDateTime();

	/** Total number of bytes to download. */
	int64 DownloadSize;

	/** Total number of downloads. */
	int32 Downloads;

	/** Total number of likes. */
	int32 Likes;

	/** Total number of dislikes. */
	int32 Dislikes;

	/** Price, zero if free. */
	float Price = 0;

	/** Discount for the current price in the range of 0-1. */
	float Discount = 0;

	/** Is the metaverse has been purchased by the current user or not. */
	bool bPurchased = false;

	/** Governance DAO.  */
	FString GovernedBy;

	/** Amount of staked tokens. */
	int64 Staked;

	/** Name of the token. */
	FString Token;

	/** Amount of creator rewards. */
	int64 CreatorRewards;

	/** DAO proposals. */
	TArray<FString> Proposals;

	/** List of spaces of the metaverse. */
	TArray<TSharedPtr<FApiSpaceMetadata>> Spaces;

	virtual bool FromJson(const TSharedPtr<FJsonObject> InJsonObject) override;
};

/** Used to create and update mod requests. */
class VEAPI_API FApiUpdatePackageMetadata final : public IApiMetadata {
public:
	FGuid Id = FGuid();
	FString Title = TEXT("");
	FString Description = TEXT("");
	FString Summary = TEXT("");
	FString Map = TEXT("");
	FString Version = TEXT("");
	FString ReleaseName = TEXT("1.0.0");
	bool bPublic = true;

	TArray<EApiPackagePlatforms> Platforms;
	// float Price = 0;
	// float Discount = 0;

	bool operator==(const FApiUpdatePackageMetadata& rhsMetadata) const;
	bool operator!=(const FApiUpdatePackageMetadata& rhsMetadata) const;

	FApiUpdatePackageMetadata() = default;
	explicit FApiUpdatePackageMetadata(const FApiPackageMetadata& InMetadata);

	virtual TSharedPtr<FJsonObject> ToJson() override;
};

typedef TApiBatchMetadata<class FApiPackageMetadata/*, class UApiMetaverseMetadataObject*/> FApiPackageBatchMetadata;
