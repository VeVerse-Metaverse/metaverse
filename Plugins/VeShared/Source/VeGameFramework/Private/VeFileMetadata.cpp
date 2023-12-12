// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.


#include "VeFileMetadata.h"

#include "ApiFileMetadata.h"

FVeFileMetadata::FVeFileMetadata()
	: Version(0), Variation(0), Type(), Size(0), Width(0), Height(0) {}

FVeFileMetadata::FVeFileMetadata(const FApiFileMetadata& InFileMetadata)
	: Id(InFileMetadata.Id),
	  Version(InFileMetadata.Version),
	  Variation(InFileMetadata.Variation),
	  UploadedBy(InFileMetadata.UploadedBy),
	  EntityId(InFileMetadata.EntityId),
	  Url(InFileMetadata.Url),
	  Mime(InFileMetadata.Mime),
	  CreatedAt(InFileMetadata.CreatedAt),
	  UpdatedAt(InFileMetadata.UpdatedAt),
	  Type(InFileMetadata.Type),
	  Platform(InFileMetadata.Platform),
	  Size(InFileMetadata.Size),
	  Width(InFileMetadata.Width),
	  Height(InFileMetadata.Height),
	  DeploymentType(InFileMetadata.DeploymentType),
	  OriginalPath(InFileMetadata.OriginalPath),
	  Hash(InFileMetadata.Hash) {}

const FVeFileMetadata* FindFileMetadata(const TArray<FVeFileMetadata>& Files, EApiFileType Type) {
	return Files.FindByPredicate([Type](const FVeFileMetadata& File) {
		return File.Type == Type;
	});
}

const FVeFileMetadata* FindFileMetadata(const TArray<FVeFileMetadata>& Files, const TArray<EApiFileType>& Types) {
	const FVeFileMetadata* FileMetadata = nullptr;
	for (auto Type : Types) {
		FileMetadata = Files.FindByPredicate([Type](const FVeFileMetadata& File) {
			return File.Type == Type;
		});
		if (FileMetadata) {
			return FileMetadata;
		}
	}
	return nullptr;
}
