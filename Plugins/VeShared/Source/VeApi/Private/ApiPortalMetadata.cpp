// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "ApiPortalMetadata.h"

#include "ApiPackageMetadata.h"
#include "ApiPortalMetadataObject.h"
#include "ApiSpaceMetadata.h"
#include "ApiUserMetadata.h"

typedef TSharedPtr<FJsonValue> FJsonValuePtr;
typedef TSharedPtr<FJsonObject> FJsonObjectPtr;
typedef TArray<TSharedPtr<FJsonValue>> FJsonValuePtrArray;

bool FApiPortalDestinationMetadata::FromJson(const TSharedPtr<FJsonObject> InJsonObject) {
	bool bOk = FApiEntityMetadata::FromJson(InJsonObject);

	Name = GetJsonStringField(InJsonObject, Api::Fields::Name);
	const FJsonObjectPtr SpaceJsonObject = GetJsonObjectField(InJsonObject, Api::Fields::Space);
	if (SpaceJsonObject.IsValid()) {
		bOk &= Space.FromJson(SpaceJsonObject);
	}

	Files = TArray<FApiFileMetadata>();
	const FJsonValuePtrArray FileJsonObjectValues = GetJsonArrayField(InJsonObject, Api::Fields::Files);
	for (const FJsonValuePtr& JsonValue : FileJsonObjectValues) {
		const FJsonObjectPtr FileJsonObject = JsonValue->AsObject();
		if (FileJsonObject.IsValid()) {
			FApiFileMetadata FileMetadata = FApiFileMetadata();
			FileMetadata.FromJson(FileJsonObject);
			Files.Add(FileMetadata);
		}
	}

	return bOk;
}

bool FApiPortalMetadata::FromJson(const TSharedPtr<FJsonObject> InJsonObject) {
	bool bOk = FApiEntityMetadata::FromJson(InJsonObject);

	Name = GetJsonStringField(InJsonObject, Api::Fields::Name);

	const FJsonObjectPtr DestinationJsonObject = GetJsonObjectField(InJsonObject, Api::Fields::Destination);
	if (DestinationJsonObject.IsValid()) {
		bOk &= Destination.FromJson(DestinationJsonObject);
	}

	return bOk;
}


FApiUpdatePortalMetadata::FApiUpdatePortalMetadata(const FApiPortalMetadata& InMetadata) {
	Id = InMetadata.Id;
	Name = InMetadata.Name;
	DestinationId = InMetadata.Destination.Id;
	WorldId = InMetadata.Space.Id;
	bPublic = InMetadata.bPublic;
}

TSharedPtr<FJsonObject> FApiUpdatePortalMetadata::ToJson() {
	const FJsonObjectPtr JsonObject = IApiMetadata::ToJson();

	if (Id.IsValid()) {
		JsonObject->SetStringField(Api::Fields::Id, Id.ToString(EGuidFormats::DigitsWithHyphens));
	}

	if (DestinationId.IsValid()) {
		JsonObject->SetStringField(Api::Fields::DestinationId, DestinationId.ToString(EGuidFormats::DigitsWithHyphensLower));
	}

	if (WorldId.IsValid()) {
		JsonObject->SetStringField(Api::Fields::SpaceId, WorldId.ToString(EGuidFormats::DigitsWithHyphensLower));
	}

	JsonObject->SetStringField(Api::Fields::Name, Name);
	JsonObject->SetBoolField(Api::Fields::Public, bPublic);

	return JsonObject;
}
