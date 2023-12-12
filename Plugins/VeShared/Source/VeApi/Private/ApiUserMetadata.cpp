// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.


#include "ApiUserMetadata.h"

#include "ApiCommon.h"
#include "ApiPersonaMetadata.h"
#include "ApiUserMetadataObject.h"
#include "VeShared.h"

#pragma region Create

TSharedPtr<FJsonObject> FApiRegisterUserMetadata::ToJson() {
	TSharedPtr<FJsonObject> JsonObject = IApiMetadata::ToJson();
	JsonObject->SetStringField(Api::Fields::Email, Email);
	JsonObject->SetStringField(Api::Fields::Name, Name);
	JsonObject->SetStringField(Api::Fields::Password, Password);
	JsonObject->SetStringField(Api::Fields::PasswordConfirmation, PasswordConfirmation);
	JsonObject->SetStringField(Api::Fields::InviteCode, InviteCode);
	return JsonObject;
}

#pragma endregion

#pragma region Update

TSharedPtr<FJsonObject> FApiUpdateUserMetadata::ToJson() {
	TSharedPtr<FJsonObject> JsonObject = IApiMetadata::ToJson();
	JsonObject->SetStringField(Api::Fields::Name, Name);
	JsonObject->SetStringField(Api::Fields::Description, Description);
	return JsonObject;
}

TSharedPtr<FJsonObject> FApiUpdateUserPasswordMetadata::ToJson() {
	TSharedPtr<FJsonObject> JsonObject = IApiMetadata::ToJson();
	JsonObject->SetStringField(Api::Fields::Password, Password);
	JsonObject->SetStringField(Api::Fields::NewPassword, NewPassword);
	JsonObject->SetStringField(Api::Fields::NewPasswordConfirmation, NewPasswordConfirmation);
	return JsonObject;
}

TSharedPtr<FJsonObject> FApiResetUserPasswordMetadata::ToJson() {
	TSharedPtr<FJsonObject> JsonObject = IApiMetadata::ToJson();
	JsonObject->SetStringField(Api::Fields::Email, Email);
	return JsonObject;
}

#pragma endregion

#pragma region User

float FApiUserMetadata::GetRating() const {
	return RATING(TotalLikes, TotalDislikes);
}

FString FApiUserMetadata::GetDefaultAvatarUrl() const {
	return FString::Printf(TEXT("https://www.gravatar.com/avatar/%s?s=512&d=identicon&r=PG"), *FMD5::HashAnsiString(*Id.ToString()));
}

EApiPresenceStatus FApiUserMetadata::GetStatus() const {
	return PresenceMetadata.Status;
}

FString FApiUserMetadata::GetRank() const {
	if (Rank.IsEmpty()) {
		return Rank;
	}
	return Rank.Mid(0, 1).ToUpper() + Rank.Mid(1);
}

float FApiUserMetadata::GetLevelExperience() const {
	// return 0.5;
	const float base = 10.0;
	const float exponent = 1.5;
	const float level = FMath::Pow(static_cast<float>(Experience) / base, 1.0 / exponent);
	return level - FMath::Floor(level);
}

