// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "Api2Request.h"

/**
 * @brief Class for handling the Cognitive AI actions and queries.
 */
class VEAPI2_API FApi2CogAiMessage : public IApiMetadata {
public:
	/**
	 * @brief From whom the message is (system or npc).
	 */
	FString From = TEXT("");

	/**
	 * @brief Content of the message.
	 */
	TSharedPtr<FJsonObject> Content = nullptr;

	FString GetKey() const;

	virtual TSharedPtr<FJsonObject> ToJson() const override;
};

#pragma region AI System Query Messages

#pragma region Requests - Game <== AI

/**
 * @brief Class for handling the AI system requests.
 * @note Used for most of the queries (with the exception of the inspect query which requires a target).
 */
class VEAPI2_API FApi2CogAiSystemQueryMessage : public IApiMetadata {
public:
	/**
	 * @brief Query to send to the system.
	 */
	FString Query;
};

/**
 * @brief Class for handling the AI inspect system requests.
 */
class VEAPI2_API FApi2CogAiSystemInspectQueryMessage final : public FApi2CogAiSystemQueryMessage {
public:
	/**
	 * @brief Name of the object to inspect.
	 */
	FString Target;
};

#pragma endregion

#pragma region Responses - Game ==> AI

/**
 * @brief Response to the query message from the AI system.
 * @note This is the base class for all the responses.
 * @details This class is used as a base to send the information of current state of the game to the AI backend. 
 */
class VEAPI2_API FApi2CogAiSystemResponseMessage : public IApiMetadata {
public:
	/**
	 * @brief Query sent by the system.
	 */
	FString Query;
};

/**
 * @brief Response to the who am I query message from the AI system.
 */
class VEAPI2_API FApi2CogAiWhoAmIResponseMessage final : public FApi2CogAiSystemResponseMessage {
public:
	/**
	 * @brief Name of the npc.
	 */
	FString Name;

	/**
	 * @brief Description of the npc.
	 */
	FString Description;

	/**
	 * @brief Faction of the npc.
	 */
	FString Faction;

	/**
	 * @brief Attributes of the npc.
	 */
	TMap<FString, FString> Attributes;
};

/**
 * @brief Response to the context query message from the AI system.
 */
class VEAPI2_API FApi2CogAiContextResponseMessage final : public FApi2CogAiSystemResponseMessage {
public:
	/**
	 * @brief Description of the context.
	 */
	FString Description;

	/**
	 * @brief Location description, optional.
	 */
	FString Location;

	/**
	 * @brief Time description (day, night, etc.), optional.
	 */
	FString Time;

	/**
	 * @brief Weather description (rain, snow, etc.), optional.
	 */
	FString Weather;

	/**
	 * @brief Mood of the context (happy, sad, horror, etc.), optional.
	 */
	FString Mood;
};

/**
 * @brief Data of the perception query message for the AI system.
 */
class VEAPI2_API FApi2CogAiSystemPerceptionData : public IApiMetadata {
public:
	/**
	 * @brief Name of the entity perceived.
	 */
	FString Name;

	/**
	 * @brief Description of the entity perceived.
	 */
	FString Description;
};

/**
 * @brief Response to the perception query message from the AI system.
 */
class VEAPI2_API FApi2CogAiSystemPerceptionResponseMessage final : public FApi2CogAiSystemResponseMessage {
public:
	/**
	 * @brief Visual perception of the npc.
	 */
	TArray<FApi2CogAiSystemPerceptionData> Visual;

	/**
	 * @brief Audio perception of the npc.
	 */
	TArray<FApi2CogAiSystemPerceptionData> Audio;

	/**
	 * @brief Other perception of the npc, such as smell, temperature, health conditions, hunger and thirst, etc.
	 */
	TArray<FApi2CogAiSystemPerceptionData> Other;
};

/**
 * @brief Response to the inspect query message from the AI system.
 */
class VEAPI2_API FApi2CogAiSystemInspectResponseMessage final : public FApi2CogAiSystemResponseMessage {
public:
	/**
	 * @brief Name of the object to inspect.
	 */
	FString Name;

	/**
	 * @brief Description of the inspected object.
	 */
	FString Description;
};

#pragma endregion

#pragma endregion

class VEAPI2_API FApi2CogAiRequestMetadata : public IApiMetadata {
public:
	/**
	 * @brief Messages to send, log of recent AI actions and system messages.
	 */
	TArray<FApi2CogAiMessage> Messages;

	virtual TSharedPtr<FJsonObject> ToJson() override;
};

class VEAPI2_API FApi2CogAiUserRequestMetadata final : public FApi2CogAiRequestMetadata {
public:
	/** @brief User's OpenAI API key. */
	FString Key;

	/** @brief User defined version of the AI model to use. */
	FString Version;

	virtual TSharedPtr<FJsonObject> ToJson() override;
};

class VEAPI2_API FApi2CogAiResponseMessage : public FApi2CogAiMessage {
public:
	TMap<FString, FString> Parameters;

	virtual bool FromJson(const TSharedPtr<FJsonObject> JsonObject) override;
};

/**
 * @brief Response returned from the API for the Cognitive AI requests.
 */
class VEAPI2_API FApi2CogAiResponseMetadata final : public IApiMetadata {
public:
	/**
	 * @brief Messages to send, log of recent AI actions and system messages.
	 */
	TArray<FApi2CogAiResponseMessage> Messages;

	virtual bool FromJson(const TArray<TSharedPtr<FJsonValue>>&) override;
};

/**
 * @brief Request sent to the API for the Cognitive AI requests.
 */
class VEAPI2_API FApi2CogAiRequest : public FApi2Request {
public:
	/**
	 * @brief Metadata of the request.
	 * @details Contains the messages to send to the AI to make decisions based on the current state of the game.
	 */
	FApi2CogAiRequestMetadata RequestMetadata;

	/**
	 * @brief Metadata of the response, returned by the API.
	 * @details Contains the system queries and actions performed by the AI.
	 */
	FApi2CogAiResponseMetadata ResponseMetadata;

	virtual bool Process() override;

	virtual bool OnComplete(const FHttpRequestPtr& InRequest, const FHttpResponsePtr& InResponse, bool bSuccessful) override;
};

/**
 * @brief Request sent to the API for the Cognitive AI requests.
 */
class VEAPI2_API FApi2CogAiUserRequest : public FApi2Request {
public:
	/**
	 * @brief Metadata of the request.
	 * @details Contains the messages to send to the AI to make decisions based on the current state of the game.
	 */
	FApi2CogAiUserRequestMetadata RequestMetadata;

	/**
	 * @brief Metadata of the response, returned by the API.
	 * @details Contains the system queries and actions performed by the AI.
	 */
	FApi2CogAiResponseMetadata ResponseMetadata;

	virtual bool Process() override;

	virtual bool OnComplete(const FHttpRequestPtr& InRequest, const FHttpResponsePtr& InResponse, bool bSuccessful) override;
};
