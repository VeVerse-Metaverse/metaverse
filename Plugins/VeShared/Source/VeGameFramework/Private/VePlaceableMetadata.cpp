// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "VePlaceableMetadata.h"

#include "ApiPlaceableMetadata.h"
#include "VeGameFramework.h"
#include "VeShared.h"

// Map file types to class references
TMap<FString, FString> ClassMapping = {
	{
		TEXT("pdf"), TEXT("/VeGame/Shared/Placeables/BP_PlaceablePDF.BP_PlaceablePDF_C")
	},
	{
		TEXT("youtube"), TEXT("/UTubeVideoPlayer/Content/Utube/UTubePlayer/BP_PlaceableYoutube.BP_PlaceableYoutube_C")
	}
};

FVePlaceableClassMetadata::FVePlaceableClassMetadata(const FApiPlaceableClassMetadata& ApiMetadata)
	: Id(ApiMetadata.Id),
	ClassName(ApiMetadata.Class),
	Name(ApiMetadata.Name),
	Description(ApiMetadata.Description),
	Category(ApiMetadata.Category) {

	for (auto File : ApiMetadata.Files) {
		Files.Add(FVeFileMetadata(File));
	}

	Class = FindObject<UClass>(nullptr, *ClassName);
	if (!Class) {
		Class = LoadObject<UClass>(nullptr, *ClassName);
	}
}

FVePlaceableMetadata::FVePlaceableMetadata(const FApiPlaceableMetadata& ApiMetadata)
	: Id(ApiMetadata.Id),
	  Position(ApiMetadata.Position),
	  Rotation(ApiMetadata.Rotation),
	  Scale(ApiMetadata.Scale),
	  EntityId(ApiMetadata.EntityId),
	  Type(ApiMetadata.Type) {

	for (auto File : ApiMetadata.Files) {
		Files.Add(FVeFileMetadata(File));
	}

	Owner = FVeUserMetadata(ApiMetadata.Owner);

	if (ClassMapping.Contains(Type)) {
		Type = ClassMapping[Type];
	}

	PlaceableClassMetadata = FVePlaceableClassMetadata(ApiMetadata.PlaceableClassMetadata);
	const auto ClassName = PlaceableClassMetadata.ClassName;

	Class = FindObject<UClass>(nullptr, *ClassName);
	if (!Class) {
		Class = LoadObject<UClass>(nullptr, *ClassName);
	}

	if (!IsValid(Class)) {
		VeLogErrorFunc("Failed to get class: %s", *ClassName);
	}

	Properties.Reserve(ApiMetadata.Properties.Num());
	for (auto ApiProperty : ApiMetadata.Properties) {
		Properties.Emplace(FVePropertyMetadata(ApiProperty));
	}
}
