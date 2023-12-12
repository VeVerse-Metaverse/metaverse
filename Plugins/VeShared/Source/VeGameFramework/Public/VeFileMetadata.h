// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "ApiFileMetadata.h"
#include "VeFileMetadata.generated.h"

enum class EApiFileType : uint8;
class FApiFileMetadata;

/**
 * 
 */
USTRUCT(BlueprintType)
struct VEGAMEFRAMEWORK_API FVeFileMetadata {
	GENERATED_BODY()

	/** Unique ID of the entity. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGuid Id;

	/** Current version of the file. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Version;

	/** Current version of the file. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Variation;

	/** ID of uploader. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGuid UploadedBy;

	/** ID of the parent entity. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGuid EntityId;

	/** File cloud URL. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Url;

	/** Mime type of the file. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Mime;

	/** File create date and time. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDateTime CreatedAt = FDateTime();

	/** File update date and time. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDateTime UpdatedAt = FDateTime();

	/** File type. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EApiFileType Type;

	/** File platform. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Platform;

	/** File size. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int64 Size;

	/** Media file pixel width. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Width;

	/** Media file pixel height. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Height;

	/** Type of file "Server" or "Client" */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EApiFileDeploymentType DeploymentType = EApiFileDeploymentType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString OriginalPath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Hash;

	FVeFileMetadata();
	explicit FVeFileMetadata(const FApiFileMetadata& InFileMetadata);
};

VEGAMEFRAMEWORK_API const FVeFileMetadata* FindFileMetadata(const TArray<FVeFileMetadata>& Files, EApiFileType Type);
VEGAMEFRAMEWORK_API const FVeFileMetadata* FindFileMetadata(const TArray<FVeFileMetadata>& Files, const TArray<EApiFileType>& Types);
