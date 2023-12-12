// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.


#include "ApiPersonaMetadata.h"

#include "ApiCommon.h"
#include "ApiUserMetadataObject.h"


FString FApiPersonaMetadata::GetDefaultAvatarUrl() const {
	return FString::Printf(TEXT("https://www.gravatar.com/avatar/%s?s=512&d=identicon&r=PG"), *FMD5::HashAnsiString(*Id.ToString()));
}

bool FApiPersonaMetadata::FromJson(const TSharedPtr<FJsonObject> JsonObject) {
	const bool bOk = FApiEntityMetadata::FromJson(JsonObject);
	Name = GetJsonStringField(JsonObject, Api::Fields::Name);
	Description = GetJsonStringField(JsonObject, Api::Fields::Description);
	Configuration = GetJsonStringField(JsonObject, Api::Fields::Configuration);
	Type = GetJsonStringField(JsonObject, Api::Fields::Type);

	Files = TArray<FApiFileMetadata>();
	const TArray<TSharedPtr<FJsonValue>> FileJsonObjectValues = GetJsonArrayField(JsonObject, Api::Fields::Files);
	for (const TSharedPtr<FJsonValue>& JsonValue : FileJsonObjectValues) {
		const TSharedPtr<FJsonObject> FileJsonObject = JsonValue->AsObject();
		if (FileJsonObject.IsValid()) {
			FApiFileMetadata FileMetadata;
			FileMetadata.FromJson(FileJsonObject);
			Files.Add(FileMetadata);
		}
	}

	return bOk;
}

TSharedPtr<FJsonObject> FApiCreatePersonaMetadata::ToJson() {
	const TSharedPtr<FJsonObject> JsonObject = IApiMetadata::ToJson();
	// if (Id.IsValid()) {
	// 	JsonObject->SetStringField(Api::Fields::Id, Id.ToString(EGuidFormats::DigitsWithHyphens));
	// }
	
	JsonObject->SetStringField(Api::Fields::Name, Name);
	JsonObject->SetStringField(Api::Fields::Type, Type);
	JsonObject->SetStringField(Api::Fields::Configuration, Configuration);
	
	return JsonObject;
}

TSharedPtr<FJsonObject> FApiDefaultPersonaMetadata::ToJson() {
	const TSharedPtr<FJsonObject> JsonObject = IApiMetadata::ToJson();
	
	if (PersonaId.IsValid()) {
		JsonObject->SetStringField(Api::Fields::PersonaId, PersonaId.ToString(EGuidFormats::DigitsWithHyphens));
	}
	
	return JsonObject;
}

TSharedPtr<FJsonObject> FApiUpdatePersonaMetadata::ToJson() {
	const TSharedPtr<FJsonObject> JsonObject = IApiMetadata::ToJson();
	if (Id.IsValid()) {
		JsonObject->SetStringField(Api::Fields::Id, Id.ToString(EGuidFormats::DigitsWithHyphens));
	}

	if (PersonaId.IsValid()) {
		JsonObject->SetStringField(Api::Fields::PersonaId, PersonaId.ToString(EGuidFormats::DigitsWithHyphens));
	}
	JsonObject->SetStringField(Api::Fields::Type, Type);
	JsonObject->SetStringField(Api::Fields::Name, Name);
	JsonObject->SetStringField(Api::Fields::Configuration, Configuration);
	
	return JsonObject;
}
