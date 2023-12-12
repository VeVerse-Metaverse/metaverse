// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "Api2Request.h"

enum class EApi2AiRole : uint8 {
	System,
	User,
	Assistant
};

class VEAPI2_API FApi2AiMessage final : public IApiMetadata {
public:
	/**
	 * @brief Role of the message.
	 */
	EApi2AiRole Role;

	/**
	 * @brief Content of the message.
	 */
	FString Content;

	virtual bool FromJson(const TSharedPtr<FJsonObject> JsonObject) override;
};

class VEAPI2_API FApi2AiActionMetadata final : public IApiMetadata {
public:
};

class VEAPI2_API FApi2AiActionRequestMetadata final : public IApiMetadata {
public:
};

class VEAPI2_API FApi2AiActionRequest : public FApi2Request {
public:
	virtual bool Process() override;

	FApi2AiActionRequestMetadata RequestMetadata;
	FApi2AiActionMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

typedef TDelegate<void(const FApi2AiActionMetadata& InMetadata, EApi2ResponseCode InResponseCode, const FString& InError)> FOnAiActionRequestCompleted2;
