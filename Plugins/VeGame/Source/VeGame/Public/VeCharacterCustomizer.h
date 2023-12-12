// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once
#include "ApiEntityMetadata.h"
#include "VeShared.h"

struct FVeCharacterCustomizerProperties : IApiMetadata {
	int32 IntField;
	FString StringField;
	virtual TSharedPtr<FJsonObject> ToJson() override;
	bool ToJsonString(FString& OutString);
};

inline TSharedPtr<FJsonObject> FVeCharacterCustomizerProperties::ToJson() {
	auto JsonObject = IApiMetadata::ToJson();
	JsonObject->SetStringField("stringField", StringField);
	JsonObject->SetStringField("intField", FString::Printf(TEXT("%d"), IntField));
	return JsonObject;
}

inline bool FVeCharacterCustomizerProperties::ToJsonString(FString& OutString) {
	auto JsonObject = ToJson();

	TArray<uint8> RequestBodyBinary;
	FString RequestBodyJsonString;
	const TCondensedJsonWriterRef JsonWriter = TCondensedJsonWriterFactory::Create(&RequestBodyJsonString);

	if (!JsonObject.IsValid()) {
		return false;
	}

	if (!FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter)) {
		return false;
	}

	// const FTCHARToUTF8 StringConverter = FTCHARToUTF8(*RequestBodyJsonString);
	// const char* Utf8RequestBodyJsonString = const_cast<ANSICHAR*>(StringConverter.Get());
	// const int32 Utf8RequestBodyJsonStringLen = FCStringAnsi::Strlen(Utf8RequestBodyJsonString);

	// OutBytes.SetNumUninitialized(Utf8RequestBodyJsonStringLen);
	// for (int32 I = 0; I < Utf8RequestBodyJsonStringLen; I++) {
	// 	OutBytes[I] = Utf8RequestBodyJsonString[I];
	// }

	OutString = RequestBodyJsonString;

	return true;

}
