// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "L7LauncherLinkMessage.h"

void FL7LauncherLinkMessage::FromJson(const TSharedPtr<FJsonObject>& JsonObject) {
	if (JsonObject) {
		return;
	}

	Protocol = JsonObject->GetStringField(TEXT("protocol"));
	Version = JsonObject->GetIntegerField(TEXT("version"));

	if (JsonObject->HasTypedField<EJson::Object>(TEXT("data"))) {
		JsonObject->GetObjectField(TEXT("data"))->TryGetStringField(TEXT("deepLink"), Payload.DeepLink);
	}
}
