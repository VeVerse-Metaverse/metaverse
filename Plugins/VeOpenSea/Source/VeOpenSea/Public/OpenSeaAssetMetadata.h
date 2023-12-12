// Author: Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once
#include "ApiEntityMetadata.h"

namespace ETraitDisplayType {
	const auto Null{"null"};
	const auto Number{"number"};
	const auto BoostPercentage{"boost_percentage"};
	const auto BoostNumber{"boost_number"};
	const auto Date{"date"};
}

namespace EContractType {
	const auto Fungible{"fungible"};
	const auto NonFungible{"non-fungible"};
	const auto SemiFungible{"semi-fungible"};
};

/** Base class for all future traits needed */
class VEOPENSEA_API FOpenSeaAssetTraitBase final : public IApiMetadata {
public:
	/** The name of the trait (for example color) */
	FString TraitType{};
	/** The value of this trait (can be a string or number) */
	FString Value{};
	/** How this trait will be displayed (options are number, boost_percentage, boost_number, and date) */
	FString DisplayType{ETraitDisplayType::Null};

	double ValueNumber{0};

	virtual bool FromJson(const TSharedPtr<FJsonObject> JsonObject) override;
};

/** Base class for all future traits needed */
class VEOPENSEA_API FOpenSeaAssetOwnerMetadata final : public IApiMetadata {
public:
	/** Hex address of the asset contract */
	FString Address{};

	FString ProfileImageUrl{};

	virtual bool FromJson(const TSharedPtr<FJsonObject> JsonObject) override;
};

/** Data contract */
class VEOPENSEA_API FOpenSeaAssetContract final : public IApiMetadata {
public:
	/** Name of the asset contract */
	FString Name{};
	/** Hex address of the asset contract */
	FString Address{};
	FString AssetContractType{EContractType::SemiFungible};
	/** Symbol, such as CKITTY */
	FString Symbol{};
	/** Image associated with the asset contract */
	FString ImageUrl{};
	/** Description of the asset contract */
	FString Description{};
	/** Link to the original website for this contract */
	FString ExternalLink{};
	/** Contract creation date */
	FDateTime CreatedDate{0};

	virtual bool FromJson(const TSharedPtr<FJsonObject> JsonObject) override;
};

UENUM(BlueprintType)
enum class EOpenSeaAssetType : uint8 {
	Unknown,
	Image,
	Video,
	Audio,
	Mesh
};

/** Single object metadata. */
class VEOPENSEA_API FOpenSeaAssetMetadata final : public IApiMetadata {
public:
	/** Id of the OpenSea asset */
	uint32 Id{0};
	/** The token ID of the NFT */
	FString TokenId{};
	/** Id of the OpenSea asset */
	uint32 NumSales{0};
	/** Not used */
	FString BackgroundColor{};
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
	FString Name{};
	/** Description of the item */
	FString Description{};
	/** External link to the original website for the item */
	FString ExternalLink{};
	/** Image OpenSea link */
	FString Permalink{};
	/** Number of decimals for the token */
	uint32 Decimals{0};
	/** Token metadata, image URL can be stored here as well */
	FString TokenMetadata{};
	/** Ethereum blockchain public address of the owner. */
	FOpenSeaAssetOwnerMetadata Owner{};
	/** Data on the contract itself (see asset contract section) */
	FOpenSeaAssetContract AssetContract{};
	/** A list of traits associated with the item (see traits section) */
	TArray<TSharedPtr<FOpenSeaAssetTraitBase>> Traits;
	/** When this item was last sold (null if there was no last sale) */
	FString LastSale{};

	/** Determines media mime type. */
	FString MimeType;
	/** Determines media media type. */
	EOpenSeaAssetType MediaType = EOpenSeaAssetType::Unknown;

	FString GetImageUrl() const;

	virtual bool FromJson(const TSharedPtr<FJsonObject> JsonObject) override;
	void SetMimeType(const FString& InMimeType);
};

/** Response metadata */
class VEOPENSEA_API FOpenSeaAssetBatchMetadata final : public IApiMetadata {
public:
	TArray<FOpenSeaAssetMetadata> Assets = {};

	virtual bool FromJson(const TSharedPtr<FJsonObject> JsonObject) override;
	bool FromJson(const TArray<TSharedPtr<FJsonValue>>& JsonArray);
};

class VEOPENSEA_API FOpenSeaBatchRequestMetadata : public IApiBatchQueryRequestMetadata {
public:
	virtual FString ToUrlParams() override;
};

/** Request metadata */
class VEOPENSEA_API FOpenSeaAssetBatchRequestMetadata final : public FOpenSeaBatchRequestMetadata {
public:
	/** The address of the owner of the assets. */
	FString Owner;

	/** An array of token IDs to search for (e.g. ?token_ids=1&token_ids=209). Will return a list of assets with token_id matching any of the IDs in this array. */
	FString TokenIds;

	/** The NFT contract address for the assets. */
	FString AssetContractAddress;

	/**
	 * An array of contract addresses to search for (e.g. ?asset_contract_addresses=0x1...&asset_contract_addresses=0x2...).
	 * Will return a list of assets with contracts matching any of the addresses in this array.
	 * If "token_ids" is also specified, then it will only return assets that match each (address, token_id) pairing, respecting order.
	 */
	FString AssetContractAddresses;

	/**
	 * How to order the assets returned. By default, the API returns the fastest ordering.
	 * Options you can set are sale_date (the last sale's transaction's timestamp), sale_count (number of sales), and sale_price (the last sale's total_price).
	 */
	FString OrderBy;

	/** Can be asc for ascending or desc for descending. */
	FString OrderDirection;

	/**
	 * Limit responses to members of a collection. Case sensitive and must match the collection slug exactly. Will return all assets from all contracts in a collection.
	 * For more information on collections, see our collections documentation.
	 */
	FString Collection;

	virtual FString ToUrlParams() override;
};
