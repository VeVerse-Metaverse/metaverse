// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#include "ApiEntityMetadata.h"

#include "ApiCommon.h"
#include "GenericPlatform/GenericPlatformHttp.h"

FString EntityTypeToString(const EApiEntityType& Type) {
	switch (Type) {
	case User:
		return Api::EntityTypes::User;
	case Space:
		return Api::EntityTypes::Space;
	case Object:
		return Api::EntityTypes::Object;
	default:
		return Api::EntityTypes::Entity;
	}
}

bool IApiMetadata::FromJson(const TSharedPtr<FJsonObject> JsonObject) {
	if (!JsonObject) {
		return false;
	}
	return true;
}

bool IApiMetadata::FromJson(const TSharedPtr<FJsonValue> JsonValue) {
	if (!JsonValue) {
		return false;
	}
	return true;
}

bool IApiMetadata::FromJson(const TArray<TSharedPtr<FJsonValue>>& JsonArray) {
	return true;
}

TSharedPtr<FJsonObject> IApiMetadata::ToJson() {
	return MakeShareable(new FJsonObject());
}

TSharedPtr<FJsonObject> IApiMetadata::ToJson() const {
	return MakeShareable(new FJsonObject());
}

FString IApiMetadata::ToUrlParams() {
	return FString();
}

FString IApiMetadata::GetJsonStringField(const TSharedPtr<FJsonObject>& JsonObject, const FString& JsonField, const FString& DefaultValue) {
	if (JsonObject && JsonObject->HasTypedField<EJson::String>(JsonField)) {
		return JsonObject->GetStringField(JsonField);
	}
	return DefaultValue;
}

float IApiMetadata::GetJsonFloatField(const TSharedPtr<FJsonObject>& JsonObject, const FString& JsonField, const float DefaultValue) {
	if (JsonObject && JsonObject->HasTypedField<EJson::Number>(JsonField)) {
		return static_cast<float>(JsonObject->GetNumberField(JsonField));
	}
	return DefaultValue;
}

double IApiMetadata::GetJsonDoubleField(const TSharedPtr<FJsonObject>& JsonObject, const FString& JsonField, float DefaultValue) {
	if (JsonObject && JsonObject->HasTypedField<EJson::Number>(JsonField)) {
		return JsonObject->GetNumberField(JsonField);
	}
	return DefaultValue;
}

bool IApiMetadata::GetJsonBoolField(const TSharedPtr<FJsonObject>& JsonObject, const FString& JsonField, const bool DefaultValue) {
	if (JsonObject && JsonObject->HasTypedField<EJson::Boolean>(JsonField)) {
		return JsonObject->GetBoolField(JsonField);
	}
	return DefaultValue;
}

int32 IApiMetadata::GetJsonIntField(const TSharedPtr<FJsonObject>& JsonObject, const FString& JsonField, const int32 DefaultValue) {
	if (JsonObject && JsonObject->HasTypedField<EJson::Number>(JsonField)) {
		return static_cast<int32>(JsonObject->GetNumberField(JsonField));
	}
	return DefaultValue;
}

int64 IApiMetadata::GetJsonInt64Field(const TSharedPtr<FJsonObject>& JsonObject, const FString& JsonField, const int64 DefaultValue) {
	if (JsonObject && JsonObject->HasTypedField<EJson::Number>(JsonField)) {
		return static_cast<int64>(JsonObject->GetNumberField(JsonField));
	}
	return DefaultValue;
}

TArray<TSharedPtr<FJsonValue>> IApiMetadata::GetJsonArrayField(const TSharedPtr<FJsonObject>& JsonObject, const FString& JsonField) {
	if (JsonObject && JsonObject->HasTypedField<EJson::Array>(JsonField)) {
		return JsonObject->GetArrayField(JsonField);
	}
	return TArray<TSharedPtr<FJsonValue>>();
}

TSharedPtr<FJsonObject> IApiMetadata::GetJsonObjectField(const TSharedPtr<FJsonObject>& JsonObject, const FString& JsonField) {
	if (JsonObject && JsonObject->HasTypedField<EJson::Object>(JsonField)) {
		return JsonObject->GetObjectField(JsonField);
	}
	return nullptr;
}

FGuid IApiMetadata::GetJsonGuidField(const TSharedPtr<FJsonObject>& JsonObject, const FString& JsonField) {
	FGuid Id = FGuid();

	if (JsonObject && JsonObject->HasTypedField<EJson::String>(JsonField)) {
		if (!FGuid::Parse(JsonObject->GetStringField(JsonField), Id)) {
			Id = FGuid();
		}
	}

	return Id;
}

FDateTime IApiMetadata::GetJsonDateTimeField(const TSharedPtr<FJsonObject>& JsonObject, const FString& JsonField) {
	FDateTime DateTime = 0;

	if (JsonObject && JsonObject->HasTypedField<EJson::String>(JsonField)) {
		const auto CreatedAtStr = JsonObject->GetStringField(JsonField);

		int Index;
		FString IsoStr = CreatedAtStr;
		if (CreatedAtStr.FindLastChar('.', Index)) {
			IsoStr = CreatedAtStr.Left(Index);
		}

		if (!FDateTime::ParseIso8601(*IsoStr, DateTime)) {
			DateTime = 0;
		}
	}

	return DateTime;
}

bool IApiMetadata::SetJsonStringField(const TSharedPtr<FJsonObject> JsonObject, const FString& JsonField, const FString& Value) {
	JsonObject->SetStringField(JsonField, Value);
	return true;
}

