// Authors: Egor A. Pristavka, Khusan T. Yadgarov. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


#include "ApiTemplateMetadata.h"

#include "ApiCommon.h"
#include "ApiPackageMetadata.h"
#include "GenericPlatform/GenericPlatformHttp.h"

typedef TSharedPtr<FJsonValue> FJsonValuePtr;
typedef TSharedPtr<FJsonObject> FJsonObjectPtr;
typedef TArray<TSharedPtr<FJsonValue>> FJsonValuePtrArray;

bool FApiTemplateMetadata::FromJson(const TSharedPtr<FJsonObject> InJsonObject) {
	bool bOk = FApiEntityMetadata::FromJson(InJsonObject);
	TemplateId = GetJsonGuidField(InJsonObject, Api::Template::TemplatesId);
	Name = GetJsonStringField(InJsonObject, Api::Fields::Name);
	Title = GetJsonStringField(InJsonObject, Api::Fields::Title);
	Description = GetJsonStringField(InJsonObject, Api::Fields::Description);
	Summary = GetJsonStringField(InJsonObject, Api::Fields::Summary);
	Version = GetJsonStringField(InJsonObject, Api::Fields::Version);


	Owner = MakeShared<FApiUserMetadata>();
	const FJsonObjectPtr OwnerJsonObject = GetJsonObjectField(InJsonObject, Api::Fields::Owner);
	if (OwnerJsonObject.IsValid()) {
		bOk &= Owner->FromJson(OwnerJsonObject);
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


TSharedPtr<FJsonObject> FApiTemplateBatchQueryRequestMetadata::ToJson() {
	TSharedPtr<FJsonObject> JsonObject = IApiBatchQueryRequestMetadata::ToJson();
	return JsonObject;
}

FString FApiTemplateBatchQueryRequestMetadata::ToUrlParams() {
	return IApiBatchQueryRequestMetadata::ToUrlParams();
}


FApiUpdateTemplateMetadata::FApiUpdateTemplateMetadata(const FApiTemplateMetadata& InMetadata) {
	if (InMetadata.Id.IsValid()) {
		TemplateId = InMetadata.Id;
	} else {
		TemplateId = FGuid();
	}
	Name = InMetadata.Name;
	Description = InMetadata.Description;
	Summary = InMetadata.Summary;
	Version = InMetadata.Version;
	if (InMetadata.TemplateId.IsValid()) {
		TemplateId = InMetadata.TemplateId;
	} else if (InMetadata.TemplateId.IsValid()) {
		TemplateId = InMetadata.TemplateId;
	}
}

TSharedPtr<FJsonObject> FApiUpdateTemplateMetadata::ToJson() {
	const TSharedPtr<FJsonObject> JsonObject = IApiMetadata::ToJson();
	JsonObject->SetStringField(Api::Fields::Name, Name);
	JsonObject->SetStringField(Api::Fields::Description, Title);
	JsonObject->SetStringField(Api::Fields::Map, Summary);
	JsonObject->SetStringField(Api::Fields::GameMode, Description);
	JsonObject->SetStringField(Api::Fields::Type, Version);
	JsonObject->SetBoolField(Api::Fields::Public, bPublic);
	if (TemplateId.IsValid()) {
		JsonObject->SetStringField(Api::Fields::MetaverseId, TemplateId.ToString(EGuidFormats::DigitsWithHyphens));
	}
	return JsonObject;
}
