// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once
#include "ApiEntityMetadata.h"
#include "ApiPresenceMetadata.generated.h"

class FApiSpaceMetadata;
class FApiServerMetadata;

UENUM(BlueprintType)
enum class EApiPresenceStatus : uint8 {
	/** 0: Not online or timed out. */
	Offline,
	/** 1: Had no activity for some time, but not timed out. */
	Away,
	/** 2: Is in standalone mode, available to join online game. */
	Available,
	/** 3: Playing at online servers. */
	Playing,
};

inline FString VEAPI_API PresenceStatusToString(const EApiPresenceStatus& Type) {
	switch (Type) {
	case EApiPresenceStatus::Away:
		return Api::Presence::Away;
	case EApiPresenceStatus::Available:
		return Api::Presence::Available;
	case EApiPresenceStatus::Playing:
		return Api::Presence::Playing;
	default:
		return Api::Presence::Offline;
	}
}

inline EApiPresenceStatus VEAPI_API PresenceStatusFromString(const FString& Source) {
	if (Source == Api::Presence::Away) {
		return EApiPresenceStatus::Away;
	}

	if (Source == Api::Presence::Available) {
		return EApiPresenceStatus::Available;
	}

	if (Source == Api::Presence::Playing) {
		return EApiPresenceStatus::Playing;
	}

	return EApiPresenceStatus::Offline;
}

class VEAPI_API FApiPresenceMetadata final : public IApiMetadata {
public:
	/** Current server ID. */
	TSharedPtr<FApiServerMetadata> ServerMetadata;

	/** Current space ID. */
	TSharedPtr<FApiSpaceMetadata> SpaceMetadata;

	/** Current presence status. */
	EApiPresenceStatus Status;

	virtual bool FromJson(const TSharedPtr<FJsonObject> JsonObject) override;
	virtual TSharedPtr<FJsonObject> ToJson() override;
};
