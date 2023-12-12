// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.


#include "VeWorldMetadata.h"

#include "VeFileMetadata.h"
#include "VeShared.h"

FVeWorldMetadata::FVeWorldMetadata(const FApiSpaceMetadata& InMetadata) {
	Id = InMetadata.Id;
	Description = InMetadata.Description;
	Name = InMetadata.Name;
	Map = InMetadata.Map;
	GameMode = InMetadata.GameMode;
	bPublic = InMetadata.bPublic;
	Views = InMetadata.Views;
	TotalLikes = InMetadata.TotalLikes;
	TotalDislikes = InMetadata.TotalDislikes;
	Likes = InMetadata.Likes;
	Dislikes = InMetadata.Dislikes;
	Liked = InMetadata.Liked;
	bPublic = InMetadata.bPublic;

	if (InMetadata.Package) {
		Package = FVePackageMetadata(*InMetadata.Package);
	}

	Owner = FVeUserMetadata(InMetadata.Owner);

	Files = {};
	for (const auto& InFile : InMetadata.Files) {
		Files.Add(FVeFileMetadata(InFile));
	}
}

FVeWorldMetadata::operator FApiUpdateSpaceMetadata() const {
	FApiUpdateSpaceMetadata ApiUpdateSpaceMetadata;

	ApiUpdateSpaceMetadata.Id = Id;
	ApiUpdateSpaceMetadata.Name = Name;
	ApiUpdateSpaceMetadata.Description = Description;
	ApiUpdateSpaceMetadata.Map = Map;
	ApiUpdateSpaceMetadata.GameMode = GameMode;
	ApiUpdateSpaceMetadata.PackageId = Package.Id;
	ApiUpdateSpaceMetadata.bPublic = bPublic;

	return ApiUpdateSpaceMetadata;
}

FString FVeWorldMetadata::GetPakUrl(const FVeWorldMetadata& WorldMetadata) {
	const FVeFileMetadata* Result = WorldMetadata.Package.Files.FindByPredicate([=](const FVeFileMetadata& InFileMetadata) {
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

FGuid FVeWorldMetadata::GetPakId(const FVeWorldMetadata& WorldMetadata) {
	const auto* Result = WorldMetadata.Package.Files.FindByPredicate([=](const FVeFileMetadata& InFileMetadata) {
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

TArray<FVeWorldMetadata> FVeWorldMetadata::MakeArray(const TArray<TSharedPtr<FApiSpaceMetadata>>& rhs) {
	TArray<FVeWorldMetadata> Result;
	for (auto& Record : rhs) {
		Result.Emplace(*Record);
	}
	return Result;
}

//==============================================================================

// TArray<UObject*> UVeSpaceMetadataObject::BatchToUObjectArray(TArray<FVeSpaceMetadata> InEntities, UObject* InParent) {
// 	TArray<UObject*> Objects = TArray<UObject*>();
// 	for (const auto& Entity : InEntities) {
// 		auto* Object = NewObject<UVeSpaceMetadataObject>(InParent);
// 		Object->Metadata = Entity;
// 		Objects.Add(Object);
// 	}
// 	return Objects;
// }
