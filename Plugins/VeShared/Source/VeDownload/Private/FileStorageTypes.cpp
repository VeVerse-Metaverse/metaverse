// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "FileStorageTypes.h"

void FFileStorageMetadata::Empty() {
	ContentLength = 0;
	LastModified = FDateTime();
	ExpireTime = FDateTime();
	Hash.Empty();
}

bool FFileStorageMetadata::Serialize(FArchive& Ar) {
	if (!FMetadataFile::Serialize(Ar)) {
		return false;
	}

	Ar << MetadataVersion;
	if (MetadataVersion == 2) {
		TimeStamp = FDateTime::UtcNow();

		Ar << TimeStamp;
		Ar << ContentLength;
		Ar << LastModified;
		Ar << ExpireTime;
		Ar << Hash;
		return true;
	}

	return false;

}
