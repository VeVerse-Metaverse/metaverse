// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "Api2CogAiRequest.h"
#include "Api2AiSimpleFsmRequest.h"
#include "Api2AiTtsRequest.h"
#include "VeShared.h"

typedef TDelegate<void(const FApi2CogAiResponseMetadata& InMetadata, EApi2ResponseCode InResponseCode, const FString& InError)> FOnCogAiRequestCompleted;

/**
 * @brief Class for handling the AI subsystem requests.
 */
class VEAPI2_API FApi2AiSubsystem final : public IModuleSubsystem {
public:
	static FName Name;

	virtual void Initialize() override;
	virtual void Shutdown() override;

	/** @brief Request the simple finite state machine from the AI subsystem. */
	void GetSimpleFsm(const FApi2AiSimpleFsmRequestMetadata& InMetadata, TSharedRef<FOnAiSimpleFsmRequestCompleted2> InCallback) const;

	/** @brief Request the text to speech from the AI subsystem. */
	void GetTextToSpeech(const FApi2AiTtsRequestMetadata& InMetadata, TSharedRef<FOnAiTtsRequestCompleted2> InCallback) const;
	void StreamTextToSpeech(const FApi2AiTtsRequestMetadata& InMetadata, TSharedRef<FOnAiTtsRequestCompleted2> InCallback) const;

	/** @brief Request to get the string. */
	void SendString(const FString& Url, const FString& InString, TSharedRef<FOnStringRequestCompleted2> InCallback) const;

	/** @brief Is busy processing a Cognitive AI request. */
	bool bProcessingCogAiRequest = false;

	/** @brief Is busy processing a Cognitive AI user request. */
	bool bProcessingCogAiUserRequest = false;

	/** @brief Allow up to 10 requests to be queued. */
	int MaxQueueSize = 10;

	/** @brief Critical section for the queue. */
	FCriticalSection CogAiQueueCriticalSection;

	/** @brief Queue of requests to send to the Cognitive AI. */
	TQueue<TSharedPtr<FApi2CogAiRequest>> CogAiRequestQueue;

	/** @brief Current size of the Cognitive AI queue. */
	int CurrentCogAiQueueSize = 0;

	/** @brief Request to get the Cognitive AI response. */
	void SendCogAiRequest(const FApi2CogAiRequestMetadata& InMetadata, TSharedRef<FOnCogAiRequestCompleted> InCallback);

	/** @brief Critical section for the queue. */
	FCriticalSection CogAiUserQueueCriticalSection;

	/** @brief Queue of requests to send to the Cognitive AI using user-provided key and options. */
	TQueue<TSharedPtr<FApi2CogAiUserRequest>> CogAiUserRequestQueue;

	/** @brief Current size of the Cognitive AI user queue. */
	int CurrentCogAiUserQueueSize = 0;

	/** @brief Request to get the Cognitive AI response using user-provided key and options. */
	void SendCogAiUserRequest(const FApi2CogAiUserRequestMetadata& InMetadata, TSharedRef<FOnCogAiRequestCompleted> InCallback);

};
