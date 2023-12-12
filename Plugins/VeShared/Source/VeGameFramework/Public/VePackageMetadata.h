// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "VeFileMetadata.h"
#include "VeUserMetadata.h"
// #include "VeSpaceMetadata.h"
#include "VePackageMetadata.generated.h"

class FApiPackageMetadata;
class FApiUpdatePackageMetadata;

/**
 *  See also: EApiMetaversePlatforms
 */
UENUM(BlueprintType)
enum class EVeMetaversePlatforms : uint8 {
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

/**
 * 
 */
USTRUCT(BlueprintType)
struct VEGAMEFRAMEWORK_API FVePackageMetadata {
	GENERATED_BODY()

	FVePackageMetadata();
	explicit FVePackageMetadata(const FApiPackageMetadata& InMetadata);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGuid Id;

	/** List of files of the metaverse, can include previews, pak files, etc. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FVeFileMetadata> Files;

	/** List of tags of the metaverse. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FString> Tags;

	/** List of supported platforms. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<EVeMetaversePlatforms> Platforms;

	/** Name of the metaverse (directory). */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Name = TEXT("");

	/** Title of the metaverse. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Title = TEXT("");

	/** Short description of the metaverse. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Summary = TEXT("");

	/** Full markdown description of the metaverse. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Description = TEXT("");

	/** Base release version name. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString ReleaseName = TEXT("1.0.0");

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Map = TEXT("");

	/** Current version of the mod. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Version;

	/** Metaverse release date. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FDateTime ReleasedAt = FDateTime();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Price = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float DownloadSize = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVeUserMetadata Owner;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Views;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Downloads;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Likes;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Dislikes;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool Public = true;

	FApiUpdatePackageMetadata ToUpdateMetadata() const;

	static FString GetPakUrl(const FVePackageMetadata& InMetaverseMetadata);
	static FGuid GetPakId(const FVePackageMetadata& InMetaverseMetadata);
	static FString GetPakPath(const FVePackageMetadata& InMetaverseMetadata);
};

typedef TDelegate<void(const FVePackageMetadata& InMetadata, const bool bInSuccessful, const FString& InError)> FOnVeMetaverseRequestCompleted;