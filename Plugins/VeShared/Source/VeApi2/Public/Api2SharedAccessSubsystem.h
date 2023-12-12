// Authors: Khusan Yadgarov, Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "Api2Request.h"
#include "Api2SharedAccessMetadata.h"
#include "VeShared.h"


typedef TDelegate<void(const FApi2SharedAccessMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError)> FOnSharedAccessRequestCompleted2;
typedef TDelegate<void(const FApi2SharedAccessBatchMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError)> FOnSharedAccessBatchRequestCompleted2;

class VEAPI2_API FApi2SharedAccessBatchRequestMetadata final : public IApiBatchQueryRequestMetadata {
public:
	FApi2SharedAccessBatchRequestMetadata();

	FGuid EntityId = FGuid{};
	
	virtual TSharedPtr<FJsonObject> ToJson() override;
};

/** Used to update mods. */
class VEAPI2_API FApi2SharedAccessRequest final : public FApi2Request {
public:
	virtual bool Process() override;

	FApi2SharedAccessMetadata RequestMetadata;
	FApi2SharedAccessMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

/** Used to update mods. */
class VEAPI2_API FApi2SharedPublicRequest final : public FApi2Request {
public:
	virtual bool Process() override;

	FApi2SharedAccessMetadata RequestMetadata;
	FApi2SharedAccessMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

class VEAPI2_API FApi2SharedAccessIndexRequest final : public TApi2BatchRequest<FApi2SharedAccessMetadata>  {
public:
	virtual bool Process() override;
	
	FApi2SharedAccessBatchRequestMetadata RequestMetadata;
	FApi2SharedAccessBatchMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

class VEAPI2_API FApi2SharedAccessSubsystem final : public IModuleSubsystem {

public:
	static FName Name;

	virtual void Initialize() override;
	virtual void Shutdown() override;
	
	/**
	 * Requests the SharedAccess update.
	 * @param InMetadata Request metadata.
	 * @param InCallback Complete callback delegate.
	 */
	void UpdateSharedAccess(const FApi2SharedAccessMetadata& InMetadata, TSharedRef<FOnSharedAccessRequestCompleted2> InCallback) const;
	
	void UpdatePublicAccess(const FApi2SharedAccessMetadata& InMetadata, TSharedRef<FOnSharedAccessRequestCompleted2> InCallback) const;
	
	void Index(const FApi2SharedAccessBatchRequestMetadata& InMetadata, TSharedRef<FOnSharedAccessBatchRequestCompleted2> InCallback) const;
	
};
