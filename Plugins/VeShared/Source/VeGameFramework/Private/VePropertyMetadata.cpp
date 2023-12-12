// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "VePropertyMetadata.h"

#include "Serialization/BufferArchive.h"


FVePropertyMetadata::FVePropertyMetadata(const FApiPropertyMetadata& ApiMetadata)
	: Name(ApiMetadata.Name),
	  Type(StringToType(ApiMetadata.Type)),
	  Value(ApiMetadata.Value) { }

FApiPropertyMetadata FVePropertyMetadata::ToApiMetadata() const {
	FApiPropertyMetadata ApiMetadata;

	ApiMetadata.Name = Name;
	ApiMetadata.Type = TypeToString(Type);
	ApiMetadata.Value = Value;

	return ApiMetadata;
}

EVePropertyType FVePropertyMetadata::StringToType(const FString Type) {
	if (Type == TEXT("string")) {
		return EVePropertyType::String;
	}
	return EVePropertyType::None;
}

FString FVePropertyMetadata::TypeToString(EVePropertyType Type) {
	switch (Type) {
	case EVePropertyType::String:
		return TEXT("string");
	default:
		return TEXT("");
	}
}

TArray<FVePropertyMetadata> FVePropertyMetadata::ToArray(const TArray<FApiPropertyMetadata>& ApiProperties) {
	TArray<FVePropertyMetadata> Result;
	Result.Reserve(ApiProperties.Num());

	for (const auto& ApiMetadata : ApiProperties) {
		Result.Emplace(ApiMetadata);
	}

	return Result;
}

void FVePropertyBatchMetadata::Deserialize(const TArray<uint8>& Bytes) {
	FMemoryReader ArReader(Bytes);
	StaticStruct()->SerializeBin(ArReader, this);
}

TArray<uint8> FVePropertyBatchMetadata::Serialize() {
	FBufferArchive Buffer{};
	StaticStruct()->SerializeBin(Buffer, this);
	auto Bytes = static_cast<TArray<uint8>>(Buffer);
	return Bytes;

}
