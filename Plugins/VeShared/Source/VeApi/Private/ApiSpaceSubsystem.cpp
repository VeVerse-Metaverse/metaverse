// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "ApiSpaceSubsystem.h"
#include "VeApi.h"

const FName FApiSpaceSubsystem::Name = FName("ApiSpaceSubsystem");

void FApiSpaceSubsystem::Initialize() {}

void FApiSpaceSubsystem::Shutdown() {}

// bool FApiSpaceSubsystem::GetSpace(const FGuid& InId, TSharedPtr<FOnSpaceRequestCompleted> InCallback) {
// 	if (!InCallback) {
// 		return false;
// 	}
//
// 	// Callback should be bound otherwise it has no sense to make request.
// 	if (!InCallback->IsBound()) {
// 		return false;
// 	}
//
// 	// Create request.
// 	const auto RequestPtr = MakeShared<FApiGetSpaceRequest>();
//
// 	// Set request metadata.
// 	RequestPtr->RequestEntityId = InId;
//
// 	// Bind request complete lambda.
// 	RequestPtr->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
// 		// Check if request successful.
// 		if (bInSuccessful) {
// 			// Try to cast to target request class.
// 			const auto Request = StaticCastSharedPtr<FApiGetSpaceRequest>(InRequest);
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
// 			InCallback->Execute(FApiSpaceMetadata(), bInSuccessful, InRequest->GetErrorString());
// 		} else {
// 			LogErrorF("callback is not bound");
// 		}
// 	});
//
// 	// Process request.
// 	return RequestPtr->Process();
// }

// bool FApiSpaceSubsystem::GetBatch(const FApiSpaceBatchQueryRequestMetadata& InMetadata, TSharedPtr<FOnSpaceBatchRequestCompleted> InCallback) const {
// 	if (!InCallback) {
// 		return false;
// 	}
//
// 	// Callback should be bound otherwise it has no sense to make request.
// 	if (!InCallback->IsBound()) {
// 		return false;
// 	}
//
// 	// Create request.
// 	const auto RequestPtr = MakeShared<FApiSpaceBatchRequest>();
//
// 	// Set request metadata.
// 	RequestPtr->RequestMetadata = InMetadata;
//
// 	// Bind request complete lambda.
// 	RequestPtr->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
// 		// Check if request successful.
// 		if (bInSuccessful) {
// 			// Try to cast to target request class.
// 			const auto Request = StaticCastSharedPtr<FApiSpaceBatchRequest>(InRequest);
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
// 			InCallback->Execute(FApiSpaceBatchMetadata(), bInSuccessful, InRequest->GetErrorString());
// 		} else {
// 			LogErrorF("callback is not bound");
// 		}
// 	});
//
// 	// Process request.
// 	return RequestPtr->Process();
// }

bool FApiSpaceSubsystem::GetBatchForUser(const IApiUserBatchRequestMetadata& InMetadata, TSharedPtr<FOnSpaceBatchRequestCompleted> InCallback) {
	if (!InCallback) {
		return false;
	}

	// Callback should be bound otherwise it has no sense to make request.
	if (!InCallback->IsBound()) {
		return false;
	}

	// Create request.
	const auto RequestPtr = MakeShared<FApiUserSpaceBatchRequest>();

	// Set request metadata.
	RequestPtr->RequestMetadata = InMetadata;

	// Bind request complete lambda.
	RequestPtr->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		// Check if request successful.
		if (bInSuccessful) {
			// Try to cast to target request class.
			const auto Request = StaticCastSharedPtr<FApiUserSpaceBatchRequest>(InRequest);
			if (Request) {
				if (InCallback->IsBound()) {
					InCallback->Execute(Request->ResponseMetadata, bInSuccessful, TEXT(""));
				} else {
					LogErrorF("callback is not bound");
				}
				return;
			}
		}

		if (InCallback->IsBound()) {
			InCallback->Execute(FApiSpaceBatchMetadata(), bInSuccessful, InRequest->GetErrorString());
		} else {
			LogErrorF("callback is not bound");
		}
	});

	// Process request.
	return RequestPtr->Process();
}

