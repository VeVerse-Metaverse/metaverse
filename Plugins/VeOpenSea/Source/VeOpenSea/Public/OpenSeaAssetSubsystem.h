// Author: Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once
#include "OpenSeaAssetRequest.h"
#include "VeShared.h"

class IApiBatchQueryRequestMetadata;

class VEOPENSEA_API FOpenSeaAssetSubsystem final : public IModuleSubsystem {
public:
	const static FName Name;

	virtual void Initialize() override;
	virtual void Shutdown() override;

	/**
	 * Requests the asset metadata batch.
	 * @param InMetadata Request metadata, offset, limit, query.
	 * @param InCallback Complete callback delegate.
	 */
	bool GetBatch(const FOpenSeaAssetBatchRequestMetadata& InMetadata, TSharedPtr<FOnOpenSeaAssetBatchRequestCompleted> InCallback);

	/**
	 * Requests the asset metadata by the id.
	 * @param InId Object ID.
	 * @param InCallback Complete callback delegate.
	 */
	void GetAsset(const FString& ContractAddress, const FString& TokenId, TSharedRef<FOnOpenSeaAssetRequestCompleted> InCallback);

	/**
	 * Requests all the assets for the given owner.
	 * @param OwnerAddress Address of the owner.
	 * @param Callback Complete callback delegate.
	 */
	bool GetOwnerAssets(const FString& OwnerAddress, TSharedPtr<FOnOpenSeaAssetBatchRequestCompleted> Callback);
};

typedef TSharedPtr<FOpenSeaAssetSubsystem> FApiAssetSubsystemPtr;
