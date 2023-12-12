// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


#pragma once

#include "ApiPropertyMetadata.h"
#include "VePropertyMetadata.generated.h"


UENUM(BlueprintType)
enum class EVePropertyType : uint8 {
	None,
	String
};


/**
 * 
 */
USTRUCT(BlueprintType)
struct VEGAMEFRAMEWORK_API FVePropertyMetadata {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Name;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EVePropertyType Type = EVePropertyType::None;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Value;

	FVePropertyMetadata() = default;
	explicit FVePropertyMetadata(const FApiPropertyMetadata& ApiMetadata);

	FApiPropertyMetadata ToApiMetadata() const;

	static EVePropertyType StringToType(const FString Type);
	static FString TypeToString(EVePropertyType Type);
	static TArray<FVePropertyMetadata> ToArray(const TArray<FApiPropertyMetadata>& ApiProperties);
};

USTRUCT()
struct VEGAMEFRAMEWORK_API FVePropertyBatchMetadata {
	GENERATED_BODY()

	UPROPERTY()
	TArray<FVePropertyMetadata> List;

	void Deserialize(const TArray<uint8>& Bytes);
	TArray<uint8> Serialize();
};
