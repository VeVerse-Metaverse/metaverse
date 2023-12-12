// Author: Egor A. Pristavka

#pragma once
#include "VePersonaMetadata.h"

#include "VeUserMetadata.generated.h"

class FApiUserMetadata;

USTRUCT(BlueprintType)
struct VEGAMEFRAMEWORK_API FVeUserMetadata {
	GENERATED_BODY()

	FVeUserMetadata();
	explicit FVeUserMetadata(const FApiUserMetadata& InUserMetadata);
	FApiUserMetadata ToApiUserMetadata() const;
	~FVeUserMetadata() = default;

	/** User identifier. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FGuid Id;

	/** Display name of the user. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString Name;

	/** Description of the user. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString Description;

	/** Display title of the user. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString Title;

	/** Avatar URL. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString AvatarUrl;

	/** Level. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 Level;

	/** Rating. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float Rating;

	/** Default persona metadata. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVePersonaMetadata DefaultPersonaMetadata;

	/** Is user an administrator. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsAdmin;

	/** Is user muted globally. This means that the user is not allowed to use microphone at any online server. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsMuted;
};

USTRUCT(BlueprintType)
struct VEGAMEFRAMEWORK_API FVeAiUserMetadata {
	GENERATED_BODY()

	FVeAiUserMetadata();
	explicit FVeAiUserMetadata(const FApiUserMetadata& InUserMetadata);
	~FVeAiUserMetadata() = default;

	/** User identifier. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FGuid Id;

	/** Display name of the user. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString Name;

	/** Description of the user. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString Description;

	/** Default persona metadata. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVePersonaMetadata DefaultPersonaMetadata;
};
