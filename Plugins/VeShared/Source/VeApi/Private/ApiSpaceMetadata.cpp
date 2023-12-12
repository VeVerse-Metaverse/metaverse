// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "ApiSpaceMetadata.h"

#include "ApiCommon.h"
#include "ApiPackageMetadata.h"
#include "GenericPlatform/GenericPlatformHttp.h"

typedef TSharedPtr<FJsonValue> FJsonValuePtr;
typedef TSharedPtr<FJsonObject> FJsonObjectPtr;
typedef TArray<TSharedPtr<FJsonValue>> FJsonValuePtrArray;

bool FApiSpaceMetadata::FromJson(const TSharedPtr<FJsonObject> InJsonObject) {
	const bool bOk = FApiEntityMetadata::FromJson(InJsonObject);
	Name = GetJsonStringField(InJsonObject, Api::Fields::Name);
	Description = GetJsonStringField(InJsonObject, Api::Fields::Description);
	Map = GetJsonStringField(InJsonObject, Api::Fields::Map);
	GameMode = GetJsonStringField(InJsonObject, Api::Fields::GameMode);
	TotalLikes = GetJsonIntField(InJsonObject, Api::Fields::TotalLikes);
	TotalDislikes = GetJsonIntField(InJsonObject, Api::Fields::TotalDislikes);
	Likes = GetJsonIntField(InJsonObject, Api::Fields::Likes);
	Dislikes = GetJsonIntField(InJsonObject, Api::Fields::Dislikes);
	Liked = GetJsonIntField(InJsonObject, Api::Fields::Liked);
	bPublic = GetJsonBoolField(InJsonObject, Api::Fields::Public);
	
	TSharedPtr<FJsonObject> ModJsonObject = GetJsonObjectField(InJsonObject, Api::Fields::Metaverse);
	if (!ModJsonObject.IsValid()) {
		ModJsonObject = GetJsonObjectField(InJsonObject, Api::Fields::Mod);
	}
	if (ModJsonObject.IsValid()) {
		Package->FromJson(ModJsonObject);
	}

	if (const TSharedPtr<FJsonObject> OwnerJsonObject = GetJsonObjectField(InJsonObject, Api::Fields::Owner)) {
		Owner.FromJson(OwnerJsonObject);
	}

	Files = TArray<FApiFileMetadata>();
	if (InJsonObject->HasTypedField<EJson::Array>(Api::Fields::Files)) {
		const FJsonValuePtrArray FileJsonObjectValues = GetJsonArrayField(InJsonObject, Api::Fields::Files);
		for (const FJsonValuePtr& JsonValue : FileJsonObjectValues) {
			if (const auto FileJsonObject = JsonValue->AsObject(); FileJsonObject.IsValid()) {
				FApiFileMetadata FileMetadata = FApiFileMetadata();
				FileMetadata.FromJson(FileJsonObject);
				Files.Add(FileMetadata);
			}
		}
	} else if (InJsonObject->HasTypedField<EJson::Object>(Api::Fields::Files)) {
		if (const auto FileJsonObject = GetJsonObjectField(InJsonObject, Api::Fields::Files); FileJsonObject->HasTypedField<EJson::Array>(Api::Fields::Entities)) {
			FApiFileBatchMetadata FileBatchMetadata = FApiFileBatchMetadata();
			FileBatchMetadata.FromJson(FileJsonObject);
			for (auto FileBatchItem : FileBatchMetadata.Entities) {
				if (FileBatchItem) {
					Files.Add(*FileBatchItem.Get());
				}
			}
		}
	}

	return bOk;
}

TSharedPtr<FJsonObject> FApiSpaceBatchQueryRequestMetadata::ToJson() {
	TSharedPtr<FJsonObject> JsonObject = IApiBatchQueryRequestMetadata::ToJson();
	if (!Type.IsEmpty()) {
		JsonObject->SetStringField(Api::Fields::Type, Type);
	}
	return JsonObject;
}

FString FApiSpaceBatchQueryRequestMetadata::ToUrlParams() {
	const FString Base = IApiBatchQueryRequestMetadata::ToUrlParams();
	return FString::Printf(TEXT("%s&%s=%s"), *Base, Api::Fields::Type, *FGenericPlatformHttp::UrlEncode(Type));
}

// bool FApiSpaceBatchMetadata::FromJson(const TSharedPtr<FJsonObject> JsonObject) {
// 	ENTITY_BATCH_FROM_JSON_BODY(IApiBatchMetadata, FApiSpaceMetadata);
// }
//
// TArray<UObject*> FApiSpaceBatchMetadata::ToUObjectArray(UObject* Parent) const {
// 	ENTITY_BATCH_TO_UOBJECT_ARRAY_BODY(FApiSpaceMetadata, UApiSpaceMetadataObject);
// }

FApiUpdateSpaceMetadata::FApiUpdateSpaceMetadata(const FApiSpaceMetadata& InMetadata) {
	if (InMetadata.Id.IsValid()) {
		Id = InMetadata.Id;
	} else {
		Id = FGuid();
	}
	Name = InMetadata.Name;
	Description = InMetadata.Description;
	Map = InMetadata.Map;
	GameMode = InMetadata.GameMode;
	PackageId = InMetadata.Package->Id;
}

TSharedPtr<FJsonObject> FApiUpdateSpaceMetadata::ToJson() {
	const TSharedPtr<FJsonObject> JsonObject = IApiMetadata::ToJson();
	JsonObject->SetStringField(Api::Fields::Name, Name);
	JsonObject->SetStringField(Api::Fields::Description, Description);
	JsonObject->SetStringField(Api::Fields::Map, Map);
	JsonObject->SetStringField(Api::Fields::GameMode, GameMode);
	JsonObject->SetBoolField(Api::Fields::Public, bPublic);
	JsonObject->SetStringField(Api::Fields::MetaverseId, PackageId.ToString(EGuidFormats::DigitsWithHyphensLower));
	return JsonObject;
}
