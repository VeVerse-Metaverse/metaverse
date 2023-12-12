// Authors: Khusan Yadgarov, Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "Api2WorldMetadata.h"


TSharedPtr<FJsonObject> FApi2BatchSortRequestMetadata::ToJson() {
	TSharedPtr<FJsonObject> JsonObject = IApiMetadata::ToJson();
	JsonObject->SetNumberField(Api::Fields::Offset, Offset);
	JsonObject->SetNumberField(Api::Fields::Limit, Limit);
	JsonObject->SetStringField(Api::Fields::Search, Search);

	TArray<TSharedPtr<FJsonValue>> JsonSortArray;
	for (const auto& Sort : SortOptions) {
		FString SortStr;
		if (Sort == EApiEntitySorts::Id) {
			SortStr = TEXT("id");
		} else if (Sort == EApiEntitySorts::Name) {
			SortStr = TEXT("name");
		} else if (Sort == EApiEntitySorts::Description) {
			SortStr = TEXT("description");
		} else if (Sort == EApiEntitySorts::Map) {
			SortStr = TEXT("map");
		} else if (Sort == EApiEntitySorts::PackageId) {
			SortStr = TEXT("packageId");
		} else if (Sort == EApiEntitySorts::Type) {
			SortStr = TEXT("type");
		} else if (Sort == EApiEntitySorts::Scheduled) {
			SortStr = TEXT("scheduled");
		} else if (Sort == EApiEntitySorts::Game_Mode) {
			SortStr = TEXT("game_mode");
		} else if (Sort == EApiEntitySorts::Views) {
			SortStr = TEXT("views");
		} else if (Sort == EApiEntitySorts::CreatedAt) {
			SortStr = TEXT("createdAt");
		} else if (Sort == EApiEntitySorts::UpdatedAt) {
			SortStr = TEXT("updatedAt");
		} else if (Sort == EApiEntitySorts::Public) {
			SortStr = TEXT("public");
		} else if (Sort == EApiEntitySorts::Likes) {
			SortStr = TEXT("likes");
		} else if (Sort == EApiEntitySorts::Dislikes) {
			SortStr = TEXT("dislikes");
		} else if (Sort == EApiEntitySorts::PakFile) {
			SortStr = TEXT("pakFile");
		} else if (Sort == EApiEntitySorts::PreviewFile) {
			SortStr = TEXT("previewFile");
		} else {
			continue;
		}
		FString DirectionStr;
		DirectionStr = TEXT("asc");
		TSharedPtr<FJsonObject> JsonSortObject = MakeShared<FJsonObject>();
		JsonSortObject->SetStringField(Api::Fields::Column, SortStr);
		JsonSortObject->SetStringField(Api::Fields::Direction, DirectionStr);

		JsonSortArray.Emplace(MakeShared<FJsonValueObject>(JsonSortObject));
	}
	JsonObject->SetArrayField(Api::Fields::Sort, JsonSortArray);

	TSharedPtr<FJsonObject> JsonOptionsObject = MakeShared<FJsonObject>();
	JsonOptionsObject->SetBoolField(Api::Fields::Pak, Pak);
	
	TSharedPtr<FJsonObject> JsonPakOptionsObject = MakeShared<FJsonObject>();
	JsonPakOptionsObject->SetStringField(Api::Fields::Platform, Platform);
	JsonPakOptionsObject->SetStringField(Api::Fields::Deployment, Deployment);
	JsonOptionsObject->SetObjectField(Api::Fields::PakOptions, JsonPakOptionsObject);
	
	JsonOptionsObject->SetBoolField(Api::Fields::Preview, Preview);
	JsonOptionsObject->SetBoolField(Api::Fields::Likes, Likes);
	JsonOptionsObject->SetBoolField(Api::Fields::Owner, Owner);
	JsonObject->SetObjectField(Api::Fields::Options, JsonOptionsObject);

	return JsonObject;
}
TSharedPtr<FJsonObject> FApi2GetRequestMetadata::ToJson() {
	TSharedPtr<FJsonObject> JsonObject = IApiMetadata::ToJson();
	
	TSharedPtr<FJsonObject> JsonOptionsObject = MakeShared<FJsonObject>();
	JsonOptionsObject->SetBoolField(Api::Fields::Pak, Pak);
	
	TSharedPtr<FJsonObject> JsonPakOptionsObject = MakeShared<FJsonObject>();
	JsonPakOptionsObject->SetStringField(Api::Fields::Platform, Platform);
	JsonPakOptionsObject->SetStringField(Api::Fields::Deployment, Deployment);
	JsonOptionsObject->SetObjectField(Api::Fields::PakOptions, JsonPakOptionsObject);
	
	JsonOptionsObject->SetBoolField(Api::Fields::Preview, Preview);
	JsonOptionsObject->SetBoolField(Api::Fields::Likes, Likes);
	JsonOptionsObject->SetBoolField(Api::Fields::Owner, Owner);
	JsonObject->SetObjectField(Api::Fields::Options, JsonOptionsObject);

	return JsonObject;
}
