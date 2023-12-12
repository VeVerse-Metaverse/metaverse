// Authors: Khusan Yadgarov, Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "Api2PortalSubsystem.h"
#include "VeApi2.h"
#include "GenericPlatform/GenericPlatformHttp.h"

FName FApi2PortalSubsystem::Name = TEXT("Api2PortalSubsystem");

void FApi2PortalSubsystem::Initialize() {
	// Example:
	// TSharedPtr<FOnPortalBatchRequestCompleted> Callback = MakeShared<FOnPortalBatchRequestCompleted>();
	// Callback->BindLambda([=](const FApi2PortalBatchMetadata& InMetadata, const bool bInSuccessful, const FString& Error) {
	// 	if (bInSuccessful) {
	// 		for (const auto EntityPtr : InMetadata.Entities) {
	// 			VeLogFunc("%s", *EntityPtr->Name);
	// 		}
	// 	}
	// });
	// if (Index({}, Callback)) {}
}

void FApi2PortalSubsystem::Shutdown() {}

void FApi2PortalSubsystem::Index(const FApiPortalIndexRequestMetadata& InMetadata, TSharedRef<FOnPortalBatchRequestCompleted2> InCallback) const {
	const auto Request = MakeShared<FApi2PortalBatchRequest>();
	Request->RequestMetadata = InMetadata;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}

void FApi2PortalSubsystem::Get(const FGuid& Id, TSharedRef<FOnPortalRequestCompleted2> InCallback) const {
	const auto Request = MakeShared<FApi2PortalRequest>();
	Request->RequestMetadata.Id = Id;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}

void FApi2PortalSubsystem::Create(const FApiUpdatePortalMetadata& InMetadata, TSharedRef<FOnPortalRequestCompleted2> InCallback) const {
	const auto Request = MakeShared<FApi2CreatePortalRequest>();

	Request->RequestMetadata = InMetadata;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, Request->GetApiResponseCode(), InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, EApi2ResponseCode::Failed, TEXT("not processed"));
	}
}
