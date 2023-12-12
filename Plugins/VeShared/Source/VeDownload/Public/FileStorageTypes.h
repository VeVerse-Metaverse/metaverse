// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "MetadataFile.h"
#include "HttpDownloadTypes.h"
#include "FileStorageTypes.generated.h"


class VEDOWNLOAD_API FFileStorageMetadata : public FMetadataFile {
public:
	virtual void Empty() override;
	virtual bool Serialize(FArchive& Ar) override;

	int64 ContentLength = 0;
	FDateTime LastModified;
	FDateTime ExpireTime;
	FString Hash;

	FDateTime GetTimeStamp() const { return TimeStamp; }

protected:
	virtual EMetadataFileType GetMetadataFileType() const override { return EMetadataFileType::FileStorage; }

private:
	uint8 MetadataVersion = 2;
	FDateTime TimeStamp;
};


USTRUCT(BlueprintType)
struct FFileStorageProgressMetadata {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	int64 ContentLength = 0;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	int64 DownloadedSize = 0;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	float Progress = 0.0f;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	int32 BytesPerSec = 0;
};

USTRUCT(BlueprintType)
struct FFileStorageResultMetadata {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	bool bSuccessful = false;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FString Error;

	/** Total content length. */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	int64 ContentLength = 0;

	/** Path where file will be placed. */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FString Filename;
};

USTRUCT(BlueprintType)
struct FBaseStorageItemOptions {
	GENERATED_BODY()

	bool DeleteCache = false;

	float DelayForCache = 0.0f;
};


// typedef TMulticastDelegate<void(const FFileStorageProgressMetadata&)> FOnFileStorageProgress;
// typedef TMulticastDelegate<void(const FFileStorageResultMetadata&)> FOnFileStorageComplete;
