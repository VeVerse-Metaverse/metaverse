// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "ApiOkMetadata.h"
#include "ApiRequest.h"
#include "ApiUserMetadata.h"

class VEAPI_API FApiUserBatchRequest final : public FApiRequest {
public:
	virtual bool Process() override;

	IApiBatchQueryRequestMetadata RequestMetadata;
	FApiUserBatchMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

class VEAPI_API FApiGetUserRequest final : public FApiRequest {
public:
	virtual bool Process() override;

	FGuid RequestEntityId = FGuid();
	FApiUserMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

class VEAPI_API FApiGetUserByEthAddressRequest final : public FApiRequest {
public:
	virtual bool Process() override;

	FString RequestEthAddress = FString();
	FApiUserMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

/** User login request. */
class VEAPI_API FApiUserLoginRequest final : public FApiRequest {
public:
	virtual bool Process() override;

	FApiUserLoginMetadata RequestMetadata;
	FApiUserMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

/** Used to request spaces in batches (with pagination). */

/** User register request. */
class VEAPI_API FApiRegisterUserRequest final : public FApiRequest {
public:
	virtual bool Process() override;

	FApiRegisterUserMetadata RequestMetadata;
	FApiUserMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

/** User update request. */
class VEAPI_API FApiUpdateUserRequest final : public FApiRequest {
public:
	virtual bool Process() override;

	FApiUpdateUserMetadata RequestMetadata;
	FApiUserMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

/** User update request. */
class VEAPI_API FApiUpdateUserPasswordRequest final : public FApiRequest {
public:
	virtual bool Process() override;

	FApiUpdateUserPasswordMetadata RequestMetadata;
	FApiUserMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

/** User reset password request. */
class VEAPI_API FApiResetUserPasswordRequest final : public FApiRequest {
public:
	virtual bool Process() override;

	FApiResetUserPasswordMetadata RequestMetadata;
	FApiOkMetadata ResponseMetadata;
	
protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

/** User get me. */
class VEAPI_API FApiGetUserMeRequest final : public FApiRequest {
public:
	virtual bool Process() override;

	FApiUserMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

/** User get avatars. */
class VEAPI_API FApiGetUserAvatarsRequest final : public FApiRequest {
public:
	virtual bool Process() override;

	FGuid RequestUserId = FGuid();
	IApiBatchRequestMetadata RequestMetadata;
	FApiFileBatchMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

/** User get files. */
class VEAPI_API FApiGetUserFilesRequest final : public FApiRequest {
public:
	virtual bool Process() override;

	IApiBatchRequestMetadata RequestMetadata;
	FApiFileBatchMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

/** User get avatar mesh. */
class VEAPI_API FApiGetUserAvatarMeshRequest final : public FApiRequest {
public:
	virtual bool Process() override;

	FGuid RequestUserId;
	FApiFileMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

class VEAPI_API FApiUserFollowRequest final : public FApiRequest {
public:
	virtual bool Process() override;

	FGuid RequestUserId = FGuid();

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

class VEAPI_API FApiUserUnfollowRequest final : public FApiRequest {
public:
	virtual bool Process() override;

	FGuid RequestUserId = FGuid();

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

class VEAPI_API FApiUserIsFollowingRequest final : public FApiRequest {
public:
	virtual bool Process() override;

	FGuid RequestFollowerId = FGuid();
	FGuid RequestLeaderId = FGuid();

	FApiOkMetadata ResponseMetadata;
protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

class VEAPI_API FApiUserGetInvitations final : public FApiRequest {
public:
	virtual bool Process() override;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

class VEAPI_API FApiUserGetFollowersRequest final : public FApiRequest {
public:
	virtual bool Process() override;

	FGuid RequestUserId = FGuid();
	bool RequestIncludeFriends = true;
	IApiBatchQueryRequestMetadata RequestMetadata;
	FApiUserBatchMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

class VEAPI_API FApiUserGetExperience final : public FApiRequest {
public:
	virtual bool Process() override;
	
	FGuid RequestUserId = FGuid();
	FApiUserMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

class VEAPI_API FApiUserGetFriendsRequest final : public FApiRequest {
public:
	virtual bool Process() override;
	
	FGuid RequestUserId = FGuid();
	IApiBatchQueryRequestMetadata RequestMetadata;
	FApiUserBatchMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

/** User batch request delegate. */
typedef TDelegate<void(const FApiUserBatchMetadata& /*InMetadata*/, const bool /*bInSuccessful*/, const FString& /*Error*/)> FOnUserBatchRequestCompleted;;
/** User entity request delegate. */
typedef TDelegate<void(const FApiUserMetadata& /*InMetadata*/, const bool /*bInSuccessful*/, const FString& /*Error*/)> FOnUserRequestCompleted;;
