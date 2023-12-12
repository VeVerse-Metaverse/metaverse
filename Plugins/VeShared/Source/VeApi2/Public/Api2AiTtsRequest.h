// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "Api2Request.h"

namespace Api2AiTtsOutputFormats {
	const FString Mp3 = TEXT("mp3");
	const FString Ogg = TEXT("ogg_vorbis");
	const FString Pcm = TEXT("pcm");
}

namespace Api2AiTtsVoiceIds { namespace Female {
		const FString C = TEXT("en-US-Neural2-C");
		const FString E = TEXT("en-US-Neural2-E");
		const FString F = TEXT("en-US-Neural2-F");
		const FString G = TEXT("en-US-Neural2-G");
		const FString H = TEXT("en-US-Neural2-H");
	}

	namespace Male {
		const FString A = TEXT("en-US-Neural2-A");
		const FString D = TEXT("en-US-Neural2-D");
		const FString I = TEXT("en-US-Neural2-I");
		const FString J = TEXT("en-US-Neural2-J");
	}}

class VEAPI2_API FApi2AiTtsRequestMetadata final : public IApiMetadata {
public:
	FApi2AiTtsRequestMetadata();

	/**
	 * @brief Engine to use.
	 */
	FString Engine = TEXT("neural");

	/**
	 * @brief Language code to use.
	 */
	FString LanguageCode = TEXT("en-US");

	/**
	 * @brief Output format to use.
	 */
	FString OutputFormat = TEXT("mp3");

	/**
	 * @brief Sample rate to use.
	 */
	FString SampleRate = TEXT("24000");

	/**
	 * @brief Text to convert to speech.
	 */
	FString Text = {};

	/**
	 * @brief Text type to use.
	 */
	FString TextType = TEXT("text");

	/**
	 * @brief Voice ID to use.
	 */
	FString VoiceId = TEXT("en-US-Neural2-J");

	virtual TSharedPtr<FJsonObject> ToJson() override;
};

class VEAPI2_API FApi2AiTtsResponseMetadata final : public IApiMetadata {
public:
	FApi2AiTtsResponseMetadata();

	/**
	 * @brief URL to the generated audio file.
	 */
	FString Url = {};
};

class VEAPI2_API FApi2AiTtsRequest final : public FApi2Request {
public:
	virtual bool Process() override;

	/**
	 * @brief Request metadata.
	 */
	FApi2AiTtsRequestMetadata RequestMetadata;

	/**
	 * @brief Response metadata.
	 */
	FApi2AiTtsResponseMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

class VEAPI2_API FApi2AiStreamTtsRequest final : public FApi2Request {
public:
	virtual bool Process() override;

	/**
	 * @brief Request metadata.
	 */
	FApi2AiTtsRequestMetadata RequestMetadata;

	/**
	 * @brief Response metadata.
	 */
	FApi2AiTtsResponseMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

typedef TDelegate<void(const FApi2AiTtsResponseMetadata& InMetadata, EApi2ResponseCode InResponseCode, const FString& InError)> FOnAiTtsRequestCompleted2;
