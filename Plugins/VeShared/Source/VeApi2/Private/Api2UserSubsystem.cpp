// Authors: Khusan Yadgarov, Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "Api2UserSubsystem.h"
#include "VeApi2.h"

FName FApi2UserSubsystem::Name = TEXT("Api2UserSubsystem");

bool FApi2GetMeRequest::Process() {
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetURL(Api::GetApi2RootUrl() / TEXT("users/me"));

	return FApi2Request::Process();
}

bool FApi2GetMeRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	if (CheckResponse()) {
		TSharedPtr<FJsonObject> JsonObject;
		if (GetResponseJsonObjectPayload(Response, JsonObject)) {
			ResponseMetadata.FromJson(JsonObject);
			return true;
		}
	}

	return false;
}

bool FApi2GetUserByEthAddressRequest::Process() {
	HttpRequest->SetVerb(Api::HttpMethods::Get);
	HttpRequest->SetURL(FString::Printf(TEXT("%s/address/%s"), *Api::GetUsersUrl2(), *RequestEthAddress));

	return FApi2Request::Process();
}

bool FApi2GetUserByEthAddressRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	if (CheckResponse()) {
		TSharedPtr<FJsonObject> JsonObject;
		if (GetResponseJsonObjectPayload(Response, JsonObject)) {
			ResponseMetadata.FromJson(JsonObject);
			return true;
		}
	}

	return false;
}

bool FApi2GetUserRequest::Process() {
	HttpRequest->SetVerb(Api::HttpMethods::Get);
	HttpRequest->SetURL(FString::Printf(TEXT("%s/%s"), *Api::GetUsersUrl2(), *RequestEntityId.ToString(EGuidFormats::DigitsWithHyphens)));

	return FApi2Request::Process();
}

bool FApi2GetUserRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	if (CheckResponse()) {
		TSharedPtr<FJsonObject> JsonObject;
		if (GetResponseJsonObjectPayload(Response, JsonObject)) {
			ResponseMetadata.FromJson(JsonObject);
			return true;
		}
	}

	return false;
}

bool FApi2UserFollowRequest::Process() {
	HttpRequest->SetVerb(Api::HttpMethods::Put);
	HttpRequest->SetURL(FString::Printf(TEXT("%s/%s/%s"), *Api::GetUsersUrl2(), *RequestUserId.ToString(EGuidFormats::DigitsWithHyphens), TEXT("follow")));

	return FApi2Request::Process();
}

bool FApi2UserFollowRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	bool bOk = false;

	if (CheckResponse()) {
		bOk = true;
	}

	return bOk;
}

bool FApi2UserUnfollowRequest::Process() {
	HttpRequest->SetVerb(Api::HttpMethods::Delete);
	HttpRequest->SetURL(FString::Printf(TEXT("%s/%s/%s"), *Api::GetUsersUrl2(), *RequestUserId.ToString(EGuidFormats::DigitsWithHyphens), TEXT("follow")));

	return FApi2Request::Process();
}

bool FApi2UserUnfollowRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	bool bOk = false;

	if (CheckResponse()) {
		bOk = true;
	}

	return bOk;
}

bool FApi2UserIsFollowingRequest::Process() {
	HttpRequest->SetVerb(Api::HttpMethods::Get);
	HttpRequest->SetURL(FString::Printf(TEXT("%s/%s/%s/%s"), *Api::GetUsersUrl(), *RequestFollowerId.ToString(EGuidFormats::DigitsWithHyphens), TEXT("follows"),
		*RequestLeaderId.ToString(EGuidFormats::DigitsWithHyphens)));

	return FApi2Request::Process();
}

bool FApi2UserIsFollowingRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	if (CheckResponse()) {
		TSharedPtr<FJsonObject> JsonObject;
		if (GetResponseJsonObjectPayload(Response, JsonObject)) {
			ResponseMetadata.FromJson(JsonObject);
			return true;
		}
	}

	return false;
}

