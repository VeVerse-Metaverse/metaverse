// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once
#include "VeFileMetadata.h"
#include "VePropertyMetadata.h"
#include "VeUserMetadata.h"

#include "VePlaceableMetadata.generated.h"

class FApiPlaceableMetadata;
class FApiPlaceableClassMetadata;

USTRUCT(BlueprintType)
struct VEGAMEFRAMEWORK_API FVePlaceableClassMetadata {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGuid Id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ClassName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Category;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> Class;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVeFileMetadata> Files;

	FVePlaceableClassMetadata() = default;
	explicit FVePlaceableClassMetadata(const FApiPlaceableClassMetadata& ApiMetadata);
};

/**
 * 
 */
USTRUCT(BlueprintType)
struct VEGAMEFRAMEWORK_API FVePlaceableMetadata {
	GENERATED_BODY()

	/** Unique ID of the entity. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGuid Id;

	/** Position. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Position = FVector::ZeroVector;

	/** Rotation. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator Rotation = FRotator::ZeroRotator;

	/** Scale. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Scale = FVector::ZeroVector;

	/** ObjectId if applicable. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGuid EntityId;

	/** Placeable type. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Type;

	/** User owner. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVeUserMetadata Owner;

	/** Files. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVeFileMetadata> Files;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVePlaceableClassMetadata PlaceableClassMetadata;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> Class;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVePropertyMetadata> Properties;

	FVePlaceableMetadata() = default;
	explicit FVePlaceableMetadata(const FApiPlaceableMetadata& ApiMetadata);
};
