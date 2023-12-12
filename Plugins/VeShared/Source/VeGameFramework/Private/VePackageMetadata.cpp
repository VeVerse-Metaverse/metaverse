// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


#include "VePackageMetadata.h"

#include "ApiPackageMetadata.h"
#include "VeFileMetadata.h"
#include "VeShared.h"

FVePackageMetadata::FVePackageMetadata(): ReleasedAt{}, Views(0), Downloads(0), Likes(0), Dislikes(0) {

}

FVePackageMetadata::FVePackageMetadata(const FApiPackageMetadata& InMetadata) {
	Id = InMetadata.Id;
	Name = InMetadata.Name;
	Title = InMetadata.Title;
	Summary = InMetadata.Summary;
	Description = InMetadata.Description;
	Map = InMetadata.Map;
	Version = InMetadata.Version;
	ReleasedAt = InMetadata.ReleasedAt;
	ReleaseName = InMetadata.ReleaseName;
	Public = InMetadata.bPublic;
	Tags = InMetadata.Tags;
	Price = InMetadata.Price;
	Views = InMetadata.Views;
	Downloads = InMetadata.Downloads;
	Likes = InMetadata.Likes;
	Dislikes = InMetadata.Dislikes;
	DownloadSize = InMetadata.DownloadSize;
	if (InMetadata.Owner) {
		Owner = FVeUserMetadata(*InMetadata.Owner);
	}

	for (const auto& Platform : InMetadata.Platforms) {
		Platforms.Add(static_cast<EVeMetaversePlatforms>(Platform));
	}
	
	Files = {};
	for (const auto& InFile : InMetadata.Files) {
		Files.Add(FVeFileMetadata(InFile));
	}
}

FString FVePackageMetadata::GetPakUrl(const FVePackageMetadata& InMetaverseMetadata) {
	const FVeFileMetadata* Result = InMetaverseMetadata.Files.FindByPredicate([=](const FVeFileMetadata& InFileMetadata) {
		if (IsRunningDedicatedServer()) {
			return InFileMetadata.Type == EApiFileType::Pak && InFileMetadata.Platform == FVeSharedModule::GetPlatformString() && InFileMetadata.DeploymentType == EApiFileDeploymentType::Server;
		}
		return InFileMetadata.Type == EApiFileType::Pak && InFileMetadata.Platform == FVeSharedModule::GetPlatformString() && InFileMetadata.DeploymentType == EApiFileDeploymentType::Client;
	});

	if (Result) {
		return Result->Url;
	}

	return {};
}

FGuid FVePackageMetadata::GetPakId(const FVePackageMetadata& InMetaverseMetadata) {
	const auto* Result = InMetaverseMetadata.Files.FindByPredicate([=](const FVeFileMetadata& InFileMetadata) {
		if (IsRunningDedicatedServer()) {
			return InFileMetadata.Type == EApiFileType::Pak && InFileMetadata.Platform == FVeSharedModule::GetPlatformString() && InFileMetadata.DeploymentType == EApiFileDeploymentType::Server;
		}
		return InFileMetadata.Type == EApiFileType::Pak && InFileMetadata.Platform == FVeSharedModule::GetPlatformString() && InFileMetadata.DeploymentType == EApiFileDeploymentType::Client;
	});

	if (Result) {
		return Result->Id;
	}

	return FGuid();
}

FString FVePackageMetadata::GetPakPath(const FVePackageMetadata& InMetaverseMetadata) {
	return FPaths::ConvertRelativePathToFull(FPaths::ProjectPersistentDownloadDir() / TEXT("Paks") / GetPakId(InMetaverseMetadata).ToString() + FString(TEXT(".pak")));
}

FApiUpdatePackageMetadata FVePackageMetadata::ToUpdateMetadata() const {
	FApiUpdatePackageMetadata Metadata;
	Metadata.Id = Id;
	Metadata.Title = Title;
	Metadata.Description = Description;
	Metadata.Summary = Summary;
	Metadata.Map = Map;
	Metadata.Version = Version;
	Metadata.ReleaseName = ReleaseName;
	Metadata.bPublic = Public;

	// Platforms
	for (const auto& Platform : Platforms) {
		Metadata.Platforms.Add(static_cast<EApiPackagePlatforms>(Platform));
	}

	return Metadata;
}
