// Authors: Khusan Yadgarov, Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "Api2AppSubsystem.h"

FApi2AppIdentityImageRequestMetadata::FApi2AppIdentityImageRequestMetadata(const FGuid& InId) {
	Id = InId;
}

TSharedPtr<FJsonObject> FApi2AppIdentityImageRequestMetadata::ToJson() {
	const TSharedPtr<FJsonObject> JsonObject = IApiMetadata::ToJson();
	if (!Id.IsValid()) {
		JsonObject->SetStringField("id", Id.ToString(EGuidFormats::DigitsWithHyphensLower));
	}
	return IApiMetadata::ToJson();
}

FString FApi2AppIdentityImageRequestMetadata::ToUrlParams() {
	return FString::Printf(TEXT("id=%s"), *Id.ToString(EGuidFormats::DigitsWithHyphensLower));
}

bool FApi2AppIdentityImageResponseMetadata::FromJsonArray(const TArray<TSharedPtr<FJsonValue>> Json) {
	for (auto& Value : Json) {
		if (FApiFileMetadata File; File.FromJson(Value->AsObject())) {
			Files.Add(File);
		}
	}

	return true;
}

bool FApi2AppIdentityImageRequest::Process() {
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetURL(FString::Printf(TEXT("%s/apps/%s/images/identity"), *Api::GetApi2RootUrl(), *Request.Id.ToString(EGuidFormats::DigitsWithHyphensLower)));
	return FApi2Request::Process();
}

bool FApi2AppIdentityImageRequest::OnComplete(const FHttpRequestPtr& InRequest, const FHttpResponsePtr& InResponse, bool bSuccessful) {
	if (CheckResponse()) {
		if (TArray<TSharedPtr<FJsonValue>> Json; GetResponseJsonArrayPayload(InResponse, Json)) {
			Response.FromJsonArray(Json);
			return true;
		}
	}

	return false;
}

FName FApi2AppSubsystem::Name = TEXT("Api2AppSubsystem");

void FApi2AppSubsystem::Initialize() {}

void FApi2AppSubsystem::Shutdown() {}

void FApi2AppSubsystem::GetAppIdentityImages(const FGuid& Id, const TSharedRef<FOnApi2AppIdentityImageRequestCompleted> InCallback) const {
	const auto Request = MakeShared<FApi2AppIdentityImageRequest>();

	Request->Request = FApi2AppIdentityImageRequestMetadata(Id);

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		if (!bInSuccessful) {
			LogErrorF("failed to get app identity images: %s", *InRequest->GetErrorString());
		}
		InCallback->ExecuteIfBound(Request->Response, Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		// ReSharper disable once CppExpressionWithoutSideEffects
		InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}
