// Authors: Khusan Yadgarov, Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once
#include "Api2Request.h"
#include "ApiEntityMetadata.h"
#include "ApiFileMetadata.h"
#include "ApiRequest.h"
#include "VeShared.h"

class VEAPI2_API FApi2SpeechRecognitionRequestMetadata final : public IApiMetadata {
public:
	explicit FApi2SpeechRecognitionRequestMetadata();

	/** Path of the source file. */
	FString FilePath;

	virtual TSharedPtr<FJsonObject> ToJson() override;
};

class VEAPI2_API FApi2SpeechRecognitionResponseMetadata final : public IApiMetadata {
public:
	FApi2SpeechRecognitionResponseMetadata();

	FString Result;
};

class VEAPI2_API FApi2SpeechRecognitionRequest final : public FApi2Request {
public:
	virtual bool Process() override;

	FApi2SpeechRecognitionRequestMetadata RequestMetadata;
	FApi2SpeechRecognitionResponseMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

typedef TDelegate<void(const FApi2SpeechRecognitionResponseMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError)> FOnSpeechRecognitionRequestCompleted2;

class VEAPI2_API FApi2SpeechRecognitionSubsystem final : public IModuleSubsystem {

public:
	static FName Name;

	virtual void Initialize() override;
	virtual void Shutdown() override;

	void Recognize(const FApi2SpeechRecognitionRequestMetadata& InMetadata, const TSharedRef<FOnSpeechRecognitionRequestCompleted2> InCallback) const;

	DECLARE_EVENT_ThreeParams(FApi2SpeechRecognitionSubsystem, FOnRecognitionComplete, const FString& /* Input */, const bool /* bSuccessful */, const FString& /* Error */);
};
