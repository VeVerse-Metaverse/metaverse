// Authors: Khusan Yadgarov, Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "Api2WorldSubsystem.h"
#include "VeApi2.h"
#include "GenericPlatform/GenericPlatformHttp.h"

FName FApi2WorldSubsystem::Name = TEXT("Api2WorldSubsystem");

void FApi2WorldSubsystem::Initialize() {
	// Example:
	// TSharedPtr<FOnSpaceBatchRequestCompleted> Callback = MakeShared<FOnSpaceBatchRequestCompleted>();
	// Callback->BindLambda([=](const FApi2SpaceBatchMetadata& InMetadata, const bool bInSuccessful, const FString& Error) {
	// 	if (bInSuccessful) {
	// 		for (const auto EntityPtr : InMetadata.Entities) {
	// 			VeLogFunc("%s", *EntityPtr->Name);
	// 		}
	// 	}
	// });
	// if (Index({}, Callback)) {}
}

void FApi2WorldSubsystem::Shutdown() {}


void FApi2WorldSubsystem::Index(const FApiWorldIndexRequestMetadata& InMetadata, TSharedRef<FOnWorldBatchRequestCompleted2> InCallback) const {
	const auto Request = MakeShared<FApi2WorldIndexRequest>();

	Request->RequestMetadata = InMetadata;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}

void FApi2WorldSubsystem::IndexV2(const FApi2BatchSortRequestMetadata& InMetadata, TSharedRef<FOnWorldBatchRequestCompleted2> InCallback) const {
	const auto Request = MakeShared<FApi2WorldIndexRequestV2>();

	Request->RequestMetadata = InMetadata;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}

void FApi2WorldSubsystem::Get(const FGuid& InWorldId, TSharedRef<FOnWorldRequestCompleted2> InCallback) const {
	const auto Request = MakeShared<FApi2WorldRequest>();

	Request->WorldId = InWorldId;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}

void FApi2WorldSubsystem::GetV2(const FGuid& InWorldId,const FApi2GetRequestMetadata& InMetadata, TSharedRef<FOnWorldRequestCompleted2> InCallback) const {
	const auto Request = MakeShared<FApi2GetWorldRequestV2>();

	Request->RequestMetadata = InMetadata;
	Request->WorldId = InWorldId;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}

void FApi2WorldSubsystem::Create(const FApiUpdateSpaceMetadata& InMetadata, TSharedRef<FOnWorldRequestCompleted2> InCallback) const {
	const auto Request = MakeShared<FApi2CreateWorldRequest>();

	Request->RequestMetadata = InMetadata;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, Request->GetApiResponseCode(), InRequest->GetErrorString());
		// if (Request->GetApiResponseCode() == EApi2ResponseCode::Ok) {
		// 	BroadcastApiSpaceCreated(Request->ResponseMetadata);
		// }
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}

void FApi2WorldSubsystem::Update(const FApiUpdateSpaceMetadata& InMetadata, TSharedRef<FOnWorldRequestCompleted2> InCallback) const {
	const auto Request = MakeShared<FApi2UpdateWorldRequest>();

	Request->RequestMetadata = InMetadata;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}

void FApi2WorldSubsystem::Delete(const FGuid& InWorldId, TSharedRef<FOnWorldRequestCompleted2> InCallback) const {
	const auto Request = MakeShared<FApi2DeleteWorldRequest>();

	Request->RequestMetadataId = InWorldId;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}

void FApi2WorldSubsystem::GetPortalBatch(const FGuid& WorldId, const IApiBatchQueryRequestMetadata& QueryMetadata, TSharedRef<FOnPortalBatchRequestCompleted2> InCallback) const {
	const auto Request = MakeShared<FApi2WorldPortalsBatchRequest>();

	Request->WorldId = WorldId;
	Request->QueryMetadata = QueryMetadata;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}
