// Authors: Khusan Yadgarov, Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "Api2PackageSubsystem.h"
#include "VeApi2.h"
#include "Api2PackageMetadata.h"
#include "GenericPlatform/GenericPlatformHttp.h"

FName FApi2PackageSubsystem::Name = TEXT("Api2PackageSubsystem");

void FApi2PackageSubsystem::Initialize() {
	// Example:
	// TSharedPtr<FOnMetaverseBatchRequestCompleted> Callback = MakeShared<FOnMetaverseBatchRequestCompleted>();
	// Callback->BindLambda([=](const FApiMetaverseBatchMetadata& InMetadata, const bool bInSuccessful, const FString& Error) {
	// 	if (bInSuccessful) {
	// 		for (const auto EntityPtr : InMetadata.Entities) {
	// 			VeLogFunc("%s", *EntityPtr->Name);
	// 		}
	// 	}
	// });
	// if (Index({}, Callback)) {}
}

void FApi2PackageSubsystem::Shutdown() {}

FApiPackageIndexRequestMetadata::FApiPackageIndexRequestMetadata() {
	Platform = FVeApi2Module::GetPlatform();
	Deployment = FVeApi2Module::GetDeployment();
}

TSharedPtr<FJsonObject> FApiPackageIndexRequestMetadata::ToJson() {
	TSharedPtr<FJsonObject> JsonObject = IApiBatchQueryRequestMetadata::ToJson();
	if (!Platform.IsEmpty()) {
		JsonObject->SetStringField(TEXT("platform"), Platform);
	}
	return JsonObject;
}

FString FApiPackageIndexRequestMetadata::ToUrlParams() {
	const FString Base = IApiBatchQueryRequestMetadata::ToUrlParams();
	return FString::Printf(TEXT("%s&%s=%s&%s=%s"), *Base, TEXT("platform"), *FGenericPlatformHttp::UrlEncode(Platform),TEXT("deployment"), *FGenericPlatformHttp::UrlEncode(Deployment));
}

bool FApi2PackageBatchRequest::Process() {
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetURL(FString::Printf(TEXT("%s/%s?%s"), *Api::GetApi2RootUrl(), TEXT("metaverses"), *RequestMetadata.ToUrlParams()));

	return FApi2Request::Process();
}

bool FApi2PackageBatchRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	if (CheckResponse()) {
		TSharedPtr<FJsonObject> JsonObject;
		if (GetResponseJsonObjectPayload(Response, JsonObject)) {
			ResponseMetadata.FromJson(JsonObject);
			return true;
		}
	}
	return false;
}

bool FApi2CreatePackageRequest::Process() {
	const TSharedPtr<FJsonObject> JsonObject = RequestMetadata.ToJson();

	TArray<uint8> RequestBodyBinary;
	if (!SerializeJsonPayload(JsonObject, RequestBodyBinary)) {
		return false;
	}

	HttpRequest->SetVerb(Api::HttpMethods::Post);
	HttpRequest->SetURL(FString::Printf(TEXT("%s/%s"), *Api::GetApi2RootUrl(), TEXT("packages")));
	HttpRequest->SetContent(RequestBodyBinary);

	return FApi2Request::Process();
}

bool FApi2CreatePackageRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	if (CheckResponse()) {
		TSharedPtr<FJsonObject> JsonObject;
		if (GetResponseJsonObjectPayload(Response, JsonObject)) {
			ResponseMetadata.FromJson(JsonObject);
			return true;
		}
	}
	return false;
}

bool FApi2GetPackageRequest::Process() {
	HttpRequest->SetVerb(Api::HttpMethods::Get);
	HttpRequest->SetURL(FString::Printf(TEXT("%s/%s"), *Api::GetPackagesUrl2(), *RequestEntityId.ToString(EGuidFormats::DigitsWithHyphensLower)));

	return FApi2Request::Process();
}

bool FApi2GetPackageRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	if (CheckResponse()) {
		TSharedPtr<FJsonObject> JsonObject;
		if (GetResponseJsonObjectPayload(Response, JsonObject)) {
			ResponseMetadata.FromJson(JsonObject);
			return true;
		}
	}
	return false;
}