bool FApi2ResetUserPasswordRequest::Process() {
	const TSharedPtr<FJsonObject> JsonObject = RequestMetadata.ToJson();

	TArray<uint8> RequestBodyBinary;
	if (!SerializeJsonPayload(JsonObject, RequestBodyBinary)) {
		return false;
	}

	HttpRequest->SetVerb(Api::HttpMethods::Post);
	HttpRequest->SetURL(Api::GetUsersPasswordResetUrl2());
	HttpRequest->SetContent(RequestBodyBinary);

	return FApi2Request::Process();
}

bool FApi2ResetUserPasswordRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	bool bOk = false;

	if (CheckResponse()) {
		TSharedPtr<FJsonObject> JsonPayload;
		if (GetResponseJsonObjectPayload(Response, JsonPayload)) {
			if (ResponseMetadata.FromJson(JsonPayload)) {
				bOk = true;
			}
		}
	}

	return bOk;
}

bool FApi2ChangeUserNameRequest::Process() {
	HttpRequest->SetVerb(Api::HttpMethods::Put);
	HttpRequest->SetURL(FString::Printf(TEXT("%s/%s/%s?%s=%s"), *Api::GetApi2RootUrl(), TEXT("users"),TEXT("me/name"),TEXT("name"),*UserName));

	return FApi2Request::Process();
}

bool FApi2ChangeUserNameRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	bool bOk = false;

	if (CheckResponse()) {
		TSharedPtr<FJsonObject> JsonPayload;
		if (GetResponseJsonObjectPayload(Response, JsonPayload)) {
			if (ResponseMetadata.FromJson(JsonPayload)) {
				bOk = true;
			}
		}
	}

	return bOk;
}

bool FApi2UserGetFriendsRequest::Process() {
	HttpRequest->SetVerb(Api::HttpMethods::Get);
	HttpRequest->SetURL(FString::Printf(TEXT("%s/%s/friends?%s"), *Api::GetUsersUrl2(), *RequestUserId.ToString(EGuidFormats::DigitsWithHyphens), *RequestMetadata.ToUrlParams()));

	return FApi2Request::Process();
}

bool FApi2UserGetFriendsRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	bool bOk = false;

	if (CheckResponse()) {
		TSharedPtr<FJsonObject> JsonPayload;
		if (GetResponseJsonObjectPayload(Response, JsonPayload)) {
			if (ResponseMetadata.FromJson(JsonPayload)) {
				bOk = true;
			}
		}
	}

	return bOk;
}

bool FApi2UserGetFollowersRequest::Process() {
	HttpRequest->SetVerb(Api::HttpMethods::Get);
	HttpRequest->SetURL(FString::Printf(TEXT("%s/%s/followers?include_friends=%s&%s"),
		*Api::GetUsersUrl2(),
		*RequestUserId.ToString(EGuidFormats::DigitsWithHyphens),
		(RequestIncludeFriends ? TEXT("true") : TEXT("false")),
		*RequestMetadata.ToUrlParams()));

	return FApi2Request::Process();
}

bool FApi2UserGetFollowersRequest::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	bool bOk = false;

	if (CheckResponse()) {
		TSharedPtr<FJsonObject> JsonPayload;
		if (GetResponseJsonObjectPayload(Response, JsonPayload)) {
			if (ResponseMetadata.FromJson(JsonPayload)) {
				bOk = true;
			}
		}
	}

	return bOk;
}


bool FApi2UserGetExperience::Process() {
	HttpRequest->SetVerb(Api::HttpMethods::Get);
	HttpRequest->SetURL(FString::Printf(TEXT("%s/%s/experience"), *Api::GetUsersUrl2(), *RequestUserId.ToString(EGuidFormats::DigitsWithHyphens)));

	return FApi2Request::Process();
}

bool FApi2UserGetExperience::OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) {
	bool bOk = false;

	if (CheckResponse()) {
		TSharedPtr<FJsonObject> JsonPayload;
		if (GetResponseJsonObjectPayload(Response, JsonPayload)) {
			if (ResponseMetadata.FromJson(JsonPayload)) {
				bOk = true;
			}
		}
	}

	return bOk;
}

void FApi2UserSubsystem::Initialize() {}

void FApi2UserSubsystem::Shutdown() {}

