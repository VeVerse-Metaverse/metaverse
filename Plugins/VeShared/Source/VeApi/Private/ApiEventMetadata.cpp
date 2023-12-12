// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "ApiEventMetadata.h"

#include "ApiCommon.h"
#include "ApiEventMetadataObject.h"
#include "ApiSpaceMetadata.h"

typedef TSharedPtr<FJsonValue> FJsonValuePtr;
typedef TSharedPtr<FJsonObject> FJsonObjectPtr;
typedef TArray<TSharedPtr<FJsonValue>> FJsonValuePtrArray;

bool FApiEventMetadata::FromJson(const TSharedPtr<FJsonObject> InJsonObject) {
	bool bOk = FApiEntityMetadata::FromJson(InJsonObject);

	Name = GetJsonStringField(InJsonObject, Api::Fields::Name);
	Title = GetJsonStringField(InJsonObject, Api::Fields::Title);
	Summary = GetJsonStringField(InJsonObject, Api::Fields::Summary);
	Description = GetJsonStringField(InJsonObject, Api::Fields::Description);
	Active = GetJsonBoolField(InJsonObject, Api::Fields::Active);

	StartsAt = GetJsonDateTimeField(InJsonObject, Api::Fields::StartsAt);
	EndsAt = GetJsonDateTimeField(InJsonObject, Api::Fields::EndsAt);
	Type = GetJsonStringField(InJsonObject, Api::Fields::Type);
	
	Owner = FApiUserMetadata{};
	const FJsonObjectPtr OwnerJsonObject = GetJsonObjectField(InJsonObject, Api::Fields::Owner);
	if (OwnerJsonObject.IsValid()) {
		bOk &= Owner.FromJson(OwnerJsonObject);
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

	Space = FApiSpaceMetadata();
	const FJsonObjectPtr SpaceJsonObject = GetJsonObjectField(InJsonObject, Api::Fields::Owner);
	if (SpaceJsonObject.IsValid()) {
		bOk &= Space.FromJson(SpaceJsonObject);
	}

	return bOk;
}

bool FApiUpdateEventMetadata::operator==(const FApiUpdateEventMetadata& rhsMetadata) const {
	bool Result = rhsMetadata.Id == Id &&
	rhsMetadata.Title == Title &&
	rhsMetadata.Description == Description &&
	rhsMetadata.Summary == Summary;

	if (!Result) {
		return false;
	}

	return true;
}

bool FApiUpdateEventMetadata::operator!=(const FApiUpdateEventMetadata& rhsMetadata) const {
	return !(operator==(rhsMetadata));
}

FApiUpdateEventMetadata::FApiUpdateEventMetadata(const FApiEventMetadata& InMetadata) {
	if (InMetadata.Id.IsValid()) {
		Id = InMetadata.Id;
	} else {
		Id = FGuid();
	}
	
	Title = InMetadata.Title;
	Summary = InMetadata.Summary;
	Description = InMetadata.Description;
	bPublic = InMetadata.bPublic;
}

TSharedPtr<FJsonObject> FApiUpdateEventMetadata::ToJson() {
	const FJsonObjectPtr JsonObject = IApiMetadata::ToJson();
	JsonObject->SetStringField(Api::Fields::Title, Title);
	JsonObject->SetStringField(Api::Fields::Summary, Summary);
	JsonObject->SetStringField(Api::Fields::Description, Description);
	JsonObject->SetBoolField(Api::Fields::Public, bPublic);
	
	return JsonObject;
}
