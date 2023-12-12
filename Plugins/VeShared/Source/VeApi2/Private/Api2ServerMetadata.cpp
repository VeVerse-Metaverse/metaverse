// Authors: Khusan Yadgarov, Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "Api2ServerMetadata.h"

#include "VeGameUserSettings.h"


EApi2ServerStatus Api2ServerStatusFromString(const FString& Status) {
	if (Status == TEXT("created")) {
		return EApi2ServerStatus::Created;
	} else if (Status == TEXT("launching")) {
		return EApi2ServerStatus::Launching;
	} else if (Status == TEXT("downloading")) {
		return EApi2ServerStatus::Downloading;
	} else if (Status == TEXT("starting")) {
		return EApi2ServerStatus::Starting;
	} else if (Status == TEXT("online")) {
		return EApi2ServerStatus::Online;
	} else if (Status == TEXT("offline")) {
		return EApi2ServerStatus::Offline;
	} else if (Status == TEXT("error")) {
		return EApi2ServerStatus::Error;
	} else {
		return EApi2ServerStatus::None;
	}
}

FString Api2ServerStatusToString(EApi2ServerStatus Status) {
	switch (Status) {
	case EApi2ServerStatus::Created:
		return TEXT("created");
	case EApi2ServerStatus::Launching:
		return TEXT("launching");
	case EApi2ServerStatus::Downloading:
		return TEXT("downloading");
	case EApi2ServerStatus::Starting:
		return TEXT("starting");
	case EApi2ServerStatus::Online:
		return TEXT("online");
	case EApi2ServerStatus::Offline:
		return TEXT("offline");
	case EApi2ServerStatus::Error:
		return TEXT("error");
	default:
		return TEXT("none");
	}
}

bool FApi2ServerMetadata::FromJson(const TSharedPtr<FJsonObject> InJsonObject) {
	bool bOk = FApiEntityMetadata::FromJson(InJsonObject);

	Id = GetJsonGuidField(InJsonObject, Api::Fields::Id);
	ReleaseId = GetJsonGuidField(InJsonObject, Api::Fields::ReleaseId);
	WorldId = GetJsonGuidField(InJsonObject, Api::Fields::WorldId);
	GameModeId = GetJsonGuidField(InJsonObject, Api::Fields::GameModeId);
	RegionId = GetJsonGuidField(InJsonObject, Api::Fields::RegionId);
	Host = GetJsonStringField(InJsonObject, Api::Fields::Host);
	Port = GetJsonIntField(InJsonObject, Api::Fields::Port);
	MaxPlayers = GetJsonIntField(InJsonObject, Api::Fields::MaxPlayers);
	StatusMessage = GetJsonStringField(InJsonObject, Api::Fields::StatusMessage);
	bPublic = GetJsonBoolField(InJsonObject, Api::Fields::Public);

	const FString TypeStr = GetJsonStringField(InJsonObject, Api::Fields::Type);
	if (TypeStr == TEXT("official")) {
		Type = EApi2ServerType::Official;
	} else if (TypeStr == TEXT("community")) {
		Type = EApi2ServerType::Community;
	} else {
		Type = EApi2ServerType::None;
	}

	Status = Api2ServerStatusFromString(GetJsonStringField(InJsonObject, Api::Fields::Status));

	//return FApiEntityMetadata::FromJson(InJsonObject);
	return bOk;
}

TSharedPtr<FJsonObject> FApi2RequestServerHeartbeatMetadata::ToJson() {
	TSharedPtr<FJsonObject> JsonObject = IApiMetadata::ToJson();

	JsonObject->SetStringField(Api::Fields::Status, Api2ServerStatusToString(Status));

	if (OnlinePlayerIds.Num()) {
		TArray<TSharedPtr<FJsonValue>> Players;
		Players.Reserve(OnlinePlayerIds.Num());
		for (auto& PlayerId : OnlinePlayerIds) {
			Players.Emplace(MakeShared<FJsonValueString>(PlayerId.ToString(EGuidFormats::DigitsWithHyphensLower)));
		}
		JsonObject->SetArrayField(Api::Fields::OnlinePlayerIds, Players);
	}

	return JsonObject;
}

TSharedPtr<FJsonObject> FApi2RequestServerMatchMetadata::ToJson() {
	TSharedPtr<FJsonObject> JsonObject = IApiMetadata::ToJson();
	JsonObject->SetStringField(Api::Fields::ReleaseId, FVeConfigLibrary::GetAppReleaseId());
	JsonObject->SetStringField(Api::Fields::WorldId, WorldId.ToString(EGuidFormats::DigitsWithHyphensLower));

	if (auto* GameUserSettings = UVeGameUserSettings::Get()) {
		JsonObject->SetStringField(Api::Fields::RegionId, GameUserSettings->RegionId.ToString(EGuidFormats::DigitsWithHyphensLower));
	}

	if (GameModeId.IsValid()) {
		JsonObject->SetStringField(Api::Fields::GameModeId, GameModeId.ToString(EGuidFormats::DigitsWithHyphensLower));
	}

	switch (Type) {
	case EApi2ServerType::Official:
		JsonObject->SetStringField(Api::Fields::Type, TEXT("official"));
		break;
	}

	return JsonObject;
}

TSharedPtr<FJsonObject> FApi2RequestPlayerConnectMetadata::ToJson() {
	TSharedPtr<FJsonObject> JsonObject = IApiMetadata::ToJson();
	JsonObject->SetStringField(Api::Fields::PlayerId, PlayerId.ToString(EGuidFormats::DigitsWithHyphensLower));
	return JsonObject;
}
