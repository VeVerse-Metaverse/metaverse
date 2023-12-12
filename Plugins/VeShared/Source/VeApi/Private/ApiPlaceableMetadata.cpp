// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "ApiPlaceableMetadata.h"

#include "GenericPlatform/GenericPlatformHttp.h"

typedef TSharedPtr<FJsonObject> FJsonObjectPtr;

FTransform FApiPlaceableMetadata::GetTransform() const {
	return FTransform(Rotation.Quaternion(), Position, Scale);
}

bool FApiPlaceableMetadata::FromJson(const TSharedPtr<FJsonObject> JsonObject) {
	const bool bOk = IApiMetadata::FromJson(JsonObject);

	Id = GetJsonGuidField(JsonObject, Api::Fields::Id);
	SlotId = GetJsonGuidField(JsonObject, Api::Fields::SlotId);
	EntityId = GetJsonGuidField(JsonObject, Api::Fields::EntityId);
	Type = GetJsonStringField(JsonObject, Api::Fields::Type);

	{
		const float Px = GetJsonFloatField(JsonObject, Api::Fields::OffsetX);
		const float Py = GetJsonFloatField(JsonObject, Api::Fields::OffsetY);
		const float Pz = GetJsonFloatField(JsonObject, Api::Fields::OffsetZ);
		Position.Set(Px, Py, Pz);
	}

	{
		const float Rx = GetJsonFloatField(JsonObject, Api::Fields::RotationX);
		const float Ry = GetJsonFloatField(JsonObject, Api::Fields::RotationY);
		const float Rz = GetJsonFloatField(JsonObject, Api::Fields::RotationZ);
		Rotation.SetComponentForAxis(EAxis::X, Rx);
		Rotation.SetComponentForAxis(EAxis::Y, Ry);
		Rotation.SetComponentForAxis(EAxis::Z, Rz);
	}

	{
		const float Sx = GetJsonFloatField(JsonObject, Api::Fields::ScaleX);
		const float Sy = GetJsonFloatField(JsonObject, Api::Fields::ScaleY);
		const float Sz = GetJsonFloatField(JsonObject, Api::Fields::ScaleZ);
		Scale.Set(Sx, Sy, Sz);
	}

	{
		const TSharedPtr<FJsonObject> EntityJsonObject = GetJsonObjectField(JsonObject, Api::Fields::Entity);
	}

	{
		const TSharedPtr<FJsonObject> OwnerJsonObject = GetJsonObjectField(JsonObject, Api::Fields::Owner);
		if (OwnerJsonObject.IsValid()) {
			Owner.FromJson(OwnerJsonObject);
		}
	}

	{
		const TSharedPtr<FJsonObject> PlaceableClassJsonObject = GetJsonObjectField(JsonObject, Api::Fields::PlaceableClass);
		if (PlaceableClassJsonObject.IsValid()) {
			PlaceableClassMetadata.FromJson(PlaceableClassJsonObject);
		}
	}

	{
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
	}

	{
		Properties = TArray<FApiPropertyMetadata>();
		const TArray<TSharedPtr<FJsonValue>> PropertyJsonObjectValues = GetJsonArrayField(JsonObject, Api::Fields::Properties);
		for (const TSharedPtr<FJsonValue>& JsonValue : PropertyJsonObjectValues) {
			const TSharedPtr<FJsonObject> PropertyJsonObject = JsonValue->AsObject();
			if (PropertyJsonObject.IsValid()) {
				FApiPropertyMetadata PropertyMetadata;
				PropertyMetadata.FromJson(PropertyJsonObject);
				Properties.Add(PropertyMetadata);
			}
		}
	}

	return bOk;
}

