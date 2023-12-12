// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "AnalyticsMetadata.generated.h"

USTRUCT(BlueprintType)
struct VEGAMEFRAMEWORK_API FAnalyticsRequestMetadata {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Analytics")
	FString ContextEntityType;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Analytics")
	FGuid ContextEntityId;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Analytics")
	FGuid UserId;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Analytics")
	FString Event;
};

USTRUCT(BlueprintType)
struct VEGAMEFRAMEWORK_API FAnalyticsIndexRequestMetadata {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Analytics")
	int32 Offset = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Analytics")
	int32 Limit = 100;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Analytics")
	FString ContextEntityType;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Analytics")
	FGuid ContextEntityId;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Analytics")
	FGuid UserId;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Analytics")
	FString Platform;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Analytics")
	FString Deployment;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Analytics")
	FString Event;
};

USTRUCT(BlueprintType)
struct VEGAMEFRAMEWORK_API FAnalyticsResponseMetadata {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Analytics")
	FString ContextEntityType;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Analytics")
	FGuid ContextEntityId;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Analytics")
	FGuid UserId;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Analytics")
	FString Platform;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Analytics")
	FString Deployment;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Analytics")
	FString Configuration;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Analytics")
	FString Event;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Analytics")
	FDateTime Timestamp;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Analytics")
	FString Payload;
};
