// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "CoreMinimal.h"


class VEDOWNLOAD_API FStorageItemId {
public:
	explicit FStorageItemId(const FString& InUrl)
		: Url(InUrl) {
		FSHA1 HashBytes;
		HashBytes.UpdateWithString(*Url, Url.Len());
		Id = HashBytes.Finalize().ToString();
	}

	FStorageItemId(const FGuid& InFileId, const FGuid& InEntityId)
		: FileId(InFileId),
		  EntityId(InEntityId) {
		FSHA1 HashBytes;
		// HashBytes.Update(reinterpret_cast<const uint8*>(&InEntityId), sizeof(InEntityId));
		// HashBytes.Update(reinterpret_cast<const uint8*>(&FileId), sizeof(FileId));
		FString EntityIdStr(InEntityId.ToString(EGuidFormats::Digits));
		FString FileIdStr(InFileId.ToString(EGuidFormats::Digits));
		HashBytes.UpdateWithString(*EntityIdStr, EntityIdStr.Len());
		HashBytes.UpdateWithString(*FileIdStr, FileIdStr.Len());
		Id = HashBytes.Finalize().ToString();
	}

	const FString& GetUrl() const { return Url; }
	const FGuid& GetEntityId() const { return EntityId; }
	const FGuid& GetFileId() const { return FileId; }
	const FString& GetId() const { return Id; }

	FString ToString() const {
		if (Url.IsEmpty()) {
			return FString::Printf(TEXT("{Id: %s; EntityId: %s; FileId: %s}"),
				*Id,
				*EntityId.ToString(EGuidFormats::DigitsWithHyphensLower),
				*FileId.ToString(EGuidFormats::DigitsWithHyphensLower)
				);
		}
		return FString::Printf(TEXT("{Id: %s; Url: %s}"), *Id, *Url);
	}

private:
	/** URL from where the file will be downloaded. */
	FString Url;

	/** File ID for the pre-signed request. */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FGuid FileId;

	/** Entity ID for the pre-signed request. */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FGuid EntityId;

	FString Id;
};
