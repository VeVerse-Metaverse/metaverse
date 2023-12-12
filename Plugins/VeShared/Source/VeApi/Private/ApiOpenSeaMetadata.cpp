// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "ApiOpenSeaMetadata.h"

#include "ApiCommon.h"
#include "GenericPlatform/GenericPlatformHttp.h"

typedef TSharedPtr<FJsonValue> FJsonValuePtr;
typedef TSharedPtr<FJsonObject> FJsonObjectPtr;
typedef TArray<TSharedPtr<FJsonValue>> FJsonValuePtrArray;

FString FApiOpenSeaAssetMetadata::GetImageUrl() const {
	if (!AnimationOriginalUrl.IsEmpty()) {
		return AnimationOriginalUrl;
	}

	if (!AnimationUrl.IsEmpty()) {
		return AnimationUrl;
	}

	if (!ImageOriginalUrl.IsEmpty()) {
		return ImageOriginalUrl;
	}

	if (!ImageUrl.IsEmpty()) {
		return ImageUrl;
	}

	if (!ImagePreviewUrl.IsEmpty()) {
		return ImagePreviewUrl;
	}

	if (!ImageThumbnailUrl.IsEmpty()) {
		return ImageThumbnailUrl;
	}

	return {};
}

bool FApiOpenSeaAssetMetadata::FromJson(const TSharedPtr<FJsonObject> JsonObject) {
	bool bOk = IApiMetadata::FromJson(JsonObject);

	Id = GetJsonGuidField(JsonObject, Api::Fields::Id);
	AssetId = GetJsonStringField(JsonObject, "assetId");
	ContactAddress = GetJsonStringField(JsonObject, "contractAddress");
	ImageUrl = GetJsonStringField(JsonObject, "imageUrl");
	ImagePreviewUrl = GetJsonStringField(JsonObject, "imagePreviewUrl");
	ImageThumbnailUrl = GetJsonStringField(JsonObject, "imageThumbnailUrl");
	ImageOriginalUrl = GetJsonStringField(JsonObject, "imageOriginalUrl");
	AnimationUrl = GetJsonStringField(JsonObject, "animationUrl");
	AnimationOriginalUrl = GetJsonStringField(JsonObject, "animationOriginalUrl");
	AssetName = GetJsonStringField(JsonObject, "assetName");
	AssetDescription = GetJsonStringField(JsonObject, "assetDescription");
	ExternalLink = GetJsonStringField(JsonObject, "external_link");
	Permalink = GetJsonStringField(JsonObject, "permalink");
	Decimals = GetJsonIntField(JsonObject, "decimals");
	ObjectName = GetJsonStringField(JsonObject, "objectName");
	ObjectType = GetJsonStringField(JsonObject, "objectType");
	MimeType = GetJsonStringField(JsonObject, "mimeType");

	Owner = GetJsonStringField(JsonObject, "owner");

	return bOk;
}

void FApiOpenSeaAssetMetadata::SetMimeType(const FString& InMimeType) {
	MimeType = InMimeType;

	if (InMimeType.StartsWith(TEXT("image/"))) {
		MediaType = EApi2OpenSeaAssetType::Image;
	} else if (InMimeType.StartsWith(TEXT("video"))) {
		MediaType = EApi2OpenSeaAssetType::Video;
	} else if (InMimeType.StartsWith(TEXT("model/")) && InMimeType.Contains(TEXT("gltf"))) {
		MediaType = EApi2OpenSeaAssetType::Mesh;
	} else if (InMimeType.StartsWith(TEXT("audio/"))) {
		MediaType = EApi2OpenSeaAssetType::Audio;
	} else {
		MediaType = EApi2OpenSeaAssetType::Unknown;
	}
}
bool FApiOpenSeaAssetBatchMetadata::FromJson(const TSharedPtr<FJsonObject> JsonObject) {
	bool bOk = IApiMetadata::FromJson(JsonObject);

	const auto JsonEntities = GetJsonArrayField(JsonObject, Api::Fields::Entities);

	for (TArray<TSharedPtr<FJsonValue>>::TConstIterator It(JsonEntities); It; ++It) {
		TSharedPtr<FJsonObject> JsonEntity = (*It)->AsObject();
		if (JsonEntity) {
			// todo: do we need a sharedptr here as well as sharedptr array
			FApiOpenSeaAssetMetadata Entity;
			Entity.FromJson(JsonEntity);
			Assets.Add(Entity);
		} else {
			bOk &= false;
		}
	}

	return bOk;
}

bool FApiOpenSeaAssetBatchMetadata::FromJson(const TArray<TSharedPtr<FJsonValue>>& JsonArray) {
	bool bOk = true;
	for (auto& Json : JsonArray) {
		auto JsonObject{Json->AsObject()};
		if (JsonObject) {
			FApiOpenSeaAssetMetadata Entity;
			Entity.FromJson(JsonObject);
			Assets.Emplace(Entity);
		} else {
			bOk &= false;
		}
	}
	return bOk;
}
