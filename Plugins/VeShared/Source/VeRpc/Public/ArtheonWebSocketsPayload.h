// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once
#include "ArtheonJsonInterface.h"
#include "Dom/JsonObject.h"
#include "ArtheonWebSocketsPayload.generated.h"


USTRUCT(BlueprintType)
struct VERPC_API FArtheonWebSocketsUserAction {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Id;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString SenderId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString UserId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Action;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Details;

	TSharedPtr<FJsonObject> ToJson() const;
	void ParseJson(const TSharedPtr<FJsonObject> JsonObject);

	bool IsValid() const;
};

UENUM(BlueprintType)
enum class EArtheonVivoxChannelType : uint8 {
	Echo = 0,
	Positional = 1,
};

USTRUCT(BlueprintType)
struct VERPC_API FArtheonVivoxChannelProperties {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Id;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int AudibleDistance = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ConversationalDistance = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AudioFadeIntensity = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int AudioFadeModel = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EArtheonVivoxChannelType ChannelType = EArtheonVivoxChannelType::Echo;

	bool FORCEINLINE IsPositional() const { return ChannelType == EArtheonVivoxChannelType::Positional; }

	TSharedPtr<FJsonObject> ToJson() const;
	void ParseJson(const TSharedPtr<FJsonObject> JsonObject);

	bool IsValid() const;
};

USTRUCT(BlueprintType)
struct VERPC_API FArtheonWebSocketsVivoxPayload {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Issuer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int64 ExpiresAt = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Action;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int64 Serial = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Subject;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString From;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString To;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Token;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Server;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FArtheonVivoxChannelProperties ChannelProperties = FArtheonVivoxChannelProperties();

	TSharedPtr<FJsonObject> ToJson() const;
	void ParseJson(const TSharedPtr<FJsonObject> JsonObject);

	bool IsValid() const;
};

USTRUCT(BlueprintType)
struct VERPC_API FArtheonWebSocketsUser {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Id;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString AvatarUrl;

	TSharedPtr<FJsonObject> ToJson() const;
	void ParseJson(const TSharedPtr<FJsonObject> JsonObject);

	bool IsValid() const;
};

/** Base payload class */
struct VERPC_API FArtheonWebSocketsPayload final : FArtheonJsonInterface {
	FString Status;
	FString Message;
	FString ChannelId;
	FString Key;
	FArtheonWebSocketsUser User;
	FArtheonWebSocketsUserAction Action;
	FArtheonWebSocketsVivoxPayload VivoxPayload;
	FString Category;

	virtual TSharedPtr<FJsonObject> ToJson() override;
	virtual void ParseJson(const TSharedPtr<FJsonObject> JsonObject) override;
};
