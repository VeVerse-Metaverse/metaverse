// Author: Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#include "OpenSeaAssetMetadata.h"
#include "GenericPlatform/GenericPlatformHttp.h"
#include "VeShared.h"


bool FOpenSeaAssetTraitBase::FromJson(const TSharedPtr<FJsonObject> JsonObject) {
	TraitType = GetJsonStringField(JsonObject, "trait_type");
	Value = GetJsonStringField(JsonObject, "value", {});
	if (Value.IsEmpty()) {
		ValueNumber = GetJsonFloatField(JsonObject, "value");
	}
	DisplayType = GetJsonStringField(JsonObject, "display_type");

	return IApiMetadata::FromJson(JsonObject);
}

bool FOpenSeaAssetOwnerMetadata::FromJson(const TSharedPtr<FJsonObject> JsonObject) {
	Address = GetJsonStringField(JsonObject, "address");
	ProfileImageUrl = GetJsonStringField(JsonObject, "profile_img_url");
	return IApiMetadata::FromJson(JsonObject);
}

bool FOpenSeaAssetContract::FromJson(const TSharedPtr<FJsonObject> JsonObject) {
	Name = GetJsonStringField(JsonObject, "name");
	Address = GetJsonStringField(JsonObject, "address");
	AssetContractType = GetJsonStringField(JsonObject, "asset_contract_type");
	Symbol = GetJsonStringField(JsonObject, "symbol");
	ImageUrl = GetJsonStringField(JsonObject, "image_url");
	Description = GetJsonStringField(JsonObject, "description");
	ExternalLink = GetJsonStringField(JsonObject, "external_link");
	CreatedDate = GetJsonDateTimeField(JsonObject, "crated_date");

	return IApiMetadata::FromJson(JsonObject);
}

FString FOpenSeaAssetMetadata::GetImageUrl() const {
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

	if (!TokenMetadata.IsEmpty()) {
		if (TokenMetadata.EndsWith(".jpg") || TokenMetadata.EndsWith(".png")) {
			return TokenMetadata;
		}

		LogWarningF("token metadata of unknown format: %s", *TokenMetadata);
	} else {
		LogWarningF("token has no image: %s", *Name);
	}

	return {};
}

bool FOpenSeaAssetMetadata::FromJson(const TSharedPtr<FJsonObject> JsonObject) {
	bool bOk = IApiMetadata::FromJson(JsonObject);

	TokenId = GetJsonStringField(JsonObject, "token_id");
	NumSales = GetJsonIntField(JsonObject, "num_sales");
	BackgroundColor = GetJsonStringField(JsonObject, "background_color");
	ImageUrl = GetJsonStringField(JsonObject, "image_url");
	ImagePreviewUrl = GetJsonStringField(JsonObject, "image_preview_url");
	ImageThumbnailUrl = GetJsonStringField(JsonObject, "image_thumbnail_url");
	ImageOriginalUrl = GetJsonStringField(JsonObject, "image_original_url");
	AnimationUrl = GetJsonStringField(JsonObject, "animation_url");
	AnimationOriginalUrl = GetJsonStringField(JsonObject, "animation_original_url");
	Name = GetJsonStringField(JsonObject, "name");
	Description = GetJsonStringField(JsonObject, "description");
	ExternalLink = GetJsonStringField(JsonObject, "external_link");
	Permalink = GetJsonStringField(JsonObject, "permalink");
	Decimals = GetJsonIntField(JsonObject, "decimals");
	TokenMetadata = GetJsonStringField(JsonObject, "token_metadata");
	LastSale = GetJsonStringField(JsonObject, "last_sale");

	Owner.FromJson(GetJsonObjectField(JsonObject, Api::Fields::Owner));
	AssetContract.FromJson(GetJsonObjectField(JsonObject, "asset_contract"));

	const auto JsonEntities{GetJsonArrayField(JsonObject, "traits")};
	for (const auto& JsonEntity : JsonEntities) {
		if (const TSharedPtr<FJsonObject> EntityJsonObject = JsonEntity->AsObject()) {
			TSharedPtr<FOpenSeaAssetTraitBase> Trait = MakeShareable(new FOpenSeaAssetTraitBase);
			Trait->FromJson(EntityJsonObject);
			Traits.Emplace(Trait);
		} else {
			bOk &= false;
		}
	}
	LastSale = GetJsonStringField(JsonObject, "last_sale");

	return bOk;
}

