// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "ApiLinkMetadata.h"

FString LinkTypeToString(const EApiLinkType& Type) {
	switch (Type) {
	case LT_Website:
		return Api::LinkTypes::Website;
	case LT_Facebook:
		return Api::LinkTypes::Facebook;
	case LT_Twitter:
		return Api::LinkTypes::Twitter;
	case LT_Youtube:
		return Api::LinkTypes::Youtube;
	case LT_Discord:
		return Api::LinkTypes::Discord;
	default:
		return Api::LinkTypes::Unexplored;
	}
}

EApiLinkType LinkTypeFromString(const FString& Source) {

	if (Source == Api::LinkTypes::Website) {
		return LT_Website;
	}

	if (Source == Api::LinkTypes::Facebook) {
		return LT_Facebook;
	}

	if (Source == Api::LinkTypes::Twitter) {
		return LT_Twitter;
	}

	if (Source == Api::LinkTypes::Youtube) {
		return LT_Youtube;
	}

	if (Source == Api::LinkTypes::Discord) {
		return LT_Discord;
	}

	return LT_Unknown;
}

bool FApiLinkMetadata::FromJson(const TSharedPtr<FJsonObject> JsonObject) {
	const bool bOk = IApiMetadata::FromJson(JsonObject);

	Type = LinkTypeFromString(GetJsonStringField(JsonObject, Api::Fields::LinkType));
	Url = GetJsonStringField(JsonObject, Api::Fields::Url);

	return bOk;
}
