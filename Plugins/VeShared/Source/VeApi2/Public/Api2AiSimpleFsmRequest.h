// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "Api2Request.h"

class VEAPI2_API FApi2AiSimpleFsmEntity : public IApiMetadata {
public:
	/**
	 * @brief Name of the entity (used as ID), e.g. "John Doe".
	 */
	FString Name;

	/**
	 * @brief Description of the entity, e.g. "John Doe is an AI controlled virtual human avatar".
	 */
	FString Description;

	virtual TSharedPtr<FJsonObject> ToJson() override;
	virtual bool FromJson(const TSharedPtr<FJsonObject> JsonObject) override;
	virtual FString ToUrlParams() override;
};

class VEAPI2_API FApi2AiSimpleFsmLocationLink final : public FApi2AiSimpleFsmEntity {
public:
	/**
	 * @brief Target location of the link, e.g. "Office cubicle".
	 */
	FString Target;

	/**
	 * @brief Object that can be used to interact with the link, e.g. "Portal to the Nowhere".
	 */
	FString Object;

	virtual TSharedPtr<FJsonObject> ToJson() override;
};

class VEAPI2_API FApi2AiSimpleFsmEntities final : public IApiMetadata {
public:
	/**
	 * @brief List of NPCs in the location.
	 */
	TArray<FApi2AiSimpleFsmEntity> NPCs;

	/**
	 * @brief List of players in the location.
	 */
	TArray<FApi2AiSimpleFsmEntity> Players;

	/**
	 * @brief List of objects in the location.
	 */
	TArray<FApi2AiSimpleFsmEntity> Objects;

	virtual TSharedPtr<FJsonObject> ToJson() override;
};

class VEAPI2_API FApi2AiSimpleFsmLocation final : public FApi2AiSimpleFsmEntity {
public:
	/**
	 * @brief List of links to other locations.
	 */
	TArray<FApi2AiSimpleFsmLocationLink> Links;

	/**
	 * @brief Map of entities in the location.
	 */
	FApi2AiSimpleFsmEntities Entities;

	virtual TSharedPtr<FJsonObject> ToJson() override;
};

class VEAPI2_API FApi2AiSimpleFsmRequestMetadata final : public IApiMetadata {
public:
	FApi2AiSimpleFsmRequestMetadata();

	/**
	 * @brief Number or states to generate.
	 */
	int32 StateNum = 10;

	/**
	 * @brief Context to generate the script in.
	 */
	FString Context = {};

	/**
	 * @brief List of locations to be used in the act.
	 */
	TArray<FApi2AiSimpleFsmLocation> Locations;

	virtual TSharedPtr<FJsonObject> ToJson() override;
};

class VEAPI2_API FApi2AiSimpleFsmStateMetadata final : public IApiMetadata {
public:
	FApi2AiSimpleFsmStateMetadata();

	/**
	 * @brief Subject to act.
	 */
	FString NPC = {};

	/**
	 * @brief Action to perform.
	 */
	FString Action = {};

	/**
	 * @brief Context to perform the action in.
	 */
	FString Context = {};

	/**
	 * @brief Target of the action.
	 */
	FString Target = {};

	/**
	 * @brief Metadata for the action.
	 */
	FString Metadata = {};

	/**
	 * @brief Emotion of the NPC.
	 */
	FString Emotion = {};

	virtual bool FromJson(const TSharedPtr<FJsonObject> JsonObject) override;

	/**
	 * @brief Parse from a serialized string.
	 */
	void FromPlayScriptString(const FString& InString);

	/**
	 * @brief Convert to a serialized string.
	 */
	FString ToString() const;
};

class VEAPI2_API FApi2AiSimpleFsmMetadata final : public IApiMetadata {
public:
	FApi2AiSimpleFsmMetadata();

	/**
	 * @brief List of states in the scene.
	 */
	TArray<FApi2AiSimpleFsmStateMetadata> States;

	/**
	 * @brief Parse from a serialized string.
	 */
	bool FromString(const FString& InString);

	/**
	 * @brief Parse from a JSON object.
	 */
	virtual bool FromJson(const TSharedPtr<FJsonObject> JsonObject) override;
};

class VEAPI2_API FApi2AiSimpleFsmRequest final : public FApi2Request {
public:
	virtual bool Process() override;

	FApi2AiSimpleFsmRequestMetadata RequestMetadata;
	FApi2AiSimpleFsmMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

typedef TDelegate<void(const FApi2AiSimpleFsmMetadata& InMetadata, EApi2ResponseCode InResponseCode, const FString& InError)> FOnAiSimpleFsmRequestCompleted2;