void FOpenSeaAssetMetadata::SetMimeType(const FString& InMimeType) {
	MimeType = InMimeType;

	if (InMimeType.StartsWith(TEXT("image/"))) {
		MediaType = EOpenSeaAssetType::Image;
	} else if (InMimeType.StartsWith(TEXT("video"))) {
		MediaType = EOpenSeaAssetType::Video;
	} else if (InMimeType.StartsWith(TEXT("model/")) && InMimeType.Contains(TEXT("gltf"))) {
		MediaType = EOpenSeaAssetType::Mesh;
	} else if (InMimeType.StartsWith(TEXT("audio/"))) {
		MediaType = EOpenSeaAssetType::Audio;
	} else {
		MediaType = EOpenSeaAssetType::Unknown;
	}
}

bool FOpenSeaAssetBatchMetadata::FromJson(const TSharedPtr<FJsonObject> JsonObject) {
	bool bOk = IApiMetadata::FromJson(JsonObject);

	const auto JsonEntities = GetJsonArrayField(JsonObject, Api::Fields::Assets);

	for (TArray<TSharedPtr<FJsonValue>>::TConstIterator It(JsonEntities); It; ++It) {
		TSharedPtr<FJsonObject> JsonEntity = (*It)->AsObject();
		if (JsonEntity) {
			// todo: do we need a sharedptr here as well as sharedptr array
			FOpenSeaAssetMetadata Entity;
			Entity.FromJson(JsonEntity);
			Assets.Add(Entity);
		} else {
			bOk &= false;
		}
	}

	return bOk;
}

bool FOpenSeaAssetBatchMetadata::FromJson(const TArray<TSharedPtr<FJsonValue>>& JsonArray) {
	bool bOk = true;
	for (auto& Json : JsonArray) {
		auto JsonObject{Json->AsObject()};
		if (JsonObject) {
			FOpenSeaAssetMetadata Entity;
			Entity.FromJson(JsonObject);
			Assets.Emplace(Entity);
		} else {
			bOk &= false;
		}
	}
	return bOk;
}

FString FOpenSeaBatchRequestMetadata::ToUrlParams() {
	const FString Base = IApiBatchRequestMetadata::ToUrlParams();

	FString Result = Base + TEXT("&format=json");

	return Result;
}

FString FOpenSeaAssetBatchRequestMetadata::ToUrlParams() {
	const FString Base = FOpenSeaBatchRequestMetadata::ToUrlParams();

	FString Result = Base;

	if (!Owner.IsEmpty()) {
		Result += FString::Printf(TEXT("&%s=%s"), Api::Fields::Owner, *FGenericPlatformHttp::UrlEncode(Owner));
	}

	if (!TokenIds.IsEmpty()) {
		Result += FString::Printf(TEXT("&%s=%s"), *FString{"token_ids"}, *FGenericPlatformHttp::UrlEncode(TokenIds));
	}

	if (!AssetContractAddress.IsEmpty()) {
		Result += FString::Printf(TEXT("&%s=%s"), *FString{"asset_contract_address"}, *FGenericPlatformHttp::UrlEncode(AssetContractAddress));
	}

	if (!AssetContractAddresses.IsEmpty()) {
		Result += FString::Printf(TEXT("&%s=%s"), *FString{"asset_contract_addresses"}, *FGenericPlatformHttp::UrlEncode(AssetContractAddresses));
	}

	if (!OrderBy.IsEmpty()) {
		Result += FString::Printf(TEXT("&%s=%s"), *FString{"order_by"}, *FGenericPlatformHttp::UrlEncode(OrderBy));
	}

	if (!OrderDirection.IsEmpty()) {
		Result += FString::Printf(TEXT("&%s=%s"), *FString{"order_direction"}, *FGenericPlatformHttp::UrlEncode(OrderDirection));
	}

	if (!Collection.IsEmpty()) {
		Result += FString::Printf(TEXT("&%s=%s"), *FString{"collection"}, *FGenericPlatformHttp::UrlEncode(Collection));
	}

	return Result;
}
