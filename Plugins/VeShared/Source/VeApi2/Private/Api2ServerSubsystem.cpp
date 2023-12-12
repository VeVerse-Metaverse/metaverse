// Authors: Khusan Yadgarov, Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "Api2ServerSubsystem.h"

FName FApi2ServerSubsystem::Name = TEXT("Api2ServerSubsystem");

void FApi2ServerSubsystem::Initialize() { }

void FApi2ServerSubsystem::Shutdown() { }

#if 0
void FApi2ServerSubsystem::GetServer(const FGuid& InServerId, TSharedRef<FOnServerRequestCompleted1> InCallback) const {
	const auto Request = MakeShared<FApi2GetServerRequest>();

	Request->RequestEntityId = InServerId;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}
#endif

void FApi2ServerSubsystem::Delete(const FGuid& InServerId, const TSharedRef<FOnGenericRequestCompleted2> InCallback) const {
	const auto Request = MakeShared<FApi2UnregisterServerRequest>();

	Request->RequestServerId = InServerId;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		InCallback->ExecuteIfBound(Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound(EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}

void FApi2ServerSubsystem::Heartbeat(const FGuid& InServerId, const FApi2RequestServerHeartbeatMetadata& InRequestMetadata, const TSharedRef<FOnGenericRequestCompleted2> InCallback) const {
	const auto Request = MakeShared<FApi2ServerHeartbeatRequest>();

	Request->ServerId = InServerId;
	Request->RequestMetadata = InRequestMetadata;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		InCallback->ExecuteIfBound(Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound(EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}

void FApi2ServerSubsystem::GetScheduledWorld(TSharedRef<FOnWorldRequestCompleted2> InCallback) const {
	const auto Request = MakeShared<FApi2GetScheduledWorldRequest>();

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}

void FApi2ServerSubsystem::GetMatchingServer(const FApi2RequestServerMatchMetadata& InRequestMetadata, TSharedRef<FOnServerRequestCompleted2> InCallback) const {
	const auto Request = MakeShared<FApi2GetMatchingServerRequest>();

	Request->RequestMetadata = InRequestMetadata;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}

void FApi2ServerSubsystem::PlayerConnect(const FGuid& InServerId, const FGuid& InPlayerId, TSharedRef<FOnGenericRequestCompleted2> InCallback) const {
	const auto Request = MakeShared<FApi2ServerPlayerConnectRequest>();

	Request->ServerId = InServerId;
	Request->RequestMetadata.PlayerId = InPlayerId;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		InCallback->ExecuteIfBound(Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound(EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}

void FApi2ServerSubsystem::PlayerDisconnect(const FGuid& InServerId, const FGuid& InPlayerId, TSharedRef<FOnGenericRequestCompleted2> InCallback) const {
	const auto Request = MakeShared<FApi2ServerPlayerDisconnectRequest>();

	Request->ServerId = InServerId;
	Request->PlayerId = InPlayerId;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		InCallback->ExecuteIfBound(Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound(EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}
