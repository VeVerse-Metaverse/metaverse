// Author: Khusan.Yadgarov, Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


#pragma once
#include "Api2Request.h"
#include "ApiPersonaMetadata.h"
#include "ApiEntityMetadata.h"
#include "VeShared.h"

/** Persona batch request delegate. */
typedef TDelegate<void(const FApiPersonaBatchMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError)> FOnPersonaBatchRequestCompleted2;
typedef TDelegate<void(const FApiPersonaMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError)> FOnPersonaRequestCompleted2;

/** Persona request delegate. */
//typedef TDelegate<void(const FApiPersonaMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError)> FOnPersonaRequestCompleted2;

class VEAPI2_API FApi2GetPersonaBatchRequest final : public FApi2Request {
public:
	virtual bool Process() override;

	FGuid RequestUserId = FGuid();
	IApiBatchQueryRequestMetadata RequestMetadata;
	FApiPersonaBatchMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

class VEAPI2_API FApi2GetPersonaRequest final : public FApi2Request {
	public:
	virtual bool Process() override;

	FGuid RequestEntityId = FGuid();
	FApiPersonaMetadata ResponseMetadata{};

	protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

class VEAPI2_API FApi2CreatePersonaRequest final : public FApi2Request {
public:
	virtual bool Process() override;

	FApiCreatePersonaMetadata RequestMetadata;
	FApiPersonaMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};


class VEAPI2_API FApi2DefaultPersonaRequest final : public FApi2Request {
public:
	virtual bool Process() override;

	FGuid PersonaId;
	FApiPersonaMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

class VEAPI2_API FApi2UpdatePersonaRequest final : public FApi2Request {
public:
	virtual bool Process() override;

	FApiUpdatePersonaMetadata RequestMetadata;
	FApiPersonaMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

class VEAPI2_API FApi2DeletePersonaRequest final : public FApi2Request {
public:
	virtual bool Process() override;

	FApiUpdatePersonaMetadata RequestMetadata;
	FApiPersonaMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

class VEAPI2_API FApi2PersonaSubsystem final : public IModuleSubsystem {

public:
	static FName Name;

	virtual void Initialize() override;
	virtual void Shutdown() override;
	
	/**
	 * Requests the user metadata by the id.
	 * @param InUserId User ID.
	 * @param InMetadata Metadata.
	 * @param InCallback Complete callback delegate. 
	 */
	
	void GetPersonas(const FGuid& InUserId, const IApiBatchQueryRequestMetadata InMetadata, TSharedPtr<FOnPersonaBatchRequestCompleted2> InCallback);
	
	/**
	 * Requests the persona metadata by its id.
	 * @param InId Persona ID.
	 * @param InCallback Complete callback delegate.
	 */
	void GetPersona(const FGuid& InId, TSharedRef<FOnPersonaRequestCompleted2> InCallback);

	/**
 * Requests the user metadata by the id.
 * @param InMetadata Metadata.
 * @param InCallback Complete callback delegate. 
 */
	void PostPersonas(const FApiCreatePersonaMetadata InMetadata, TSharedPtr<FOnPersonaRequestCompleted2> InCallback);
	
	/**
	 * Requests the user metadata by the id.
	 * @param InPersonaId PersonaId.
	 * @param InCallback Complete callback delegate. 
	 */
	void PostDefaultPersonas(const FGuid& InPersonaId, TSharedPtr<FOnPersonaRequestCompleted2> InCallback);

	/**
	 * Requests the user metadata by the id.
	 * @param InMetadata Metadata.
	 * @param InCallback Complete callback delegate. 
	 */
	void PatchPersonas(const FApiUpdatePersonaMetadata InMetadata, TSharedPtr<FOnPersonaRequestCompleted2> InCallback); /**

	* Requests the user metadata by the id.
	 * @param InMetadata Metadata.
	 * @param InCallback Complete callback delegate. 
	 */
	void DeletePersonas(const FApiUpdatePersonaMetadata InMetadata, TSharedPtr<FOnPersonaRequestCompleted2> InCallback);

};