void FApi2UserSubsystem::GetMe(TSharedRef<FOnUserRequestCompleted2> InCallback) const {
	const auto Request = MakeShared<FApi2GetMeRequest>();

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}

void FApi2UserSubsystem::GetUser(const FGuid& InId, TSharedPtr<FOnUserRequestCompleted2> InCallback) const {
	const auto Request = MakeShared<FApi2GetUserRequest>();

	Request->RequestEntityId = InId;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}

void FApi2UserSubsystem::GetUserByEthAddress(const FString& InAddress, TSharedPtr<FOnUserRequestCompleted2> InCallback) const {
	const auto Request = MakeShared<FApi2GetUserByEthAddressRequest>();

	Request->RequestEthAddress = InAddress;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}

void FApi2UserSubsystem::Follow(const FGuid& InId, TSharedPtr<FOnGenericRequestCompleted2> InCallback) const {
	const auto Request = MakeShared<FApi2UserFollowRequest>();

	Request->RequestUserId = InId;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bSuccessful) {
		InCallback->ExecuteIfBound(Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, TEXT("not processed"));
	}
}

void FApi2UserSubsystem::Unfollow(const FGuid& InId, TSharedPtr<FOnGenericRequestCompleted2> InCallback) const {
	const auto Request = MakeShared<FApi2UserUnfollowRequest>();

	Request->RequestUserId = InId;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bSuccessful) {
		InCallback->ExecuteIfBound(Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, TEXT("not processed"));
	}
}

void FApi2UserSubsystem::IsFollowing(const FGuid& InFollowerId, const FGuid& InLeaderId, TSharedPtr<FOnOkRequestCompleted2> InCallback) const {
	const auto Request = MakeShared<FApi2UserIsFollowingRequest>();

	Request->RequestFollowerId = InFollowerId;
	Request->RequestLeaderId = InLeaderId;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bSuccessful) {
			InCallback->ExecuteIfBound(Request->ResponseMetadata.bOk, Request->GetApiResponseCode(), InRequest->GetErrorString());
		});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}

void FApi2UserSubsystem::GetFollowers(const FGuid& InId, const IApiFollowersRequestMetadata& InMetadata, TSharedPtr<FOnUserBatchRequestCompleted2> InCallback) {
	const auto Request = MakeShared<FApi2UserGetFollowersRequest>();

	Request->RequestMetadata = InMetadata;
	Request->RequestUserId = InId;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}

void FApi2UserSubsystem::GetExperience(const FGuid& InId, TSharedPtr<FOnGenericRequestCompleted2> InCallback) {
	const auto Request = MakeShared<FApi2UserGetExperience>();

	Request->RequestUserId = InId;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bSuccessful) {
		InCallback->ExecuteIfBound(Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, TEXT("not processed"));
	}
}

void FApi2UserSubsystem::GetFriends(const FGuid& InId, const IApiBatchQueryRequestMetadata& InMetadata, TSharedPtr<FOnUserBatchRequestCompleted2> InCallback) {
	const auto Request = MakeShared<FApi2UserGetFriendsRequest>();

	Request->RequestUserId = InId;
	Request->RequestMetadata = InMetadata;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}

void FApi2UserSubsystem::PasswordReset(const FString& Email, TSharedPtr<FOnOkRequestCompleted2> InCallback) {
	const auto Request = MakeShared<FApi2ResetUserPasswordRequest>();

	Request->RequestMetadata.Email = Email;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bSuccessful) {
			InCallback->ExecuteIfBound(Request->ResponseMetadata.bOk, Request->GetApiResponseCode(), InRequest->GetErrorString());
		});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}

void FApi2UserSubsystem::ChangeUserName(const FString& inUserName, TSharedPtr<FOnOkRequestCompleted2> InCallback) {
	const auto Request = MakeShared<FApi2ChangeUserNameRequest>();

	Request->UserName = inUserName;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bSuccessful) {
			InCallback->ExecuteIfBound(Request->ResponseMetadata.bOk, Request->GetApiResponseCode(), InRequest->GetErrorString());
		});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}
