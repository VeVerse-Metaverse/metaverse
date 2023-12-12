// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "ApiSpaceMetadata.h"
#include "VePackageMetadata.h"
#include "VeUserMetadata.h"
#include "VeWorldMetadata.generated.h"


/**
 * 
 */
USTRUCT(BlueprintType)
struct VEGAMEFRAMEWORK_API FVeWorldMetadata {
	GENERATED_BODY()

	FVeWorldMetadata() = default;
	explicit FVeWorldMetadata(const FApiSpaceMetadata& InMetadata);
	operator FApiUpdateSpaceMetadata() const;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGuid Id = FGuid{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Name = FString{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Description = FString{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Map = FString{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString GameMode = FString{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVePackageMetadata Package = {};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVeUserMetadata Owner = {};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FVeFileMetadata> Files = {};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bPublic = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Views = 0;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 TotalLikes = 0;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 TotalDislikes = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Likes = 0;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Dislikes = 0;

	int32 Liked;
	
	static FString GetPakUrl(const FVeWorldMetadata& WorldMetadata);
	static FGuid GetPakId(const FVeWorldMetadata& WorldMetadata);

	static TArray<FVeWorldMetadata> MakeArray(const TArray<TSharedPtr<FApiSpaceMetadata>>& rhs);
};

// UCLASS()
// class VEGAMEFRAMEWORK_API UVeSpaceMetadataObject final : public UObject {
// 	GENERATED_BODY()
//
// public:
// 	
// 	UPROPERTY()
// 	FVeSpaceMetadata Metadata;
//
// 	static TArray<UObject*> BatchToUObjectArray(TArray<FVeSpaceMetadata> InEntities, UObject* InParent = nullptr);
// };