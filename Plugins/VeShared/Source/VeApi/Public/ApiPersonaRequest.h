// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "ApiOkMetadata.h"
#include "ApiRequest.h"
#include "ApiPersonaMetadata.h"
#include "ApiUserMetadata.h"

class VEAPI_API FApiGetPersonaRequest final : public FApiRequest {
public:
	virtual bool Process() override;

	FGuid RequestEntityId = FGuid();
	FApiPersonaMetadata ResponseMetadata{};

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

class VEAPI_API FApiGetPersonaBatchRequest final : public FApiRequest {
public:
	virtual bool Process() override;

	FGuid RequestUserId = FGuid();
	IApiBatchQueryRequestMetadata RequestMetadata;
	FApiPersonaBatchMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

class VEAPI_API FApiCreatePersonaRequest final : public FApiRequest {
public:
	virtual bool Process() override;

	FApiCreatePersonaMetadata RequestMetadata;
	FApiPersonaMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

class VEAPI_API FApiDefaultPersonaRequest final : public FApiRequest {
public:
	virtual bool Process() override;

	FGuid PersonaId;
	FApiPersonaMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

class VEAPI_API FApiUpdatePersonaRequest final : public FApiRequest {
public:
	virtual bool Process() override;

	FApiUpdatePersonaMetadata RequestMetadata;
	FApiPersonaMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

class VEAPI_API FApiDeletePersonaRequest final : public FApiRequest {
public:
	virtual bool Process() override;

	FApiUpdatePersonaMetadata RequestMetadata;
	FApiPersonaMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};


/** Persona batch request delegate. */
typedef TDelegate<void(const FApiPersonaBatchMetadata& /*InMetadata*/, const bool /*bInSuccessful*/, const FString& /*Error*/)> FOnPersonaBatchRequestCompleted;;
/** Persona entity request delegate. */
typedef TDelegate<void(const FApiPersonaMetadata& /*InMetadata*/, const bool /*bInSuccessful*/, const FString& /*Error*/)> FOnPersonaRequestCompleted;;
