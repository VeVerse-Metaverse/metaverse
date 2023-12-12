// Authors: Khusan Yadgarov, Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "Api2StatsSubsystem.h"
#include "VeApi2.h"

FName FApi2StatsSubsystem::Name = TEXT("Api2StatsSubsystem");

void FApi2StatsSubsystem::Initialize() {}

void FApi2StatsSubsystem::Shutdown() {}

bool FApi2StatsRequest::Process() {
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetURL(FString::Printf(TEXT("%s/%s"), *Api::GetApi2RootUrl(), TEXT("stats")));

	return FApi2Request::Process();
}

bool FApi2StatsRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	if (CheckResponse()) {
		TSharedPtr<FJsonObject> JsonObject;
		if (GetResponseJsonObjectPayload(Response, JsonObject)) {
			ResponseMetadata.FromJson(JsonObject);
			return true;
		}
	}
	return false;
}

void FApi2StatsSubsystem::Get(const TSharedRef<FOnStatsRequestCompleted> InCallback) const {
	const auto Request = MakeShared<FApi2StatsRequest>();

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		auto _ = InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}
