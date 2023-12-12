// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "GameServerMetadata.generated.h"

USTRUCT()
struct FGameServerMetadata {
	GENERATED_BODY()

	/** Server id. */
	UPROPERTY()
	FGuid Id;

	/** Map running on the server. */
	UPROPERTY()
	FString Map;

	/** Game mode running on the server. */
	UPROPERTY()
	FString GameMode;
};

DECLARE_DELEGATE_OneParam(FOnGameServerRegistered, const FGameServerMetadata& /*InMetadata*/)