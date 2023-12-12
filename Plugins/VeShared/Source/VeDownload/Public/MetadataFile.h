// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

enum class EMetadataFileType : uint8 {
	Unknown = 0,
	HttpDownload = 1,
	FileStorage = 2,
};


class VEDOWNLOAD_API FMetadataFile {
public:
	FMetadataFile() = default;
	virtual ~FMetadataFile() = default;

	virtual void Empty() = 0;

	virtual bool Serialize(FArchive& Ar);

	bool LoadFromFile(const FString& Filename);
	bool SaveToFile(const FString& Filename);

protected:
	virtual EMetadataFileType GetMetadataFileType() const = 0;
};
