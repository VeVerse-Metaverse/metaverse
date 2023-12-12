// Author: Egor A. Pristavka

#pragma once

#include "VeServerMetadata.generated.h"

class FApiServerMetadata;
class FApiUpdateServerMetadata;

USTRUCT(BlueprintType)
struct VEGAMEFRAMEWORK_API FVeServerMetadata {
	GENERATED_BODY()

	FVeServerMetadata();
	explicit FVeServerMetadata(const FApiServerMetadata& InMetadata);
	operator FApiUpdateServerMetadata() const;

	/** Portal ID. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, SimpleDisplay)
	FGuid Id;

	/** Destination portal ID. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, SimpleDisplay)
	FGuid SpaceId;

	/** Destination portal ID. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, SimpleDisplay)
	FGuid UserId;

	/** Game mode at the server. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString GameMode = TEXT("");

	/** Public address. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Host = TEXT("0.0.0.0");

	/** Map. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Map = {};

	/** Status. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Status = {};

	/** Porn number to listen at. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Port = 7777;

	/** Current online player number. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 OnlinePlayers = 0;

	/** Maximum allowed players. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 MaxPlayers = 0;

	/** Is this server public and accessible for everyone. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bPublic = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FDateTime CreatedAt;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FDateTime UpdatedAt;

	static FApiUpdateServerMetadata ToUpdateServerMetadata(const FVeServerMetadata& InServerMetadata);
};