bool FApi2MapsPackageRequest::Process() {
	HttpRequest->SetVerb(Api::HttpMethods::Get);
	HttpRequest->SetURL(FString::Printf(TEXT("%s/%s/%s"), *Api::GetPackagesUrl2(), *RequestPackageId.ToString(EGuidFormats::DigitsWithHyphensLower), *Api::Maps));

	return FApi2Request::Process();
}

bool FApi2MapsPackageRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	if (CheckResponse()) {
		TArray<TSharedPtr<FJsonValue>> JsonPayload;
		if (GetResponseJsonArrayPayload(Response, JsonPayload)) {
			ResponseMetadata.Empty();
			ResponseMetadata.Reserve(JsonPayload.Num());
			for (auto& JsonValue : JsonPayload) {
				auto& MapMetadata = ResponseMetadata.Emplace_GetRef(MakeShared<FApi2MapMetadata>());
				MapMetadata->FromJson(JsonValue);
			}
			return true;
		}
	}
	return false;
}

bool FApi2UpdatePackageRequest::Process() {
	const TSharedPtr<FJsonObject> JsonObject = RequestMetadata.ToJson();

	TArray<uint8> RequestBodyBinary;
	if (!SerializeJsonPayload(JsonObject, RequestBodyBinary)) {
		return false;
	}

	HttpRequest->SetVerb(Api::HttpMethods::Patch);
	HttpRequest->SetURL(FString::Printf(TEXT("%s/%s/%s"), *Api::GetApi2RootUrl(), TEXT("packages"), *RequestMetadata.Id.ToString(EGuidFormats::DigitsWithHyphensLower)));
	HttpRequest->SetContent(RequestBodyBinary);

	return FApi2Request::Process();
}

bool FApi2UpdatePackageRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	if (CheckResponse()) {
		TSharedPtr<FJsonObject> JsonObject;
		if (GetResponseJsonObjectPayload(Response, JsonObject)) {
			ResponseMetadata.FromJson(JsonObject);
			return true;
		}
	}
	return false;
}

void FApi2PackageSubsystem::Index(const FApiPackageIndexRequestMetadata& InMetadata, TSharedRef<FOnPackageBatchRequestCompleted2> InCallback) const {
	const auto Request = MakeShared<FApi2PackageBatchRequest>();

	Request->RequestMetadata = InMetadata;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}

void FApi2PackageSubsystem::Get(const FGuid& InPackageId, TSharedRef<FOnPackageRequestCompleted2> InCallback) const {
	const auto Request = MakeShared<FApi2GetPackageRequest>();

	Request->RequestEntityId = InPackageId;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}

void FApi2PackageSubsystem::Create(const FApiUpdatePackageMetadata& InMetadata, TSharedRef<FOnPackageRequestCompleted2> InCallback) const {
	const auto Request = MakeShared<FApi2CreatePackageRequest>();

	Request->RequestMetadata = InMetadata;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, Request->GetApiResponseCode(), InRequest->GetErrorString());
		// if (Request->GetApiResponseCode() == EApi2ResponseCode::Ok) {
		// 	BroadcastApiMetaverseCreated(Request->ResponseMetadata);
		// }
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}

void FApi2PackageSubsystem::Update(const FApiUpdatePackageMetadata& InMetadata, TSharedRef<FOnPackageRequestCompleted2> InCallback) const {
	const auto Request = MakeShared<FApi2UpdatePackageRequest>();

	Request->RequestMetadata = InMetadata;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, Request->GetApiResponseCode(), InRequest->GetErrorString());
		// if (Request->GetApiResponseCode() == EApi2ResponseCode::Ok) {
		// 	BroadcastApiMetaverseCreated(Request->ResponseMetadata);
		// }
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}

void FApi2PackageSubsystem::Delete(const FGuid& InPackageId, TSharedRef<FOnGenericRequestCompleted2> InCallback) const {
	const auto Request = MakeShared<FApi2DeleteEntityRequest>();

	Request->Id = InPackageId;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		InCallback->ExecuteIfBound(Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound(EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}

void FApi2PackageSubsystem::GetMaps(const FGuid& InPackageId, TSharedRef<FOnMapBatchRequestCompleted2> InCallback) const {
	const auto Request = MakeShared<FApi2MapsPackageRequest>();

	Request->RequestPackageId = InPackageId;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}
