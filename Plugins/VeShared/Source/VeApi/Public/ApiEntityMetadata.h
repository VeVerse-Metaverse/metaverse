// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "ApiCommon.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"

class FApiEntityMetadata;

enum EApiEntityType {
	Entity,
	User,
	Space,
	Object,
};

static FString EntityTypeToString(const EApiEntityType& Type);

/** Base abstract class for metadata descriptors. */
class VEAPI_API IApiMetadata {
public:
	virtual ~IApiMetadata() = default;

	/** Parses the metadata from the JSON object. */
	virtual bool FromJson(const TSharedPtr<FJsonObject> JsonObject);
	virtual bool FromJson(const TSharedPtr<FJsonValue> JsonValue);
	virtual bool FromJson(const TArray<TSharedPtr<FJsonValue>>& JsonArray);

	/** Converts the object to the JSON object to use in POST, PUT and PATCH requests. */
	virtual TSharedPtr<FJsonObject> ToJson();
	virtual TSharedPtr<FJsonObject> ToJson() const;

	/** Converts the object to the url parameters string to be used in GET requests. */
	virtual FString ToUrlParams();

	/** Gets the string field value from the JSON object. */
	static FString GetJsonStringField(const TSharedPtr<FJsonObject>& JsonObject, const FString& JsonField, const FString& DefaultValue = FString());

	/** Gets the float field value from the JSON object. */
	static float GetJsonFloatField(const TSharedPtr<FJsonObject>& JsonObject, const FString& JsonField, float DefaultValue = 0.f);

	/** Gets the double field value from the JSON object. */
	static double GetJsonDoubleField(const TSharedPtr<FJsonObject>& JsonObject, const FString& JsonField, float DefaultValue = 0.f);

	/** Gets the boolean field value from the JSON object. */
	static bool GetJsonBoolField(const TSharedPtr<FJsonObject>& JsonObject, const FString& JsonField, bool DefaultValue = false);

	/** Gets the integer field value from the Json object. */
	static int32 GetJsonIntField(const TSharedPtr<FJsonObject>& JsonObject, const FString& JsonField, int32 DefaultValue = 0);

	/** Gets the integer64 field value from the Json object. */
	static int64 GetJsonInt64Field(const TSharedPtr<FJsonObject>& JsonObject, const FString& JsonField, int64 DefaultValue = 0);

	/** Gets the JSON array field value from the JSON object. */
	static TArray<TSharedPtr<FJsonValue>> GetJsonArrayField(const TSharedPtr<FJsonObject>& JsonObject, const FString& JsonField);

	/** Gets the JSON object field value from the JSON object. */
	static TSharedPtr<FJsonObject> GetJsonObjectField(const TSharedPtr<FJsonObject>& JsonObject, const FString& JsonField);

	/** Gets the GUID field value from the JSON object. */
	static FGuid GetJsonGuidField(const TSharedPtr<FJsonObject>& JsonObject, const FString& JsonField);

	/** Gets the DateTime field value from the JSON object. */
	static FDateTime GetJsonDateTimeField(const TSharedPtr<FJsonObject>& JsonObject, const FString& JsonField);

	static bool SetJsonStringField(TSharedPtr<FJsonObject> JsonObject, const FString& JsonField, const FString& Value);
	static bool SetJsonFloatField(TSharedPtr<FJsonObject> JsonObject, const FString& JsonField, float Value);
	static bool SetJsonBoolField(TSharedPtr<FJsonObject> JsonObject, const FString& JsonField, bool Value);
	static bool SetJsonIntField(TSharedPtr<FJsonObject> JsonObject, const FString& JsonField, int32 Value);
	static bool SetJsonArrayField(TSharedPtr<FJsonObject> JsonObject, const FString& JsonField, const TArray<TSharedPtr<FJsonValue>>& Value);
	static bool SetJsonObjectField(TSharedPtr<FJsonObject> JsonObject, const FString& JsonField, const TSharedPtr<FJsonObject>& Value);
	static bool SetJsonGuidField(TSharedPtr<FJsonObject> JsonObject, const FString& JsonField, FGuid Value);
	static bool SetJsonDateTimeField(TSharedPtr<FJsonObject> JsonObject, const FString& JsonField, FDateTime Value);
};

/** Base interface for metadata batches. */
template <typename T = IApiMetadata/*, typename U = UObject*/>
class VEAPI_API TApiBatchMetadata : public IApiMetadata {
public:
	/** Batch query offset. */
	int32 Offset = 0;

	/** Batch query limit. */
	int32 Limit = 0;

	/** Batch total object number. */
	int32 Total = 0;

	/** Batch entities. todo: try to get rid of shared pointers? */
	TArray<TSharedPtr<T>> Entities;

