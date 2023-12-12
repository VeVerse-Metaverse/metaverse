// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "ApiPresenceMetadata.h"

#include "ApiServerMetadata.h"
#include "ApiSpaceMetadata.h"

bool FApiPresenceMetadata::FromJson(const TSharedPtr<FJsonObject> JsonObject) {
	const bool bOk = IApiMetadata::FromJson(JsonObject);

	const auto ServerObject = GetJsonObjectField(JsonObject, Api::Fields::Server);
	ServerMetadata = MakeShared<FApiServerMetadata>();
	if (ServerObject) {
		ServerMetadata->FromJson(ServerObject);
	}

	const auto SpaceObject = GetJsonObjectField(JsonObject, Api::Fields::Space);
	SpaceMetadata = MakeShared<FApiSpaceMetadata>();
	if (SpaceObject) {
		SpaceMetadata->FromJson(SpaceObject);
	}

	Status = PresenceStatusFromString(GetJsonStringField(JsonObject, Api::Fields::Status));

	return bOk;
}

TSharedPtr<FJsonObject> FApiPresenceMetadata::ToJson() {
	TSharedPtr<FJsonObject> JsonObject = IApiMetadata::ToJson();
	JsonObject->SetObjectField(Api::Fields::Server, ServerMetadata->ToJson());
	JsonObject->SetObjectField(Api::Fields::Space, SpaceMetadata->ToJson());
	JsonObject->SetStringField(Api::Fields::Status, PresenceStatusToString(Status));
	return JsonObject;
}
