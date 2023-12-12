// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "ApiPortalSubsystem.h"

const FName FApiPortalSubsystem::Name = "ApiPortalSubsystem";

void FApiPortalSubsystem::Initialize() {}

void FApiPortalSubsystem::Shutdown() {}

// void FApiPortalSubsystem::GetPortal(const FGuid& InId, TSharedRef<FOnPortalRequestCompleted> InCallback) {
// 	// Create request object.
// 	const auto Request = MakeShared<FApiGetPortalRequest>();
//
// 	// Set request metadata.
// 	Request->RequestEntityId = InId;
//
// 	// Bind request complete lambda.
// 	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
// 		InCallback->ExecuteIfBound(Request->ResponseMetadata, bInSuccessful, InRequest->GetErrorString());
// 	});
//
// 	// Process request.
// 	if (!Request->Process()) {
// 		InCallback->ExecuteIfBound({}, false, TEXT("not processed"));
// 	}
// }

// bool FApiPortalSubsystem::GetBatch(const IApiBatchQueryRequestMetadata& InMetadata, TSharedPtr<FOnPortalBatchRequestCompleted> InCallback) {
// 	if (!InCallback) {
// 		return false;
// 	}
//
// 	// Callback should be bound otherwise it has no sense to make request.
// 	if (!InCallback->IsBound()) {
// 		return false;
// 	}
//
// 	// Create request object.
// 	const auto RequestPtr = MakeShared<FApiPortalBatchRequest>();
//
// 	// Set request metadata.
// 	RequestPtr->RequestMetadata = InMetadata;
//
// 	// Bind request complete lambda.
// 	RequestPtr->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
// 		// Check if request successful.
// 		if (bInSuccessful) {
// 			// Try to cast to target request class.
// 			const auto Request = StaticCastSharedPtr<FApiPortalBatchRequest>(InRequest);
// 			if (Request) {
// 				if (InCallback->IsBound()) {
// 					InCallback->Execute(Request->ResponseMetadata, bInSuccessful, TEXT(""));
// 				} else {
// 					LogErrorF("callback is not bound");
// 				}
// 				return;
// 			}
// 		}
//
// 		if (InCallback->IsBound()) {
// 			InCallback->Execute(FApiPortalBatchMetadata(), bInSuccessful, InRequest->GetErrorString());
// 		} else {
// 			LogErrorF("callback is not bound");
// 		}
// 	});
//
// 	// Process request.
// 	return RequestPtr->Process();
// }

//
// bool FApiPortalSubsystem::GetBatchForUser(const IApiUserBatchRequestMetadata& InMetadata, TSharedPtr<FOnPortalBatchRequestCompleted> InCallback) {
// 	if (!InCallback) {
// 		return false;
// 	}
// 	
// 	// Callback should be bound otherwise it has no sense to make request.
// 	if (!InCallback->IsBound()) {
// 		return false;
// 	}
//
// 	// Create request object.
// 	const auto RequestPtr = MakeShared<FApiUserPortalBatchRequest>();
//
// 	// Set request metadata.
// 	RequestPtr->RequestMetadata = InMetadata;
//
// 	// Bind request complete lambda.
// 	RequestPtr->GetOnRequestComplete().BindLambda([this, InCallback, RequestPtr](const TSharedPtr<FApiRequest> InRequest, const bool bInSuccessful) mutable {
// 		// Check if request successful.
// 		if (bInSuccessful) {
// 			if (const auto Request = StaticCastSharedPtr<FApiUserPortalBatchRequest>(InRequest)) {
// 				if (InCallback->IsBound()) {
// 					InCallback->Execute(Request->ResponseMetadata, bInSuccessful, TEXT(""));
// 					InCallback.Reset();
// 				} else {
// 					LogErrorF("callback is not bound");
// 				}
// 				return;
// 			}
// 		}
//
// 		if (InCallback->IsBound()) {
// 			InCallback->Execute(FApiPortalBatchMetadata(), bInSuccessful, InRequest->GetErrorString());
// 			InCallback.Reset();
// 		} else {
// 			LogErrorF("callback is not bound");
// 		}
//
// 		RequestPtr.Reset();
// 	});
//
// 	// Process request.
// 	return RequestPtr->Process();
// }

// void FApiPortalSubsystem::Create(const FApiUpdatePortalMetadata& InMetadata, TSharedRef<FOnPortalRequestCompleted> InCallback) {
// 	// Create request object.
// 	const auto Request = MakeShared<FApiCreatePortalRequest>();
//
// 	// Set request metadata.
// 	Request->RequestMetadata = InMetadata;
//
// 	// Bind request complete lambda.
// 	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
// 		InCallback->ExecuteIfBound(Request->ResponseMetadata, bInSuccessful, InRequest->GetErrorString());
// 	});
//
// 	// Process request.
// 	if (!Request->Process()) {
// 		InCallback->ExecuteIfBound({}, false, TEXT("not processed"));
// 	}
// }

void FApiPortalSubsystem::Update(const FApiUpdatePortalMetadata& InMetadata, TSharedRef<FOnPortalRequestCompleted> InCallback) {
	// Create request object.
	const auto Request = MakeShared<FApiUpdatePortalRequest>();

	// Set request metadata.
	Request->RequestMetadata = InMetadata;

	// Bind request complete lambda.
	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, bInSuccessful, InRequest->GetErrorString());
	});

	// Process request.
	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, false, TEXT("not processed"));
	}
}

bool FApiPortalSubsystem::Delete(const FGuid& InId) const {
	// Create request object.
	const auto RequestPtr = MakeShared<FApiDeleteEntityRequest>();

	// Set request metadata.
	RequestPtr->Id = InId;

	// Bind request complete lambda.
	RequestPtr->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		// Check if request successful.
		if (bInSuccessful) {
			// Try to cast to target request class.
			const auto Request = StaticCastSharedPtr<FApiDeleteEntityRequest>(InRequest);
			if (Request) {
				return;
			}
		}
	});

	// Process request.
	return RequestPtr->Process();
}

bool FApiPortalSubsystem::Delete(const FGuid& InId, TSharedPtr<FOnGenericRequestCompleted> InCallback) {
	if (!InCallback) {
		return false;
	}

	// Callback should be bound otherwise it has no sense to make request.
	if (!InCallback->IsBound()) {
		return false;
	}

	// Create request object.
	const auto RequestPtr = MakeShared<FApiDeleteEntityRequest>();

	// Set request metadata.
	RequestPtr->Id = InId;

	// Bind request complete lambda.
	RequestPtr->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		// Check if request successful.
		if (bInSuccessful) {
			// Try to cast to target request class.
			const auto Request = StaticCastSharedPtr<FApiDeleteEntityRequest>(InRequest);
			if (Request) {
				if (InCallback->IsBound()) {
					InCallback->Execute(bInSuccessful, TEXT(""));
				} else {
					LogErrorF("callback is not bound");
				}
				return;
			}
		}

		if (InCallback->IsBound()) {
			InCallback->Execute(false, InRequest->GetErrorString());
		} else {
			LogErrorF("callback is not bound");
		}
	});

	// Process request.
	return RequestPtr->Process();
}
