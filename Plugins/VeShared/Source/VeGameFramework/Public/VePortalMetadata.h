// Author: Egor A. Pristavka

#pragma once

#include "ApiPortalMetadata.h"
#include "VeWorldMetadata.h"

#include "VePortalMetadata.generated.h"

USTRUCT(BlueprintType)
struct VEGAMEFRAMEWORK_API FVePortalDestinationMetadata {
	GENERATED_BODY()

	FVePortalDestinationMetadata() = default;
	explicit FVePortalDestinationMetadata(const FApiPortalDestinationMetadata& InApiMetadata);

	/** Destination portal identifier */
	UPROPERTY(EditAnywhere)
	FGuid Id;

	/** Portal editor name */
	UPROPERTY(EditAnywhere)
	FString Name;

	/** Destination portal space */
	UPROPERTY(EditAnywhere)
	FVeWorldMetadata World;

	/** List of destination portal files such as preview images */
	UPROPERTY(EditAnywhere)
	TArray<FVeFileMetadata> Files;
};

USTRUCT(BlueprintType)
struct VEGAMEFRAMEWORK_API FVePortalMetadata {
	GENERATED_BODY()

	FVePortalMetadata() = default;
	explicit FVePortalMetadata(const FApiPortalMetadata& InApiMetadata);

	/** Portal identifier */
	UPROPERTY(EditAnywhere)
	FGuid Id;

	/** Portal name */
	UPROPERTY(EditAnywhere)
	FString Name;

	/** Destination portal metadata */
	UPROPERTY(EditAnywhere, AdvancedDisplay)
	FVePortalDestinationMetadata Destination = {};
};