// bool FApiSpaceSubsystem::Create(const FApiUpdateSpaceMetadata& InMetadata, TSharedPtr<FOnSpaceRequestCompleted> InCallback) {
// 	if (!InCallback) {
// 		return false;
// 	}
//
// 	// Callback should be bound otherwise it has no sense to make request.
// 	if (!InCallback->IsBound()) {
// 		return false;
// 	}
//
// 	// Create request. This is very important that the pointer is captured by the lambda.
// 	const auto RequestPtr = MakeShared<FApiCreateSpaceRequest>();
//
// 	// Set request metadata.
// 	RequestPtr->RequestMetadata = InMetadata;
//
// 	// Bind request complete lambda.
// 	RequestPtr->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
// 		// Check if request successful.
// 		if (bInSuccessful) {
// 			// Try to cast to target request class.
// 			const auto Request = StaticCastSharedPtr<FApiCreateSpaceRequest>(InRequest);
// 			if (Request) {
// 				if (InCallback->IsBound()) {
// 					InCallback->Execute(Request->ResponseMetadata, bInSuccessful, TEXT(""));
//
// 					BroadcastApiSpaceCreated(Request->ResponseMetadata);
// 				} else {
// 					LogErrorF("callback is not bound");
// 				}
// 				return;
// 			}
// 		}
//
// 		if (InCallback->IsBound()) {
// 			InCallback->Execute(FApiSpaceMetadata(), bInSuccessful, InRequest->GetErrorString());
// 		} else {
// 			LogErrorF("callback is not bound");
// 		}
// 	});
//
// 	// Process request.
// 	return RequestPtr->Process();
// }

bool FApiSpaceSubsystem::Update(const FApiUpdateSpaceMetadata& InMetadata, TSharedPtr<FOnSpaceRequestCompleted> InCallback) {
	if (!InCallback) {
		return false;
	}

	// Callback should be bound otherwise it has no sense to make request.
	if (!InCallback->IsBound()) {
		return false;
	}

	// Create request.
	const auto RequestPtr = MakeShared<FApiUpdateSpaceRequest>();

	// Set request metadata.
	RequestPtr->RequestMetadata = InMetadata;

	// Bind request complete lambda.
	RequestPtr->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		// Check if request successful.
		if (bInSuccessful) {
			// Try to cast to target request class.
			const auto Request = StaticCastSharedPtr<FApiUpdateSpaceRequest>(InRequest);
			if (Request) {
				if (InCallback->IsBound()) {
					InCallback->Execute(Request->ResponseMetadata, bInSuccessful, TEXT(""));

					BroadcastApiSpaceUpdated(Request->ResponseMetadata);
				} else {
					LogErrorF("callback is not bound");
				}
				return;
			}
		}

		if (InCallback->IsBound()) {
			InCallback->Execute(FApiSpaceMetadata(), bInSuccessful, InRequest->GetErrorString());
		} else {
			LogErrorF("callback is not bound");
		}
	});

	// Process request.
	return RequestPtr->Process();
}

bool FApiSpaceSubsystem::Delete(const FGuid& InId, TSharedPtr<FOnGenericRequestCompleted> InCallback) {
	if (!InCallback) {
		return false;
	}

	// Callback should be bound otherwise it has no sense to make request.
	if (!InCallback->IsBound()) {
		return false;
	}

	// Create request.
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

					BroadcastApiSpaceDeleted();
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

// void FApiSpaceSubsystem::BroadcastApiSpaceCreated(const FApiSpaceMetadata& InMetadata) const {
// 	if (OnSpaceCreated.IsBound()) {
// 		OnSpaceCreated.Broadcast(InMetadata);
// 	}
// }

void FApiSpaceSubsystem::BroadcastApiSpaceUpdated(const FApiSpaceMetadata& InMetadata) const {
	if (OnSpaceUpdated.IsBound()) {
		OnSpaceUpdated.Broadcast(InMetadata);
	}
}

void FApiSpaceSubsystem::BroadcastApiSpaceDeleted() const {
	if (OnSpaceDeleted.IsBound()) {
		OnSpaceDeleted.Broadcast();
	}
}
