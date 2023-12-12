// Copyright (c) 2022 LE7EL AS. All right reserved.

#pragma once

#include "VeShared.h"
#include "ApiPropertyRequest.h"


class VEAPI_API FApiPropertySubsystem : public IModuleSubsystem {
public:
	const static FName Name;

	virtual void Initialize() override;
	virtual void Shutdown() override;

	/**
	 * Requests the properties metadata batch.
	 * @param EntityId Entity ID.
	 * @param InMetadata Request metadata, offset, limit, query.
	 * @param InCallback Complete callback delegate.
	 */
	bool GetBatch(const FGuid& EntityId, const IApiBatchQueryRequestMetadata& InMetadata, TSharedPtr<FOnPropertyBatchRequestCompleted> InCallback);

	/**
	 * Requests the property create.
	 * @param EntityId Entity ID.
	 * @param InMetadata Request metadata.
	 * @param InCallback Complete callback delegate.
	 */
	bool Create(const FGuid& EntityId, const FApiUpdatePropertyMetadata& InMetadata, TSharedPtr<FOnPropertyRequestCompleted> InCallback);

	/**
	 * Requests the property update.
	 * @param EntityId Entity ID.
	 * @param InMetadata Request metadata.
	 * @param InCallback Complete callback delegate.
	 */
	bool Update(const FGuid& EntityId, const FApiUpdatePropertyMetadata& InMetadata, TSharedRef<FOnGenericRequestCompleted> InCallback);
	void Update(const FGuid& EntityId, const TArray<FApiUpdatePropertyMetadata>& InMetadatas, int32 Index, TSharedRef<FOnGenericRequestCompleted> InCallback);

	/**
	 * Requests the property delete.
	 * @param EntityId Entity ID.
	 * @param PropertyName Property Name to be deleted.
	 * @param InCallback Complete callback delegate.
	 */
	bool Delete(const FGuid& EntityId, const FString& PropertyName, TSharedPtr<FOnGenericRequestCompleted> InCallback);

};
