// Author: Egor A. Pristavka

#pragma once

#include "VeFileMetadata.h"
#include "VePersonaMetadata.generated.h"

class FApiPersonaMetadata;

USTRUCT(BlueprintType)
struct VEGAMEFRAMEWORK_API FVePersonaMetadata {
	GENERATED_BODY()

	FVePersonaMetadata();
	explicit FVePersonaMetadata(const FApiPersonaMetadata& InPersonaMetadata);

	/** Persona identifier. */
	UPROPERTY(VisibleAnywhere)
	FGuid Id;

	/** Persona name. */
	UPROPERTY(VisibleAnywhere)
	FString Name;

	/** Persona description. */
	UPROPERTY(VisibleAnywhere)
	FString Description;

	/** Persona name. */
	UPROPERTY(VisibleAnywhere)
	FString Configuration;

	/** Persona type. */
	UPROPERTY(VisibleAnywhere)
	FString Type;

	/** List of all files attached to the persona. */
	UPROPERTY(VisibleAnywhere)
	TArray<FVeFileMetadata> Files;
};
