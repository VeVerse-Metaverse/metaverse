// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "ApiPropertyMetadata.h"


bool FApiPropertyMetadata::FromJson(const TSharedPtr<FJsonObject> JsonObject) {
	const bool bOk = IApiMetadata::FromJson(JsonObject);

	Type = GetJsonStringField(JsonObject, Api::Fields::Type);
	Name = GetJsonStringField(JsonObject, Api::Fields::Name);
	Value = GetJsonStringField(JsonObject, Api::Fields::Value);

	return bOk;
}

TArray<FApiPropertyMetadata> FApiPropertyMetadata::ToArray(const TArray<TSharedPtr<FApiPropertyMetadata>>& Entities) {
	TArray<FApiPropertyMetadata> Result;
	Result.Reserve(Entities.Num());

	for (const auto& ApiMetadata : Entities) {
		Result.Emplace(*ApiMetadata);
	}

	return Result;
}

FApiUpdatePropertyMetadata::FApiUpdatePropertyMetadata(const FApiPropertyMetadata& InMetadata)
	: Name(InMetadata.Name),
	  Type(InMetadata.Type),
	  Value(InMetadata.Value) {}

FApiUpdatePropertyMetadata::FApiUpdatePropertyMetadata(const FString& InName, const FString& InType, const FString& InValue)
	: Name(InName),
	  Type(InType),
	  Value(InValue) {}

TSharedPtr<FJsonObject> FApiUpdatePropertyMetadata::ToJson() {
	const auto JsonObject = IApiMetadata::ToJson();

	JsonObject->SetStringField(Api::Fields::Name, Name);
	JsonObject->SetStringField(Api::Fields::Type, Type);
	JsonObject->SetStringField(Api::Fields::Value, Value);

	return JsonObject;
}
