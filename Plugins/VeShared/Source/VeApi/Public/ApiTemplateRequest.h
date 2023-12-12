// Authors: Egor A. Pristavka, Khusan T. Yadgarov. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


#pragma once
#include "ApiRequest.h"
#include "ApiTemplateMetadata.h"

/** Used to request spaces in batches (with pagination). */
class VEAPI_API FApiTemplateBatchRequest final : public FApiRequest {
public:
	virtual bool Process() override;

	IApiBatchQueryRequestMetadata RequestMetadata;
	FApiTemplateBatchMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

class VEAPI_API FApiGetTemplateRequest final : public FApiRequest {
public:
	virtual bool Process() override;

	FGuid RequestEntityId = FGuid();
	FApiTemplateMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

/** Used to create spaces. */
class VEAPI_API FApiCreateTemplateRequest final : public FApiRequest {
public:
	virtual bool Process() override;

	FApiUpdateTemplateMetadata RequestMetadata;
	FApiTemplateMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

/** Used to update spaces. */
class VEAPI_API FApiUpdateTemplateRequest final : public FApiRequest {
public:
	virtual bool Process() override;

	FApiUpdateTemplateMetadata RequestMetadata;
	FApiTemplateMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

typedef TDelegate<void(const FApiTemplateMetadata& /*InMetadata*/, const bool /*bInSuccessful*/, const FString& /*InError*/)> FOnTemplateRequestCompleted;
typedef TDelegate<void(const FApiTemplateBatchMetadata& /*InMetadata*/, const bool /*bInSuccessful*/, const FString& /*InError*/)> FOnTemplateBatchRequestCompleted;