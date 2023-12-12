// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "ApiObjectMetadata.h"

bool FApiObjectMetadata::FromJson(const TSharedPtr<FJsonObject> JsonObject) {
	const bool bOk = FApiEntityMetadata::FromJson(JsonObject);

	Type = GetJsonStringField(JsonObject, Api::Fields::Type);
	Name = GetJsonStringField(JsonObject, Api::Fields::Name);
	Artist = GetJsonStringField(JsonObject, Api::Fields::Artist);
	Date = GetJsonStringField(JsonObject, Api::Fields::Date);
	Description = GetJsonStringField(JsonObject, Api::Fields::Description);
	Medium = GetJsonStringField(JsonObject, Api::Fields::Medium);
	Width = GetJsonFloatField(JsonObject, Api::Fields::Width);
	Height = GetJsonFloatField(JsonObject, Api::Fields::Height);
	ScaleMultiplier = GetJsonFloatField(JsonObject, Api::Fields::ScaleMultiplier);
	Source = GetJsonStringField(JsonObject, Api::Fields::Source);
	SourceUrl = GetJsonStringField(JsonObject, Api::Fields::SourceUrl);
	License = GetJsonStringField(JsonObject, Api::Fields::License);
	Copyright = GetJsonStringField(JsonObject, Api::Fields::Copyright);
	Credit = GetJsonStringField(JsonObject, Api::Fields::Credit);
	Origin = GetJsonStringField(JsonObject, Api::Fields::Origin);
	Location = GetJsonStringField(JsonObject, Api::Fields::Location);
	Dimensions = GetJsonStringField(JsonObject, Api::Fields::Dimensions);
	TotalLikes = GetJsonIntField(JsonObject, Api::Fields::TotalLikes);
	TotalDislikes = GetJsonIntField(JsonObject, Api::Fields::TotalDislikes);
	Liked = GetJsonIntField(JsonObject, Api::Fields::Liked);
	bPublic = GetJsonBoolField(JsonObject, Api::Fields::Public);

	const TSharedPtr<FJsonObject> OwnerJsonObject = GetJsonObjectField(JsonObject, Api::Fields::Owner);
	if (OwnerJsonObject.IsValid()) {
		Owner.FromJson(OwnerJsonObject);
	}

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

FApiUpdateObjectMetadata::FApiUpdateObjectMetadata(const FApiObjectMetadata& InMetadata) {
	if (InMetadata.Id.IsValid()) {
		Id = InMetadata.Id;
	} else {
		Id = FGuid();
	}

	bPublic = InMetadata.bPublic;

	Name = InMetadata.Name;
	Description = InMetadata.Description;
	Artist = InMetadata.Artist;
	Height = InMetadata.Height;
	Width = InMetadata.Width;
	ScaleMultiplier = InMetadata.ScaleMultiplier;
	Type = InMetadata.Type;

	Date = InMetadata.Date;
	Dimensions = InMetadata.Dimensions;
	Medium = InMetadata.Medium;
	Source = InMetadata.Source;
	SourceUrl = InMetadata.SourceUrl;
	License = InMetadata.License;
	Copyright = InMetadata.Copyright;
	Credit = InMetadata.Credit;
	Origin = InMetadata.Origin;
	Location = InMetadata.Location;

	if (!ScaleMultiplier) {
		ScaleMultiplier = 1.0f;
	}
}

TSharedPtr<FJsonObject> FApiUpdateObjectMetadata::ToJson() {
	const TSharedPtr<FJsonObject> JsonObject = IApiMetadata::ToJson();
	JsonObject->SetStringField(Api::Fields::Name, Name);
	JsonObject->SetStringField(Api::Fields::Description, Description);
	JsonObject->SetStringField(Api::Fields::Type, Type);
	JsonObject->SetBoolField(Api::Fields::Public, bPublic);
	JsonObject->SetStringField(Api::Fields::Artist, Artist);
	JsonObject->SetStringField(Api::Fields::Medium, Medium);
	JsonObject->SetNumberField(Api::Fields::Width, Width);
	JsonObject->SetNumberField(Api::Fields::Height, Height);
	JsonObject->SetNumberField(Api::Fields::ScaleMultiplier, ScaleMultiplier);
	JsonObject->SetStringField(Api::Fields::Date, Date);
	JsonObject->SetStringField(Api::Fields::Dimensions, Dimensions);
	JsonObject->SetStringField(Api::Fields::Source, Source);
	JsonObject->SetStringField(Api::Fields::SourceUrl, SourceUrl);
	JsonObject->SetStringField(Api::Fields::License, License);
	JsonObject->SetStringField(Api::Fields::Copyright, Copyright);
	JsonObject->SetStringField(Api::Fields::Credit, Credit);
	JsonObject->SetStringField(Api::Fields::Origin, Origin);
	JsonObject->SetStringField(Api::Fields::Location, Location);
	return JsonObject;
}
