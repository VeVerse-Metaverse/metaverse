// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "ApiEntityMetadata.h"
#include "HttpModule.h"
#include "Dom/JsonValue.h"
#include "Dom/JsonObject.h"
#include "HttpRequest.h"


struct FBlockchainTokenMetadata final : IApiMetadata {
	FString Name;
	FString Description;
	FString Image;

	virtual bool FromJson(const TSharedPtr<FJsonObject> InJsonObject) override;
};

/** Generic request delegate. */
typedef TDelegate<void(const bool /*bInSuccessful*/, const FString& /*Error*/)> FOnGenericRequestCompleted;
/** Owner of request delegate. */
typedef TDelegate<void(const FString& /*Owner*/, const bool /*bInSuccessful*/, const FString& /*Error*/)> FOnOwnerOfRequestCompleted;
/** Token URI request delegate. */
typedef TDelegate<void(const FBlockchainTokenMetadata& /*Metadata*/, const bool /*bInSuccessful*/, const FString& /*Error*/)> FOnTokenUriRequestCompleted;
/** Yes-no request delegate. */
typedef TDelegate<void(const bool /*bInOk*/, const bool /*bInSuccessful*/, const FString& /*Error*/)> FOnOkRequestCompleted;

class VEBLOCKCHAIN_API FBlockchainRequest : public FHttpRequest {
public:
	virtual bool Process() override;

protected:
	virtual bool CheckResponse(const FHttpResponsePtr Response) override;
	bool GetResponseJsonObjectPayload(const FHttpResponsePtr Response, TSharedPtr<FJsonObject>& OutJsonPayload) const;
	bool GetResponseJsonArrayPayload(const FHttpResponsePtr Response, TArray<TSharedPtr<FJsonValue>>& OutJsonPayload) const;
	bool SerializeJsonPayload(const TSharedPtr<FJsonObject>& JsonObject, TArray<uint8>& OutBytes) const;

};

class VEBLOCKCHAIN_API FBlockchainOwnerOfRequest : public FBlockchainRequest {
public:
	/** Token id */
	uint32 RequestTokenId = 0;

	/** Owner address */
	FString ResponseOwnerAddress = {};

	/** Error if was unable to get owner address */
	FString Error = {};

	virtual bool Process() override;
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

class VEBLOCKCHAIN_API FBlockchainTokenUriRequest : public FBlockchainRequest {
public:
	/** Token id */
	uint32 RequestTokenId = 0;

	/** Token metadata */
	FBlockchainTokenMetadata ResponseMetadata = {};

	/** Error if was unable to get metadata */
	FString Error = {};

	virtual bool Process() override;
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};
