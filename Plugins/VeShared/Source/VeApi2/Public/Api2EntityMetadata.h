// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


#pragma once

#include "ApiEntityMetadata.h"


/** Base interface for metadata batches. */
template <typename T = IApiMetadata/*, typename U = UObject*/>
class VEAPI2_API TApi2BatchMetadata : public IApiMetadata {
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
class VEAPI2_API IApi2BatchRequestMetadata : public IApiMetadata {
public:
	/** Request offset. */
	int32 Offset = 0;

	/** Request limit. */
	int32 Limit = 40;

	virtual TSharedPtr<FJsonObject> ToJson() override;
	virtual FString ToUrlParams() override;
};