bool FApiUserMetadata::FromJson(const TSharedPtr<FJsonObject> JsonObject) {
	const bool bOk = FApiEntityMetadata::FromJson(JsonObject);
	Name = GetJsonStringField(JsonObject, Api::Fields::Name);
	Email = GetJsonStringField(JsonObject, Api::Fields::Email);
	Title = GetJsonStringField(JsonObject, Api::Fields::Title);
	Description = GetJsonStringField(JsonObject, Api::Fields::Description);
	Geolocation = GetJsonStringField(JsonObject, Api::Fields::Geolocation);
	bAllowEmails = GetJsonBoolField(JsonObject, Api::Fields::AllowEmails);
	ApiKey = GetJsonStringField(JsonObject, Api::Fields::ApiKey);
	EthAddress = GetJsonStringField(JsonObject, Api::Fields::EthAddress);
	Level = GetJsonIntField(JsonObject, Api::Fields::Level);
	Rank = GetJsonStringField(JsonObject, Api::Fields::Rank);
	TotalLikes = GetJsonIntField(JsonObject, Api::Fields::Likes);
	TotalDislikes = GetJsonIntField(JsonObject, Api::Fields::Dislikes);
	Experience = GetJsonIntField(JsonObject, Api::Fields::Experience);
	bIsActive = GetJsonBoolField(JsonObject, Api::Fields::IsActive);
	bIsAdmin = GetJsonBoolField(JsonObject, Api::Fields::IsAdmin);
	bIsMuted = GetJsonBoolField(JsonObject, Api::Fields::IsMuted);
	bIsBanned = GetJsonBoolField(JsonObject, Api::Fields::IsBanned);

	const auto DefaultPersonaObject = GetJsonObjectField(JsonObject, Api::Fields::DefaultPersona);
	DefaultPersonaMetadata.FromJson(DefaultPersonaObject);

	const auto PresenceObject = GetJsonObjectField(JsonObject, Api::Fields::Presence);
	if (PresenceObject) {
		PresenceMetadata.FromJson(PresenceObject);
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

// bool FApiUserMetadataBatch::FromJson(const TSharedPtr<FJsonObject> JsonObject) {
// 	ENTITY_BATCH_FROM_JSON_BODY(IApiBatchMetadata, FApiUserMetadata);
// }
//
// TArray<UObject*> FApiUserMetadataBatch::ToUObjectArray(UObject* Parent) const {
// 	ENTITY_BATCH_TO_UOBJECT_ARRAY_BODY(FApiUserMetadata, UApiUserMetadataObject);
// }

TSharedPtr<FJsonObject> FApiUserLoginMetadata::ToJson() {
	TSharedPtr<FJsonObject> JsonObject = IApiMetadata::ToJson();
	JsonObject->SetStringField(Api::Fields::Email, Email);
	JsonObject->SetStringField(Api::Fields::Password, Password);
	return JsonObject;
}


#pragma endregion

#pragma region deprecated
// bool FApiUserRefBatch::FromJson(const TSharedPtr<FJsonObject> JsonObject) {
// 	ENTITY_BATCH_FROM_JSON(IApiMetadata, FApiUserRef);
// }
//
// TSharedPtr<FJsonObject> FApiUserRefBatch::ToJson() {
// 	ENTITY_BATCH_TO_JSON(IApiMetadata, FApiUserRef);
// }
//
// bool FApiUserBatch::FromJson(const TSharedPtr<FJsonObject> JsonObject) {
// 	ENTITY_BATCH_FROM_JSON(IApiMetadata, FApiUser);
// }
//
// TSharedPtr<FJsonObject> FApiUserBatch::ToJson() {
// 	ENTITY_BATCH_TO_JSON(IApiMetadata, FApiUserMetadata);
// }
//
// TSharedPtr<FJsonObject> FApiUserLoginMetadata::ToJson() {
// 	TSharedPtr<FJsonObject> JsonObject = IApiMetadata::ToJson();
// 	JsonObject->SetStringField(Api::Fields::Email, Email);
// 	JsonObject->SetStringField(Api::Fields::Password, Password);
// 	return JsonObject;
// }
//
// bool UApiUserLoginRequest::Process() {
// 	FString RequestBodyJsonString;
// 	const TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&RequestBodyJsonString);
// 	const TSharedPtr<FJsonObject> JsonObject = RequestData.ToJson();
//
// 	if (!JsonObject.IsValid()) {
// 		return false;
// 	}
//
// 	if (!FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter)) {
// 		return false;
// 	}
//
// 	TArray<uint8> RequestBodyBinary;
// 	const int32 JsonStringLength = RequestBodyJsonString.Len();
// 	RequestBodyBinary.SetNum(JsonStringLength);
// 	for (int32 Index = 0; Index < JsonStringLength; ++Index) {
// 		RequestBodyBinary[Index] = RequestBodyJsonString[Index];
// 	}
//
// 	HttpRequest->SetVerb(Api::HttpMethods::Post);
// 	HttpRequest->SetURL(Api::LoginUrl);
// 	HttpRequest->SetContent(RequestBodyBinary);
//
// 	return Super::Process();
// }
//
// bool UApiUserResetPasswordRequest::Process() {
// 	const TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
// 	JsonObject->SetStringField(Api::Fields::Email, RequestEmail);
//
// 	TArray<uint8> RequestBodyBinary;
// 	if (!SerializeJsonPayload(JsonObject, RequestBodyBinary)) {
// 		return false;
// 	}
//
// 	HttpRequest->SetVerb(Api::HttpMethods::Post);
// 	HttpRequest->SetURL(Api::ResetPasswordUrl);
// 	HttpRequest->SetContent(RequestBodyBinary);
//
// 	return Super::Process();
// }
//
// bool UApiUserResetPasswordRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
// 	OnApiRequestComplete.ExecuteIfBound(this, bSuccessful);
// 	Super::OnComplete(Request, Response, bSuccessful);
// }
//
// bool UApiUserLoginRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
// 	if (CheckResponse(Response)) {
// 		TSharedPtr<FJsonObject> JsonPayload;
// 		if (GetJsonObjectPayload(Response, JsonPayload)) {
// 			if (ResponseUser.FromJson(JsonPayload)) {
//
// 				OnApiRequestComplete.ExecuteIfBound(this, true);
// 				Super::OnComplete(Request, Response, bSuccessful);
//
// 				return;
// 			}
// 		}
// 	}
//
// 	OnApiRequestComplete.ExecuteIfBound(this, false);
// 	Super::OnComplete(Request, Response, bSuccessful);
// }
//
// bool UApiUserRegisterRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
// 	if (CheckResponse(Response)) {
// 		TSharedPtr<FJsonObject> JsonPayload;
// 		if (GetJsonObjectPayload(Response, JsonPayload)) {
// 			if (ResponseUser.FromJson(JsonPayload)) {
//
// 				OnApiRequestComplete.ExecuteIfBound(this, true);
// 				Super::OnComplete(Request, Response, bSuccessful);
//
// 				return;
// 			}
// 		}
// 	}
//
// 	OnApiRequestComplete.ExecuteIfBound(this, false);
// 	Super::OnComplete(Request, Response, bSuccessful);
// }
//
// bool UApiUserRegisterRequest::Process() {
// 	const TSharedPtr<FJsonObject> JsonObject = RequestData.ToJson();
//
// 	TArray<uint8> RequestBodyBinary;
// 	if (!SerializeJsonPayload(JsonObject, RequestBodyBinary)) {
// 		return false;
// 	}
//
// 	HttpRequest->SetVerb(Api::HttpMethods::Post);
// 	HttpRequest->SetURL(Api::UsersUrl);
// 	HttpRequest->SetContent(RequestBodyBinary);
//
// 	return Super::Process();
// }
//
// bool UApiUserGetMeRequest::Process() {
// 	HttpRequest->SetVerb(Api::HttpMethods::Get);
// 	HttpRequest->SetURL(Api::UserMeUrl);
// 	return Super::Process();
// }
//
// bool UApiUserGetMeRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
// 	if (!CheckResponse(Response)) {
// 		if (OnApiRequestComplete.IsBound()) {
// 			OnApiRequestComplete.Execute(this, false);
// 		}
// 		return;
// 	}
//
// 	const FString ResponseBodyJsonString = Response->GetContentAsString();
// 	const TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(ResponseBodyJsonString);
// 	TSharedPtr<FJsonObject> JsonObject;
// 	if (!FJsonSerializer::Deserialize(JsonReader, JsonObject) || !JsonObject.IsValid()) {
// 		if (OnApiRequestComplete.IsBound()) {
// 			OnApiRequestComplete.Execute(this, false);
// 		}
// 		return;
// 	}
//
// 	const TSharedPtr<FJsonObject> JsonPayload = JsonObject->GetObjectField(Api::Fields::Payload);
//
// 	if (JsonPayload.IsValid()) {
// 		ResponseUser.FromJson(JsonPayload);
// 	}
//
// 	if (OnApiRequestComplete.IsBound()) {
// 		OnApiRequestComplete.Execute(this, true);
// 	}
//
// 	Super::OnComplete(Request, Response, bSuccessful);
// }

#pragma endregion
