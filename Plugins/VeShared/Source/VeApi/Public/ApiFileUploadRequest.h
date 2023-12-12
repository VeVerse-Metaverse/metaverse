// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once
#include "ApiFileMetadata.h"
#include "ApiRequest.h"

/** Used to upload file.*/
class VEAPI_API FApiFileUploadRequest final : public FApiRequest {
public:
	virtual bool Process() override;

	FApiUploadFileMetadata RequestMetadata;
	FApiFileMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

/** Used to upload file.*/
class VEAPI_API FApiMemoryFileUploadRequest final : public FApiRequest {
public:
	virtual bool Process() override;

	FApiUploadMemoryFileMetadata RequestMetadata;
	FApiFileMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

class VEAPI_API FApiEntityFileLinkRequest final : public FApiRequest {
public:
	virtual bool Process() override;

	FApiEntityFileLinkMetadata RequestMetadata;
	FApiFileMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

/** File batch request delegate. */
typedef TDelegate<void(const FApiFileBatchMetadata& /*InMetadata*/, const bool /*bInSuccessful*/, const FString& /*InError*/)> FOnFileBatchRequestCompleted;
/** File request delegate. */
typedef TDelegate<void(const FApiFileMetadata& /*InMetadata*/, const bool /*bInSuccessful*/, const FString& /*InError*/)> FOnFileRequestCompleted;