	/** Parse from JSON. Template method. */
	virtual bool FromJson(const TSharedPtr<FJsonObject> JsonObject) override {
		bool bOk = IApiMetadata::FromJson(JsonObject);
		Offset = GetJsonIntField(JsonObject, Api::Fields::Offset);
		Limit = GetJsonIntField(JsonObject, Api::Fields::Limit);
		Total = GetJsonIntField(JsonObject, Api::Fields::Total);

		if (JsonObject->HasTypedField<EJson::Array>(Api::Fields::Entities)) {
			const TArray<TSharedPtr<FJsonValue>> JsonEntities = JsonObject->GetArrayField(Api::Fields::Entities);
			for (TArray<TSharedPtr<FJsonValue>>::TConstIterator It(JsonEntities); It; ++It) {
				TSharedPtr<FJsonObject> JsonEntity = (*It)->AsObject();
				if (JsonEntity.IsValid()) {
					TSharedPtr<T> Entity = MakeShared<T>();
					Entity->FromJson(JsonEntity);
					Entities.Add(Entity);
				} else { bOk &= false; }
			}
		} else {
			Entities = TArray<TSharedPtr<T>>();
		}
		return bOk;
	}

	/** Convert to JSON. Template method. */
	virtual TSharedPtr<FJsonObject> ToJson() override {
		TSharedPtr<FJsonObject> JsonObject = IApiMetadata::ToJson();
		JsonObject->SetNumberField(Api::Fields::Offset, Offset);
		JsonObject->SetNumberField(Api::Fields::Limit, Limit);
		JsonObject->SetNumberField(Api::Fields::Total, Total);

		TArray<TSharedPtr<FJsonValue>> JsonEntities;
		for (TSharedPtr<T> Entity : Entities) {
			const TSharedPtr<FJsonObject> JsonEntity = Entity->ToJson();
			TSharedPtr<FJsonValueObject> JsonValueObject = MakeShareable(new FJsonValueObject(JsonEntity));
			JsonEntities.Add(JsonValueObject);
		}

		JsonObject->SetArrayField(Api::Fields::Entities, JsonEntities);
		return JsonObject;
	}

	/** Convert to the array of metadata wrapper objects. Template method. Can not be linked using shared pointers :( */
	// TArray<UObject*> ToUObjectArray(UObject* Parent) const {
	// 	TArray<UObject*> Objects = TArray<UObject*>();
	// 	for (TSharedPtr<T> Entity : Entities) {
	// 		U* Object = NewObject<U>(Parent);
	// 		// Object->Metadata = Entity;
	// 		Objects.Add(Object);
	// 	}
	// 	return Objects;
	// }
};

/** Base interface for batched metadata requests. */
class VEAPI_API IApiBatchRequestMetadata : public IApiMetadata {
public:
	/** Request offset. */
	int32 Offset = 0;

	/** Request limit. */
	int32 Limit = 40;

	virtual TSharedPtr<FJsonObject> ToJson() override;
	virtual FString ToUrlParams() override;
};

/** Base interface for batched metadata requests with string query. */
class VEAPI_API IApiBatchQueryRequestMetadata : public IApiBatchRequestMetadata {
public:
	/** Request query. */
	FString Query = FString();

	virtual TSharedPtr<FJsonObject> ToJson() override;
	virtual FString ToUrlParams() override;
};

class VEAPI_API IApiBatchQuerySortRequestMetadata : public IApiBatchQueryRequestMetadata {
public:
	/** Request sort. */
	int32 Sort = -1;

	virtual TSharedPtr<FJsonObject> ToJson() override;
	virtual FString ToUrlParams() override;
};

class VEAPI_API IApiFollowersRequestMetadata : public IApiBatchQueryRequestMetadata {
public:
	bool IncludeFriends = true;

	virtual TSharedPtr<FJsonObject> ToJson() override;
	virtual FString ToUrlParams() override;
};

class VEAPI_API IApiUserBatchRequestMetadata final : public IApiBatchQuerySortRequestMetadata {
public:
	FGuid UserId{};
};

/** Entity metadata descriptor. */
class VEAPI_API FApiEntityMetadata : public IApiMetadata {
public:
	/** Unique id of the entity. */
	FGuid Id{};

	/** Time when the object has been created. */
	FDateTime CreatedAt{};

	/** Time when the object has been updated. */
	FDateTime UpdatedAt{};

	/** Numbers of views of the entity. */
	int32 Views = 0;

	/** Is the entity public or private. */
	bool bPublic = true;

	FORCEINLINE bool friend operator==(const FApiEntityMetadata& Lhs, const FApiEntityMetadata& Rhs) {
		return Lhs.Id == Rhs.Id;
	}

	FORCEINLINE bool friend operator!=(const FApiEntityMetadata& Lhs, const FApiEntityMetadata& Rhs) {
		return Lhs.Id != Rhs.Id;
	}

	virtual bool FromJson(const TSharedPtr<FJsonObject> JsonObject) override;
	virtual TSharedPtr<FJsonObject> ToJson() override;
};
