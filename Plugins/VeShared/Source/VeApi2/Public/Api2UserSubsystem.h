// Authors: Khusan Yadgarov, Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "Api2Request.h"
#include "ApiEntityMetadata.h"
#include "ApiOkMetadata.h"
#include "ApiUserMetadata.h"
#include "VeShared.h"

typedef TDelegate<void(const FApiUserMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError)> FOnUserRequestCompleted2;
typedef TDelegate<void(const FApiUserBatchMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError)> FOnUserBatchRequestCompleted2;

class VEAPI2_API FApi2GetMeRequest final : public FApi2Request {
public:
	virtual bool Process() override;

	FApiUserMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

class VEAPI2_API FApi2GetUserByEthAddressRequest final : public FApi2Request {
public:
	virtual bool Process() override;

	FString RequestEthAddress = FString();
	FApiUserMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

class VEAPI2_API FApi2GetUserRequest final : public FApi2Request {
public:
	virtual bool Process() override;

	FGuid RequestEntityId = FGuid();
	FApiUserMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

class VEAPI2_API FApi2UserFollowRequest final : public FApi2Request {
public:
	virtual bool Process() override;

	FGuid RequestUserId = FGuid();

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

class VEAPI2_API FApi2UserUnfollowRequest final : public FApi2Request {
public:
	virtual bool Process() override;

	FGuid RequestUserId = FGuid();

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

class VEAPI2_API FApi2UserIsFollowingRequest final : public FApi2Request {
public:
	virtual bool Process() override;

	FGuid RequestFollowerId = FGuid();
	FGuid RequestLeaderId = FGuid();

	FApiOkMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

class VEAPI2_API FApi2ResetUserPasswordRequest final : public FApi2Request {
public:
	virtual bool Process() override;

	FApiResetUserPasswordMetadata RequestMetadata;
	FApiOkMetadata ResponseMetadata;
	
protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

class VEAPI2_API FApi2ChangeUserNameRequest final : public FApi2Request {
public:
	virtual bool Process() override;

	FString UserName;
	FApiOkMetadata ResponseMetadata;
	
protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

class VEAPI2_API FApi2UserGetFriendsRequest final : public FApi2Request {
public:
	virtual bool Process() override;
	
	FGuid RequestUserId = FGuid();
	IApiBatchQueryRequestMetadata RequestMetadata;
	FApiUserBatchMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

class VEAPI2_API FApi2UserGetFollowersRequest final : public FApi2Request {
public:
	virtual bool Process() override;

	FGuid RequestUserId = FGuid();
	bool RequestIncludeFriends = true;
	IApiBatchQueryRequestMetadata RequestMetadata;
	FApiUserBatchMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

class VEAPI2_API FApi2UserGetExperience final : public FApi2Request {
public:
	virtual bool Process() override;
	
	FGuid RequestUserId = FGuid();
	FApiUserMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

class VEAPI2_API FApi2UserSubsystem final : public IModuleSubsystem {

public:
	static FName Name;

	virtual void Initialize() override;
	virtual void Shutdown() override;

	void GetMe(TSharedRef<FOnUserRequestCompleted2> InCallback) const;

	void GetUser(const FGuid& InId, TSharedPtr<FOnUserRequestCompleted2> InCallback) const;

	void GetUserByEthAddress(const FString& InAddress, TSharedPtr<FOnUserRequestCompleted2> InCallback) const;

	void Follow(const FGuid& InId, TSharedPtr<FOnGenericRequestCompleted2> InCallback) const;

	void Unfollow(const FGuid& InId, TSharedPtr<FOnGenericRequestCompleted2> InCallback) const;

	void IsFollowing(const FGuid& InFollowerId, const FGuid& InLeaderId, TSharedPtr<FOnOkRequestCompleted2> InCallback) const;

	/** Requests followers the user
	* @param InId Target user ID.
	* @param InMetadata Request metadata, offset, limit.
	* @param InCallback Complete callback delegate.
	*/
	void GetFollowers(const FGuid& InId, const IApiFollowersRequestMetadata& InMetadata, TSharedPtr<FOnUserBatchRequestCompleted2> InCallback);

	/** Requests experience the user
	 * @param InId Target user ID.
	 * @param InCallback Complete callback delegate.
	 */
	void GetExperience(const FGuid& InId, TSharedPtr<FOnGenericRequestCompleted2> InCallback);

	/** Requests friends the user
	 * @param InId Target user ID.
	 * @param InMetadata Request metadata, offset, limit.
	 * @param InCallback Complete callback delegate.
	 */
	void GetFriends(const FGuid& InId, const IApiBatchQueryRequestMetadata& InMetadata, TSharedPtr<FOnUserBatchRequestCompleted2> InCallback);
	
	/** Request password reset
	 * @param Email Email for password reset.
	 * @param InCallback Complete callback delegate.
	 */
	void PasswordReset(const FString& Email, TSharedPtr<FOnOkRequestCompleted2> InCallback);

	/** Request password reset
	 * @param inUserName UserName for password reset.
	 * @param InCallback Complete callback delegate.
	 */
	void ChangeUserName(const FString& inUserName, TSharedPtr<FOnOkRequestCompleted2> InCallback);
};
