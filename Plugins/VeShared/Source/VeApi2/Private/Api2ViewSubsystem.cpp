// Authors: Khusan Yadgarov, Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "Api2ViewSubsystem.h"
#include "VeApi2.h"
#include "GenericPlatform/GenericPlatformHttp.h"

FName FApi2ViewSubsystem::Name = TEXT("Api2ViewSubsystem");

bool FApi2UpdateViewRequest::Process() {
	const TSharedPtr<FJsonObject> JsonObject = RequestMetadata.ToJson();

	TArray<uint8> RequestBodyBinary;
	if (!SerializeJsonPayload(JsonObject, RequestBodyBinary)) {
		return false;
	}

	HttpRequest->SetVerb(Api::HttpMethods::Post);
	HttpRequest->SetURL(FString::Printf(TEXT("%s/%s/%s"), *Api::GetEntitiesUrl2(), *RequestMetadata.Id.ToString(EGuidFormats::DigitsWithHyphensLower), TEXT("views")));
	HttpRequest->SetContent(RequestBodyBinary);

	return FApi2Request::Process();
}

bool FApi2UpdateViewRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	if (CheckResponse()) {
		TSharedPtr<FJsonObject> JsonObject;
		if (GetResponseJsonObjectPayload(Response, JsonObject)) {
			ResponseMetadata.FromJson(JsonObject);
			return true;
		}
	}
	return false;
}

void FApi2ViewSubsystem::Initialize() {
}

void FApi2ViewSubsystem::Shutdown() {}

void FApi2ViewSubsystem::UpdateView(const FGuid& Id, TSharedRef<FOnViewRequestCompleted2> InCallback) const {
	const auto Request = MakeShared<FApi2UpdateViewRequest>();

	Request->RequestMetadata.Id = Id;


	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, Request->GetApiResponseCode(), InRequest->GetErrorString());
		// if (Request->GetApiResponseCode() == EApi2ResponseCode::Ok) {
		// 	BroadcastApiViewCreated(Request->ResponseMetadata);
		// }
	});

	if (!Request->Process()) {
		auto _ = InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}

