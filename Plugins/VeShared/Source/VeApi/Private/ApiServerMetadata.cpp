// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "ApiServerMetadata.h"
#include "ApiServerMetadataObject.h"


typedef TSharedPtr<FJsonObject> FJsonObjectPtr;

EApiServerStatus ApiServerStatusFromString(const FString& Status) {
	if (Status == TEXT("created")) {
		return EApiServerStatus::Created;
	} else if (Status == TEXT("launching")) {
		return EApiServerStatus::Launching;
	} else if (Status == TEXT("downloading")) {
		return EApiServerStatus::Downloading;
	} else if (Status == TEXT("starting")) {
		return EApiServerStatus::Starting;
	} else if (Status == TEXT("online")) {
		return EApiServerStatus::Online;
	} else if (Status == TEXT("offline")) {
		return EApiServerStatus::Offline;
	} else if (Status == TEXT("error")) {
		return EApiServerStatus::Error;
	} else {
		return EApiServerStatus::None;
	}
}

FString ApiServerStatusToString(EApiServerStatus Status) {
	switch (Status) {
	case EApiServerStatus::Created:
		return TEXT("created");
	case EApiServerStatus::Launching:
		return TEXT("launching");
	case EApiServerStatus::Downloading:
		return TEXT("downloading");
	case EApiServerStatus::Starting:
		return TEXT("starting");
	case EApiServerStatus::Online:
		return TEXT("online");
	case EApiServerStatus::Offline:
		return TEXT("offline");
	case EApiServerStatus::Error:
		return TEXT("error");
	default:
		return TEXT("none");
	}
}

bool FApiServerMetadata::FromJson(const TSharedPtr<FJsonObject> InJsonObject) {
	bool bOk = FApiEntityMetadata::FromJson(InJsonObject);

	Id = GetJsonGuidField(InJsonObject, Api::Fields::Id);
	SpaceId = GetJsonGuidField(InJsonObject, Api::Fields::SpaceId);
	UserId = GetJsonGuidField(InJsonObject, Api::Fields::UserId);
	GameMode = GetJsonStringField(InJsonObject, Api::Fields::GameMode);
	Host = GetJsonStringField(InJsonObject, Api::Fields::Host);
	Port = GetJsonIntField(InJsonObject, Api::Fields::Port);
	OnlinePlayers = GetJsonIntField(InJsonObject, Api::Fields::OnlinePlayers);
	MaxPlayers = GetJsonIntField(InJsonObject, Api::Fields::MaxPlayers);
	bPublic = GetJsonBoolField(InJsonObject, Api::Fields::Public);
	Map = GetJsonStringField(InJsonObject, Api::Fields::Map);
	Status = ApiServerStatusFromString(GetJsonStringField(InJsonObject, Api::Fields::Status));
	Details = GetJsonStringField(InJsonObject, Api::Fields::Details);

	//return FApiEntityMetadata::FromJson(InJsonObject);
	return bOk;
}

FApiUpdateServerMetadata FApiUpdateServerMetadata::FromMetadata(const FApiServerMetadata& InMetadata) {
	FApiUpdateServerMetadata Metadata;
	if (InMetadata.Id.IsValid()) {
		Metadata.Id = InMetadata.Id;
	} else {
		Metadata.Id = FGuid();
	}

	if (InMetadata.SpaceId.IsValid()) {
		Metadata.SpaceId = InMetadata.SpaceId;
	} else {
		Metadata.SpaceId = FGuid();
	}
	Metadata.GameMode = InMetadata.GameMode;
	Metadata.Host = InMetadata.Host;
	Metadata.Port = InMetadata.Port;
	Metadata.MaxPlayers = InMetadata.MaxPlayers;
	Metadata.bPublic = InMetadata.bPublic;
	Metadata.Map = InMetadata.Map;
	Metadata.Status = ApiServerStatusToString(InMetadata.Status);
	Metadata.Details = InMetadata.Details;
	return Metadata;

}

FApiUpdateServerMetadata FApiUpdateServerMetadata::FromMetadataObject(UApiServerMetadataObject* InMetadataObject) {
	if (!InMetadataObject) {
		return FApiUpdateServerMetadata();
	}

	FApiUpdateServerMetadata Metadata;
	if (InMetadataObject->Metadata->Id.IsValid()) {
		Metadata.Id = InMetadataObject->Metadata->Id;
	} else {
		Metadata.Id = FGuid();
	}
	if (InMetadataObject->Metadata->SpaceId.IsValid()) {
		Metadata.SpaceId = InMetadataObject->Metadata->SpaceId;
	} else {
		Metadata.SpaceId = FGuid();
	}
	Metadata.GameMode = InMetadataObject->Metadata->GameMode;
	Metadata.Host = InMetadataObject->Metadata->Host;
	Metadata.Port = InMetadataObject->Metadata->Port;
	Metadata.MaxPlayers = InMetadataObject->Metadata->MaxPlayers;
	Metadata.bPublic = InMetadataObject->Metadata->bPublic;
	Metadata.Map = InMetadataObject->Metadata->Map;
	Metadata.Status = ApiServerStatusToString(InMetadataObject->Metadata->Status);
	Metadata.Details = InMetadataObject->Metadata->Details;
	return Metadata;
}

TSharedPtr<FJsonObject> FApiUpdateServerMetadata::ToJson() {
	const FJsonObjectPtr JsonObject = IApiMetadata::ToJson();
	if (Id.IsValid()) {
		JsonObject->SetStringField(Api::Fields::Id, Id.ToString(EGuidFormats::DigitsWithHyphens));
	}
	if (SpaceId.IsValid()) {
		JsonObject->SetStringField(Api::Fields::SpaceId, SpaceId.ToString(EGuidFormats::DigitsWithHyphens));
	}
	JsonObject->SetStringField(Api::Fields::GameMode, GameMode);
	JsonObject->SetStringField(Api::Fields::Host, Host);
	JsonObject->SetStringField(Api::Fields::Map, Map);
	JsonObject->SetBoolField(Api::Fields::Public, bPublic);
	JsonObject->SetNumberField(Api::Fields::Port, Port);
	JsonObject->SetNumberField(Api::Fields::MaxPlayers, MaxPlayers);
	JsonObject->SetStringField(Api::Fields::Status, Status);
	JsonObject->SetStringField(Api::Fields::Details, Details);
	return JsonObject;
}
