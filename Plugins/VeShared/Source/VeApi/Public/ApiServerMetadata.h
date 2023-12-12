// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once
#include "ApiEntityMetadata.h"

class UApiServerMetadataObject;

enum class EApiServerStatus : uint8 {
	None,
	Created,
	Launching,
	Downloading,
	Starting,
	Online,
	Offline,
	Error
};

EApiServerStatus VEAPI_API ApiServerStatusFromString(const FString& Status);
FString VEAPI_API ApiServerStatusToString(EApiServerStatus Status);


class VEAPI_API FApiServerMetadata final : public FApiEntityMetadata {
public:
	FGuid Id = FGuid();

	/** Id of the space running at the server. */
	FGuid SpaceId = FGuid();

	/** User id. */
	FGuid UserId = FGuid();

	/** Game mode to start at the server. */
	FString GameMode = TEXT("");

	EApiServerStatus Status = EApiServerStatus::None;
	FString Details = TEXT("");

	/** Public address. */
	FString Host = TEXT("0.0.0.0");

	/** Porn number to listen at. */
	int32 Port = 7777;

	/** Current online player number. */
	int32 OnlinePlayers = 0;

	/** Maximum allowed players. */
	int32 MaxPlayers = 0;

	/** Is this server public and accessible for everyone. */
	bool bPublic = true;

	/** Map running at the server. */
	FString Map;

	virtual bool FromJson(const TSharedPtr<FJsonObject> InJsonObject) override;
};

class VEAPI_API FApiUpdateServerMetadata final : public IApiMetadata {
public:
	/** Id of the server. */
	FGuid Id = FGuid();

	/** Id of the space running at the server. */
	FGuid SpaceId = FGuid();

	/** Game mode to start at the server. */
	FString GameMode = TEXT("");

	/** Public address. */
	FString Host = TEXT("0.0.0.0");

	/** Porn number to listen at. */
	int32 Port = 7777;

	/** Maximum allowed players. */
	int32 MaxPlayers = 0;

	/** Is this server public and accessible for everyone. */
	bool bPublic = true;

	FString Map;
	FString Status = TEXT("");
	FString Details = TEXT("");
	int32 OnlinePlayers = 0;

	static FApiUpdateServerMetadata FromMetadata(const FApiServerMetadata& InMetadata);
	static FApiUpdateServerMetadata FromMetadataObject(UApiServerMetadataObject* InMetadataObject);

	virtual TSharedPtr<FJsonObject> ToJson() override;
};

typedef TApiBatchMetadata<class FApiServerMetadata> FApiServerBatchMetadata;