bool IApiMetadata::SetJsonFloatField(const TSharedPtr<FJsonObject> JsonObject, const FString& JsonField, const float Value) {
	JsonObject->SetNumberField(JsonField, Value);
	return true;
}

bool IApiMetadata::SetJsonBoolField(const TSharedPtr<FJsonObject> JsonObject, const FString& JsonField, const bool Value) {
	JsonObject->SetBoolField(JsonField, Value);
	return true;
}

bool IApiMetadata::SetJsonIntField(const TSharedPtr<FJsonObject> JsonObject, const FString& JsonField, const int32 Value) {
	JsonObject->SetNumberField(JsonField, Value);
	return true;
}

bool IApiMetadata::SetJsonArrayField(const TSharedPtr<FJsonObject> JsonObject, const FString& JsonField, const TArray<TSharedPtr<FJsonValue>>& Value) {
	JsonObject->SetArrayField(JsonField, Value);
	return true;
}

bool IApiMetadata::SetJsonObjectField(const TSharedPtr<FJsonObject> JsonObject, const FString& JsonField, const TSharedPtr<FJsonObject>& Value) {
	JsonObject->SetObjectField(JsonField, Value);
	return true;
}

bool IApiMetadata::SetJsonGuidField(const TSharedPtr<FJsonObject> JsonObject, const FString& JsonField, const FGuid Value) {
	JsonObject->SetStringField(JsonField, Value.ToString(EGuidFormats::DigitsWithHyphens));
	return true;
}

bool IApiMetadata::SetJsonDateTimeField(const TSharedPtr<FJsonObject> JsonObject, const FString& JsonField, const FDateTime Value) {
	JsonObject->SetStringField(JsonField, Value.ToIso8601());
	return true;
}

bool FApiEntityMetadata::FromJson(const TSharedPtr<FJsonObject> JsonObject) {
	const bool bOk = IApiMetadata::FromJson(JsonObject);

	Id = GetJsonGuidField(JsonObject, Api::Fields::Id);
	bPublic = GetJsonBoolField(JsonObject, Api::Fields::Public);
	Views = GetJsonIntField(JsonObject, Api::Fields::Views);
	CreatedAt = GetJsonDateTimeField(JsonObject, Api::Fields::CreatedAt);
	UpdatedAt = GetJsonDateTimeField(JsonObject, Api::Fields::UpdatedAt);

	return bOk;
}

TSharedPtr<FJsonObject> FApiEntityMetadata::ToJson() {
	TSharedPtr<FJsonObject> JsonObject = IApiMetadata::ToJson();

	if (Id.IsValid()) {
		JsonObject->SetStringField(Api::Fields::Id, Id.ToString(EGuidFormats::DigitsWithHyphens));
	}

	JsonObject->SetBoolField(Api::Fields::Public, bPublic);
	JsonObject->SetNumberField(Api::Fields::Views, Views);

	JsonObject->SetStringField(Api::Fields::CreatedAt, CreatedAt.ToIso8601());
	JsonObject->SetStringField(Api::Fields::UpdatedAt, UpdatedAt.ToIso8601());

	return JsonObject;
}

TSharedPtr<FJsonObject> IApiBatchRequestMetadata::ToJson() {
	TSharedPtr<FJsonObject> JsonObject = IApiMetadata::ToJson();
	JsonObject->SetNumberField(Api::Fields::Offset, Offset);
	JsonObject->SetNumberField(Api::Fields::Limit, Limit);
	return JsonObject;
}

FString IApiBatchRequestMetadata::ToUrlParams() {
	return FString::Printf(TEXT("%s=%d&%s=%d"), Api::Fields::Offset, Offset, Api::Fields::Limit, Limit);
}

TSharedPtr<FJsonObject> IApiBatchQueryRequestMetadata::ToJson() {
	TSharedPtr<FJsonObject> JsonObject = IApiBatchRequestMetadata::ToJson();
	JsonObject->SetStringField(Api::Fields::Query, Query);
	return JsonObject;
}

FString IApiBatchQueryRequestMetadata::ToUrlParams() {
	const FString Base = IApiBatchRequestMetadata::ToUrlParams();
	return FString::Printf(TEXT("%s&%s=%s"), *Base, Api::Fields::Query, *FGenericPlatformHttp::UrlEncode(Query));
}

TSharedPtr<FJsonObject> IApiBatchQuerySortRequestMetadata::ToJson() {
	TSharedPtr<FJsonObject> JsonObject = IApiBatchQueryRequestMetadata::ToJson();
	JsonObject->SetNumberField(Api::Fields::Sort, Sort);
	return JsonObject;
}

// FString IApiBatchQuerySortRequestMetadata::ToUrlParams() {
// 	return FString::Printf(TEXT("%s=%d"), Api::Fields::Sort, Sort);
// }

FString IApiBatchQuerySortRequestMetadata::ToUrlParams() {
	const FString Base = IApiBatchQueryRequestMetadata::ToUrlParams();
	return FString::Printf(TEXT("%s&%s=%d"), *Base, Api::Fields::Sort, Sort);
}

TSharedPtr<FJsonObject> IApiFollowersRequestMetadata::ToJson() {
	TSharedPtr<FJsonObject> JsonObject = IApiBatchQueryRequestMetadata::ToJson();
	JsonObject->SetBoolField(Api::Fields::IncludeFriends, IncludeFriends);
	return JsonObject;
}

FString IApiFollowersRequestMetadata::ToUrlParams() {
	const FString Base = IApiBatchQueryRequestMetadata::ToUrlParams();
	return FString::Printf(TEXT("%s&%s=%s"), *Base, Api::Fields::IncludeFriends, (IncludeFriends ? TEXT("true") : TEXT("false")));
}