TSharedPtr<FJsonObject> FApiPlaceableMetadata::ToJson() {
	const FJsonObjectPtr JsonObject = IApiMetadata::ToJson();

	if (Id.IsValid()) {
		JsonObject->SetStringField(Api::Fields::Id, Id.ToString(EGuidFormats::DigitsWithHyphensLower));
	}

	if (SlotId.IsValid()) {
		JsonObject->SetStringField(Api::Fields::SlotId, SlotId.ToString(EGuidFormats::DigitsWithHyphensLower));
	}

	if (EntityId.IsValid()) {
		JsonObject->SetStringField(Api::Fields::EntityId, EntityId.ToString(EGuidFormats::DigitsWithHyphensLower));
	}

	JsonObject->SetStringField(Api::Fields::Type, Type);

	JsonObject->SetStringField(Api::Fields::PlaceableClassId, PlaceableClassId.ToString(EGuidFormats::DigitsWithHyphensLower));

	JsonObject->SetNumberField(Api::Fields::OffsetX, Position.X);
	JsonObject->SetNumberField(Api::Fields::OffsetY, Position.Y);
	JsonObject->SetNumberField(Api::Fields::OffsetZ, Position.Z);

	JsonObject->SetNumberField(Api::Fields::RotationX, Rotation.Roll);
	JsonObject->SetNumberField(Api::Fields::RotationY, Rotation.Pitch);
	JsonObject->SetNumberField(Api::Fields::RotationZ, Rotation.Yaw);

	JsonObject->SetNumberField(Api::Fields::ScaleX, Scale.X);
	JsonObject->SetNumberField(Api::Fields::ScaleY, Scale.Y);
	JsonObject->SetNumberField(Api::Fields::ScaleZ, Scale.Z);

	return JsonObject;
}

TSharedPtr<FJsonObject> FApiPlaceableClassBatchQueryRequestMetadata::ToJson() {
	TSharedPtr<FJsonObject> JsonObject = IApiBatchQueryRequestMetadata::ToJson();
	if (!Category.IsEmpty()) {
		JsonObject->SetStringField(Api::Fields::Type, Category);
	}
	return JsonObject;
}

FString FApiPlaceableClassBatchQueryRequestMetadata::ToUrlParams() {
	const FString Base = IApiBatchQueryRequestMetadata::ToUrlParams();
	return FString::Printf(TEXT("%s&%s=%s"), *Base, Api::Fields::Category, *FGenericPlatformHttp::UrlEncode(Category));
}

bool FApiPlaceableClassMetadata::FromJson(const TSharedPtr<FJsonObject> JsonObject) {
	const bool bOk = IApiMetadata::FromJson(JsonObject);

	Id = GetJsonGuidField(JsonObject, Api::Fields::Id);
	Name = GetJsonStringField(JsonObject, Api::Fields::Name);
	Description = GetJsonStringField(JsonObject, Api::Fields::Description);
	Class = GetJsonStringField(JsonObject, Api::Fields::Class);
	Category = GetJsonStringField(JsonObject, Api::Fields::Category);

	Files = TArray<FApiFileMetadata>();
	const auto FileJsonArray = GetJsonArrayField(JsonObject, Api::Fields::Files);
	for (const auto& FileJsonValuePtr : FileJsonArray) {
		if (const auto FileJsonPtr = FileJsonValuePtr->AsObject(); FileJsonPtr) {
			FApiFileMetadata File;
			File.FromJson(FileJsonPtr);
			Files.Add(File);
		}
	}

	return bOk;
}

TSharedPtr<FJsonObject> FApiPlaceableClassMetadata::ToJson() {
	// not implemented
	check(false);
	return IApiMetadata::ToJson();
}

TSharedPtr<FJsonObject> FApiUpdatePlaceableTransformMetadata::ToJson() {
	const FJsonObjectPtr JsonObject = IApiMetadata::ToJson();

	if (Id.IsValid()) {
		JsonObject->SetStringField(Api::Fields::Id, Id.ToString(EGuidFormats::DigitsWithHyphens));
	}

	JsonObject->SetNumberField(Api::Fields::OffsetX, Position.X);
	JsonObject->SetNumberField(Api::Fields::OffsetY, Position.Y);
	JsonObject->SetNumberField(Api::Fields::OffsetZ, Position.Z);

	JsonObject->SetNumberField(Api::Fields::RotationX, Rotation.Roll);
	JsonObject->SetNumberField(Api::Fields::RotationY, Rotation.Pitch);
	JsonObject->SetNumberField(Api::Fields::RotationZ, Rotation.Yaw);

	JsonObject->SetNumberField(Api::Fields::ScaleX, Scale.X);
	JsonObject->SetNumberField(Api::Fields::ScaleY, Scale.Y);
	JsonObject->SetNumberField(Api::Fields::ScaleZ, Scale.Z);

	return JsonObject;
}
