// Authors: Egor A. Pristavka, Khusan T. Yadgarov. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


#include "ApiTemplateSubsystem.h"

const FName FApiTemplateSubsystem::Name = FName("ApiSpaceSubsystem");

void FApiTemplateSubsystem::Initialize() {
}

void FApiTemplateSubsystem::Shutdown() {
}

bool FApiTemplateSubsystem::GetTemplate(const FGuid& InId, TSharedPtr<FOnTemplateRequestCompleted> InCallback) {
	if (!InCallback) {
		return false;
	}

	// Callback should be bound otherwise it has no sense to make request.
	if (!InCallback->IsBound()) {
		return false;
	}

	// Create request.
	const auto RequestPtr = MakeShared<FApiGetTemplateRequest>();

	// Set request metadata.
	RequestPtr->RequestEntityId = InId;

	// Bind request complete lambda.
	RequestPtr->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		// Check if request successful.
		if (bInSuccessful) {
			// Try to cast to target request class.
			const auto Request = StaticCastSharedPtr<FApiGetTemplateRequest>(InRequest);
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
			InCallback->Execute(FApiTemplateMetadata(), bInSuccessful, InRequest->GetErrorString());
		} else {
			LogErrorF("callback is not bound");
		}
	});

	// Process request.
	return RequestPtr->Process();
}

bool FApiTemplateSubsystem::GetBatch(const IApiBatchQueryRequestMetadata& InMetadata, TSharedPtr<FOnTemplateBatchRequestCompleted> InCallback) const {
	if (!InCallback) {
		return false;
	}

	// Callback should be bound otherwise it has no sense to make request.
	if (!InCallback->IsBound()) {
		return false;
	}

	// Create request.
	const auto RequestPtr = MakeShared<FApiTemplateBatchRequest>();

	// Set request metadata.
	RequestPtr->RequestMetadata = InMetadata;

	// Bind request complete lambda.
	RequestPtr->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		// Check if request successful.
		if (bInSuccessful) {
			// Try to cast to target request class.
			const auto Request = StaticCastSharedPtr<FApiTemplateBatchRequest>(InRequest);
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
			InCallback->Execute(FApiTemplateBatchMetadata(), bInSuccessful, InRequest->GetErrorString());
		} else {
			LogErrorF("callback is not bound");
		}
	});

	// Process request.
	return RequestPtr->Process();
}

bool FApiTemplateSubsystem::Create(const FApiUpdateTemplateMetadata& InMetadata, TSharedPtr<FOnTemplateRequestCompleted> InCallback) {
	if (!InCallback) {
		return false;
	}

	// Callback should be bound otherwise it has no sense to make request.
	if (!InCallback->IsBound()) {
		return false;
	}

	// Create request. This is very important that the pointer is captured by the lambda.
	const auto RequestPtr = MakeShared<FApiCreateTemplateRequest>();

	// Set request metadata.
	RequestPtr->RequestMetadata = InMetadata;

	// Bind request complete lambda.
	RequestPtr->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		// Check if request successful.
		if (bInSuccessful) {
			// Try to cast to target request class.
			const auto Request = StaticCastSharedPtr<FApiCreateTemplateRequest>(InRequest);
			if (Request) {
				if (InCallback->IsBound()) {
					InCallback->Execute(Request->ResponseMetadata, bInSuccessful, TEXT(""));

					BroadcastApiTemplateCreated(Request->ResponseMetadata);
				} else {
					LogErrorF("callback is not bound");
				}
				return;
			}
		}

		if (InCallback->IsBound()) {
			InCallback->Execute(FApiTemplateMetadata(), bInSuccessful, InRequest->GetErrorString());
		} else {
			LogErrorF("callback is not bound");
		}
	});

	// Process request.
	return RequestPtr->Process();
}

bool FApiTemplateSubsystem::Update(const FApiUpdateTemplateMetadata& InMetadata, TSharedPtr<FOnTemplateRequestCompleted> InCallback) {
	if (!InCallback) {
		return false;
	}

	// Callback should be bound otherwise it has no sense to make request.
	if (!InCallback->IsBound()) {
		return false;
	}

	// Create request.
	const auto RequestPtr = MakeShared<FApiUpdateTemplateRequest>();

	// Set request metadata.
	RequestPtr->RequestMetadata = InMetadata;

	// Bind request complete lambda.
	RequestPtr->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		// Check if request successful.
		if (bInSuccessful) {
			// Try to cast to target request class.
			const auto Request = StaticCastSharedPtr<FApiUpdateTemplateRequest>(InRequest);
			if (Request) {
				if (InCallback->IsBound()) {
					InCallback->Execute(Request->ResponseMetadata, bInSuccessful, TEXT(""));

					BroadcastApiTemplateUpdated(Request->ResponseMetadata);
				} else {
					LogErrorF("callback is not bound");
				}
				return;
			}
		}

		if (InCallback->IsBound()) {
			InCallback->Execute(FApiTemplateMetadata(), bInSuccessful, InRequest->GetErrorString());
		} else {
			LogErrorF("callback is not bound");
		}
	});

	// Process request.
	return RequestPtr->Process();
}

// bool FApiTemplateSubsystem::Delete(const FGuid& InId, TSharedPtr<FOnGenericRequestCompleted> InCallback) {
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
// 	const auto RequestPtr = MakeShared<FApiDeleteEntityRequest>();
//
// 	// Set request metadata.
// 	RequestPtr->Id = InId;
//
// 	// Bind request complete lambda.
// 	RequestPtr->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
// 		// Check if request successful.
// 		if (bInSuccessful) {
// 			// Try to cast to target request class.
// 			const auto Request = StaticCastSharedPtr<FApiDeleteEntityRequest>(InRequest);
// 			if (Request) {
// 				if (InCallback->IsBound()) {
// 					InCallback->Execute(bInSuccessful, TEXT(""));
//
// 					BroadcastApiSpaceDeleted();
// 				} else {
// 					LogErrorF("callback is not bound");
// 				}
// 				return;
// 			}
// 		}
//
// 		if (InCallback->IsBound()) {
// 			InCallback->Execute(false, InRequest->GetErrorString());
// 		} else {
// 			LogErrorF("callback is not bound");
// 		}
// 	});
//
// 	// Process request.
// 	return RequestPtr->Process();
// }


void FApiTemplateSubsystem::BroadcastApiTemplateCreated(const FApiTemplateMetadata& InMetadata) const {
	if (OnTemplateCreated.IsBound()) {
		OnTemplateCreated.Broadcast(InMetadata);
	}
}

void FApiTemplateSubsystem::BroadcastApiTemplateUpdated(const FApiTemplateMetadata& InMetadata) const {
	if (OnTemplateUpdated.IsBound()) {
		OnTemplateUpdated.Broadcast(InMetadata);
	}
}

void FApiTemplateSubsystem::BroadcastApiTemplateDeleted() const {
	if (OnTemplateDeleted.IsBound()) {
		OnTemplateDeleted.Broadcast();
	}
}
