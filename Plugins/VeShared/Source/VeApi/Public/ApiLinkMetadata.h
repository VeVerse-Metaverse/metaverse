// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once
#include "ApiEntityMetadata.h"

UENUM()
enum EApiLinkType {
	LT_Website,
	LT_Facebook,
	LT_Twitter,
	LT_Youtube,
	LT_Discord,
	LT_Unknown
};

static FString LinkTypeToString(const EApiLinkType& Type);
static EApiLinkType LinkTypeFromString(const FString& Source);

class VEAPI_API FApiLinkMetadata final : public IApiMetadata {
public:
	/** Type of the link. */
	EApiLinkType Type;

	/** Target URL. */
	FString Url;

	virtual bool FromJson(const TSharedPtr<FJsonObject> JsonObject) override;
};
