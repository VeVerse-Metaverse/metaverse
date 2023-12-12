// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "MetadataFile.h"

#include "VeShared.h"
#include "VeDownload.h"


bool FMetadataFile::Serialize(FArchive& Ar) {
	EMetadataFileType MetadataType = GetMetadataFileType();

	Ar << MetadataType;
	if (MetadataType != GetMetadataFileType()) {
		return false;
	}

	return true;
}

bool FMetadataFile::LoadFromFile(const FString& Filename) {
	TArray<uint8> Buffer;

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	if (auto* FileHandle = PlatformFile.OpenRead(*Filename, false)) {
		Buffer.AddUninitialized(FileHandle->Size());
		if (!FileHandle->Read(Buffer.GetData(), FileHandle->Size())) {
			VeLogErrorFunc("Failed to read from file: %s", *Filename);
			delete FileHandle; // don't forget
			Empty();
			return false;
		}
		delete FileHandle; // don't forget
	} else {
		Empty();
		return false;
	}

	FMemoryReader ArReader(Buffer);
	return Serialize(ArReader);
}

bool FMetadataFile::SaveToFile(const FString& Filename) {
	TArray<uint8> Storage;
	FMemoryWriter ArWriter(Storage);

	if (!Serialize(ArWriter)) {
		return false;
	}

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	if (auto* FileHandle = PlatformFile.OpenWrite(*Filename, false, false)) {
		if (!FileHandle->Write(Storage.GetData(), Storage.Num())) {
			VeLogErrorFunc("Failed to write to file: %s", *Filename);
			delete FileHandle; // don't forget
			return false;
		}
		delete FileHandle; // don't forget
	} else {
		VeLogErrorFunc("Failed to open a file for writing: %s", *Filename);
		return false;
	}

	return true;
}
