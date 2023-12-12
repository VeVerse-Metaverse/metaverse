// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once
#include "AiPerceptionData.h"
#include "CogAiEnums.h"
#include "CogAiActionDefinition.h"

#include "CogAiMessage.generated.h"

class FApi2CogAiMessage;
class FApi2CogAiResponseMessage;

UCLASS(BlueprintType, Blueprintable)
class UCogAiMessage : public UObject {
	GENERATED_BODY()

public:
	UCogAiMessage() = default;
	UCogAiMessage(const FApi2CogAiMessage& Message);

	virtual operator FApi2CogAiMessage() const;
	virtual FApi2CogAiMessage ToApi2Message() const;
	virtual FString ToString() const;
	virtual bool operator==(const UCogAiMessage& Other) const;

	FString RoleToString() const;

	/**
	 * @brief Role of the message (system or NPC).
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	ECogAiMessageRole Role = ECogAiMessageRole::None;

	/**
	 * @brief Raw list of parameters of the message (action data or system query data).
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	TMap<FString, FString> Parameters;
};

/**
 * @brief Action message sent by the CogAI backend.
 */
UCLASS(BlueprintType)
class UCogAiActionMessage : public UCogAiMessage {
	GENERATED_BODY()

public:
	UCogAiActionMessage() = default;
	UCogAiActionMessage(const FApi2CogAiMessage& Message);
	virtual bool operator==(const UCogAiMessage& Other) const override;

	virtual FApi2CogAiMessage ToApi2Message() const override;
	virtual FString ToString() const override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	FString Type;
};

USTRUCT(BlueprintType)
struct FCogAiActionMessageReplicatedParameter {
	GENERATED_BODY()

	explicit FCogAiActionMessageReplicatedParameter(const FString& InName = "", const FString& InValue = "");

	UPROPERTY(VisibleInstanceOnly, Category = "AI")
	FString Name;

	UPROPERTY(VisibleInstanceOnly, Category = "AI")
	FString Value;
};

USTRUCT(BlueprintType)
struct FCogAiActionMessageClient {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	FString Type;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	TMap<FString, FString> Parameters;
};

/** @brief Used to replicate the action message to the clients. */
USTRUCT(BlueprintType)
struct FCogAiActionMessageReplicated {
	GENERATED_BODY()

	explicit FCogAiActionMessageReplicated(UCogAiActionMessage* InMessage = nullptr);

	FCogAiActionMessageClient ToClientMessage() const;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	FString Type;

	UPROPERTY(VisibleInstanceOnly, Category = "AI")
	TArray<FCogAiActionMessageReplicatedParameter> RawParameters;
};

/**
 * @brief System message sent by the system to the CogAI backend.
 */
UCLASS(BlueprintType)
class UCogAiSystemMessage : public UCogAiMessage {
	GENERATED_BODY()

public:
	UCogAiSystemMessage() = default;
	UCogAiSystemMessage(const FApi2CogAiMessage& Message);
	virtual bool operator==(const UCogAiMessage& Other) const override;

	virtual FApi2CogAiMessage ToApi2Message() const override;
	virtual FString ToString() const override;
	FString SystemTypeToString() const;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	ECogAiSystemMessageType Type;
};

UCLASS(BlueprintType)
class UCogAiSystemQueryMessage : public UCogAiSystemMessage {
	GENERATED_BODY()

public:
	UCogAiSystemQueryMessage() = default;
	UCogAiSystemQueryMessage(const FApi2CogAiMessage& Message);
	virtual bool operator==(const UCogAiMessage& Other) const override;

	virtual FApi2CogAiMessage ToApi2Message() const override;
	virtual FString ToString() const override;
};

/**
 * @brief System message sent by the system to the CogAI backend to register the NPC.
 */
UCLASS(BlueprintType)
class UCogAiWhoamiSystemMessage : public UCogAiSystemMessage {
	GENERATED_BODY()

public:
	UCogAiWhoamiSystemMessage();
	UCogAiWhoamiSystemMessage(const FApi2CogAiMessage& Message);

	virtual FApi2CogAiMessage ToApi2Message() const override;
	virtual FString ToString() const override;
	virtual bool operator==(const UCogAiMessage& Other) const override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	FString Name;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	FString Description;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	FString Faction;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	TMap<FString, int32> Personality;
};

UCLASS(BlueprintType)
class UCogAiActionsSystemMessage : public UCogAiSystemMessage {
	GENERATED_BODY()

public:
	UCogAiActionsSystemMessage();
	UCogAiActionsSystemMessage(const FApi2CogAiMessage& Message);
	virtual bool operator==(const UCogAiMessage& Other) const override;

	virtual FApi2CogAiMessage ToApi2Message() const override;
	virtual FString ToString() const override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	TArray<FCogAiActionDefinition> Actions;
};


UCLASS(BlueprintType)
class UCogAiContextSystemMessage : public UCogAiSystemMessage {
	GENERATED_BODY()

public:
	UCogAiContextSystemMessage();;
	UCogAiContextSystemMessage(const FApi2CogAiMessage& Message);
	virtual bool operator==(const UCogAiMessage& Other) const override;

	virtual FApi2CogAiMessage ToApi2Message() const override;
	virtual FString ToString() const override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	FString Description;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	FString Location;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	FString Time;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	FString Weather;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	FString Vibe;
};

UCLASS(BlueprintType)
class UCogAiInspectSystemMessage : public UCogAiSystemMessage {
	GENERATED_BODY()

public:
	UCogAiInspectSystemMessage();;
	UCogAiInspectSystemMessage(const FApi2CogAiMessage& Message);
	virtual bool operator==(const UCogAiMessage& Other) const override;

	virtual FApi2CogAiMessage ToApi2Message() const override;
	virtual FString ToString() const override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	FString TargetName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	FString TargetDescription;
};

USTRUCT(BlueprintType)
struct FCogAiPerception {
	GENERATED_BODY()

	bool operator==(const FCogAiPerception& Other) const;

	/**
	 * @brief Type of the perception.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	EAiPerception Type = EAiPerception::None;

	/**
	 * @brief Relative strength of the perception, from 0.0 to 1.0.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	float Strength = 1.0f;

	/**
	 * @brief Name of the perceived object. E.g. "John Doe". Can not be empty.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	FString Name;

	/**
	 * @brief Description of the perceived object. E.g. "Says: Hello!". Can be empty.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	FString Description;
};

UCLASS(BlueprintType)
class UCogAiPerceptionSystemMessage : public UCogAiSystemMessage {
	GENERATED_BODY()

public:
	UCogAiPerceptionSystemMessage();;
	UCogAiPerceptionSystemMessage(const FApi2CogAiMessage& Message);

	virtual FApi2CogAiMessage ToApi2Message() const override;
	virtual FString ToString() const override;
	virtual bool operator==(const UCogAiMessage& InOther) const override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	TArray<FCogAiPerception> Visual;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	TArray<FCogAiPerception> Audio;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	TArray<FCogAiPerception> Other;
};

/**
 * @brief CogAI decision taken by the NPC, received from the CogAI backend.
 */
USTRUCT(BlueprintType, meta=(Deprecated))
struct FCogAiDecision {
	GENERATED_BODY()

	FCogAiDecision() = default;
	FCogAiDecision(const FApi2CogAiResponseMessage& Message);

	/**
	 * @brief List of parameters for the decision (action data or system query data).
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	TMap<FString, FString> Parameters;
};
