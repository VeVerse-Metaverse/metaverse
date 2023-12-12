// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "ApiServerMetadata.h"

enum class EApi2ServerStatus : uint8 {
	None,
	Created,
	Launching,
	Downloading,
	Starting,
	Online,
	Offline,
	Error,
};

EApi2ServerStatus VEAPI2_API Api2ServerStatusFromString(const FString& Status);
FString VEAPI2_API Api2ServerStatusToString(EApi2ServerStatus Status);

enum class EApi2ServerType : uint8 {
	None,
	Community,
	Official,
};


class VEAPI2_API FApi2ServerMetadata final : public FApiEntityMetadata {
public:
	/** Server Id */
	FGuid Id;

	/** Release Id of the server. */
	FGuid ReleaseId;

	/** Id of the world running at the server. */
	FGuid WorldId;

	/** GameMode Id of the server. */
	FGuid GameModeId;

	/** Region Id of the server. */
	FGuid RegionId;

	/** Type of the server. */
	EApi2ServerType Type = EApi2ServerType::Official;

	/** Public address. */
	FString Host = TEXT("0.0.0.0");

	/** Porn number to listen at. */
	int32 Port = 7777;

	/** Maximum allowed players. */
	int32 MaxPlayers = 0;

	/** Status of the server. */
	EApi2ServerStatus Status = EApi2ServerStatus::None;

	/** Status message of the server. */
	FString StatusMessage;

	/** Is this server public and accessible for everyone. */
	bool bPublic = true;

	virtual bool FromJson(const TSharedPtr<FJsonObject> InJsonObject) override;
};

class VEAPI2_API FApi2RequestServerHeartbeatMetadata final : public IApiMetadata {
public:
	EApi2ServerStatus Status = EApi2ServerStatus::None;
	TArray<FGuid> OnlinePlayerIds;

	virtual TSharedPtr<FJsonObject> ToJson() override;
};

class VEAPI2_API FApi2RequestServerMatchMetadata final : public IApiMetadata {
public:
	FGuid WorldId;
	FGuid GameModeId;
	EApi2ServerType Type = EApi2ServerType::Official;

	virtual TSharedPtr<FJsonObject> ToJson() override;
};


/** Used for user connect requests. */
class VEAPI2_API FApi2RequestPlayerConnectMetadata final : public IApiMetadata {
public:
	FGuid PlayerId;

	virtual TSharedPtr<FJsonObject> ToJson() override;
};
