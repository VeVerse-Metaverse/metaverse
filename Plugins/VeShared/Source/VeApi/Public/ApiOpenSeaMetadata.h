// Authors: Egor A. Pristavka, Khusan T. Yadgarov. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


#pragma once

#include "ApiEntityMetadata.h"


UENUM(BlueprintType)
enum class EApi2OpenSeaAssetType : uint8 {
	Unknown,
	Image,
	Video,
	Audio,
	Mesh
};

class VEAPI_API FApiOpenSeaAssetMetadata final : public IApiMetadata {
public:
	/** Id of the OpenSea asset */
	FGuid Id;
	/** The token ID of the NFT */
	FString AssetId{};
	FString ContactAddress{};
	/** An image for the item. Note that this is the cached URL we store on our end. The original image url is image_original_url */
	FString ImageUrl{};
	/** Image preview */
	FString ImagePreviewUrl{};
	/** Image thumbnail */
	FString ImageThumbnailUrl{};
	/** Image original */
	FString ImageOriginalUrl{};
	/** Animation */
	FString AnimationUrl{};
	/** Animation original */
	FString AnimationOriginalUrl{};
	/** Name of the item */
	FString AssetName{};
	/** Description of the item */
	FString AssetDescription{};
	/** External link to the original website for the item */
	FString ExternalLink{};
	/** Image OpenSea link */
	FString Permalink{};
	/** Number of decimals for the token */
	uint32 Decimals{0};
	FString ObjectName{};
	FString ObjectType{};
	FString Owner{};
	/** Determines media mime type. */
	FString MimeType;
	
	FString GetImageUrl() const;
	
	/** Determines media media type. */
	EApi2OpenSeaAssetType MediaType = EApi2OpenSeaAssetType::Unknown;

	virtual bool FromJson(const TSharedPtr<FJsonObject> JsonObject) override;
	void SetMimeType(const FString& InMimeType);
};

/** Response metadata */
class VEAPI_API FApiOpenSeaAssetBatchMetadata final : public TApiBatchMetadata<FApiOpenSeaAssetMetadata> {
public:
	TArray<FApiOpenSeaAssetMetadata> Assets = {};

	virtual bool FromJson(const TSharedPtr<FJsonObject> JsonObject) override;
	virtual bool FromJson(const TArray<TSharedPtr<FJsonValue>>& JsonArray) override;
};
