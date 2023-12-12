// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "ApiFileUploadRequest.h"
#include "ApiUserRequest.h"
#include "VeShared.h"

class VEAPI_API FApiUserSubsystem final : public IModuleSubsystem {
public:
	const static FName Name;

	virtual void Initialize() override;
	virtual void Shutdown() override;

	/**
	 * Requests the user metadata by the id.
	 * @param InId User ID.
	 * @param InCallback Complete callback delegate. 
	 */
	bool GetUser(const FGuid& InId, TSharedPtr<FOnUserRequestCompleted> InCallback);

	/**
	 * Requests the user metadata by the id.
	 * @param InId User ID.
	 * @param InCallback Complete callback delegate. 
	 */
	bool GetUserByEthAddress(const FString& InAddress, TSharedPtr<FOnUserRequestCompleted> InCallback);

	/**
	 * Requests the user metadata batch.
	 * @param InMetadata Request metadata, offset, limit, query.
	 * @param InCallback Complete callback delegate.
	 */
	bool GetBatch(const IApiBatchQueryRequestMetadata& InMetadata, TSharedPtr<FOnUserBatchRequestCompleted> InCallback);

	/**
	 * Requests the user avatar metadata batch.
	 * @param InMetadata Request metadata, offset, limit, query.
	 * @param InCallback Complete callback delegate.
	 */
	bool GetAvatarBatch(const FGuid& InUserId, const IApiBatchRequestMetadata& InMetadata, TSharedPtr<FOnFileBatchRequestCompleted> InCallback);

	
	/**
	 * Requests the user file metadata batch.
	 * @param InMetadata Request metadata, offset, limit, query.
	 * @param InCallback Complete callback delegate.
	 */
	// bool GetFileBatch(const IApiBatchRequestMetadata& InMetadata, TSharedPtr<FOnFileBatchRequestCompleted> InCallback);

	/**
	 * Requests the user file metadata batch.
	 * @param InUserId Request user id.
	 * @param InCallback Complete callback delegate.
	 */
	// bool GetAvatarMesh(const FGuid& InUserId, TSharedPtr<FOnFileRequestCompleted> InCallback);

	/**
	 * Requests the current user metadata.
	 * @param InCallback Complete callback delegate.
	 */
	// bool GetMe(TSharedPtr<FOnUserRequestCompleted> InCallback);

	/**
	 * Requests following the user.
	 * @param InId Target user ID.
	 * @param InCallback Complete callback delegate.
	 */
	bool Follow(const FGuid& InId, TSharedPtr<FOnGenericRequestCompleted> InCallback);

	/**
	 * Requests unfollowing the user.
	 * @param InId Target user ID.
	 * @param InCallback Complete callback delegate.
	 */
	bool Unfollow(const FGuid& InId, TSharedPtr<FOnGenericRequestCompleted> InCallback);

	/**
	 * Requests unfollowing the user.
	 * @param InFollowerId Source user ID.
	 * @param InLeaderId Target user ID.
	 * @param InCallback Complete callback delegate.
	 */
	bool IsFollowing(const FGuid& InFollowerId, const FGuid& InLeaderId, TSharedPtr<FOnOkRequestCompleted> InCallback);

	/** Requests followers the user
	 * @param InId Target user ID.
	 * @param InMetadata Request metadata, offset, limit.
	 * @param InCallback Complete callback delegate.
	 */
	bool GetFollowers(const FGuid& InId, const IApiFollowersRequestMetadata& InMetadata, TSharedPtr<FOnUserBatchRequestCompleted> InCallback);

	/** Requests experience the user
	 * @param InId Target user ID.
	 * @param InCallback Complete callback delegate.
	 */
	bool GetExperience(const FGuid& InId, TSharedPtr<FOnUserRequestCompleted> InCallback);

	/** Requests friends the user
	 * @param InId Target user ID.
	 * @param InMetadata Request metadata, offset, limit.
	 * @param InCallback Complete callback delegate.
	 */
	bool GetFriends(const FGuid& InId, const IApiBatchQueryRequestMetadata& InMetadata, TSharedPtr<FOnUserBatchRequestCompleted> InCallback);


	/** Request password reset
	 * @param Email Email for password reset.
	 * @param InCallback Complete callback delegate.
	 */
	bool PasswordReset(const FString& Email, TSharedPtr<FOnOkRequestCompleted> InCallback);
};

typedef TSharedPtr<FApiUserSubsystem> FApiUserSubsystemPtr;
