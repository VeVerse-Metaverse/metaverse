// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "ApiEventSubsystem.h"

const FName FApiEventSubsystem::Name = "ApiEventSubsystem";

void FApiEventSubsystem::Initialize() {
}

void FApiEventSubsystem::Shutdown() {
}

bool FApiEventSubsystem::GetEvent(const FGuid& InId, TSharedPtr<FOnEventRequestCompleted> InCallback) {
	if (!InCallback) {
		return false;
	}
	
	// Callback should be bound otherwise it has no sense to make request.
	if (!InCallback->IsBound()) {
		return false;
	}

	// Create request object.
	const auto RequestPtr = MakeShared<FApiGetEventRequest>();

	// Set request metadata.
	RequestPtr->RequestEntityId = InId;

	// Bind request complete lambda.
	RequestPtr->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		// Check if request successful.
		if (bInSuccessful) {
			// Try to cast to target request class.
			const auto Request = StaticCastSharedPtr<FApiGetEventRequest>(InRequest);
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
			InCallback->Execute(FApiEventMetadata(), bInSuccessful, InRequest->GetErrorString());
		} else {
			LogErrorF("callback is not bound");
		}
	});

	// Process request.
	return RequestPtr->Process();
}

bool FApiEventSubsystem::GetBatch(const IApiBatchQueryRequestMetadata& InMetadata, TSharedPtr<FOnEventBatchRequestCompleted> InCallback) {
	if (!InCallback) {
		return false;
	}
	
	// Callback should be bound otherwise it has no sense to make request.
	if (!InCallback->IsBound()) {
		return false;
	}

	// Create request object.
	const auto RequestPtr = MakeShared<FApiEventBatchRequest>();

	// Set request metadata.
	RequestPtr->RequestMetadata = InMetadata;

	// Bind request complete lambda.
	RequestPtr->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		// Check if request successful.
		if (bInSuccessful) {
			// Try to cast to target request class.
			const auto Request = StaticCastSharedPtr<FApiEventBatchRequest>(InRequest);
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
			InCallback->Execute(FApiEventBatchMetadata(), bInSuccessful, InRequest->GetErrorString());
		} else {
			LogErrorF("callback is not bound");
		}
	});

	// Process request.
	return RequestPtr->Process();
}

bool FApiEventSubsystem::GetBatchForUser(const IApiUserBatchRequestMetadata& InMetadata, TSharedPtr<FOnEventBatchRequestCompleted> InCallback) {
	if (!InCallback) {
		return false;
	}
	
	// Callback should be bound otherwise it has no sense to make request.
	if (!InCallback->IsBound()) {
		return false;
	}

	// Create request object.
	const auto RequestPtr = MakeShared<FApiUserEventBatchRequest>();

	// Set request metadata.
	RequestPtr->RequestMetadata = InMetadata;

	// Bind request complete lambda.
	RequestPtr->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		// Check if request successful.
		if (bInSuccessful) {
			if (const auto Request = StaticCastSharedPtr<FApiUserEventBatchRequest>(InRequest)) {
				if (InCallback->IsBound()) {
					InCallback->Execute(Request->ResponseMetadata, bInSuccessful, TEXT(""));
				} else {
					LogErrorF("callback is not bound");
				}
				return;
			}
		}

		if (InCallback->IsBound()) {
			InCallback->Execute(FApiEventBatchMetadata(), bInSuccessful, InRequest->GetErrorString());
		} else {
			LogErrorF("callback is not bound");
		}
	});

	// Process request.
	return RequestPtr->Process();
}

bool FApiEventSubsystem::Create(const FApiUpdateEventMetadata& InMetadata, TSharedPtr<FOnEventRequestCompleted> InCallback) {
	if (!InCallback) {
		return false;
	}
	
	// Callback should be bound otherwise it has no sense to make request.
	if (!InCallback->IsBound()) {
		return false;
	}

	// Create request object.
	const auto RequestPtr = MakeShared<FApiCreateEventRequest>();

	// Set request metadata.
	RequestPtr->RequestMetadata = InMetadata;

	// Bind request complete lambda.
	RequestPtr->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		// Check if request successful.
		if (bInSuccessful) {
			// Try to cast to target request class.
			const auto Request = StaticCastSharedPtr<FApiCreateEventRequest>(InRequest);
			if (Request) {
				if (InCallback->IsBound()) {
					InCallback->Execute(Request->ResponseMetadata, bInSuccessful, TEXT(""));

					BroadcastApiEventCreated(Request->ResponseMetadata);
				} else {
					LogErrorF("callback is not bound");
				}
				return;
			}
		}

		if (InCallback->IsBound()) {
			InCallback->Execute(FApiEventMetadata(), bInSuccessful, InRequest->GetErrorString());
		} else {
			LogErrorF("callback is not bound");
		}
	});

	// Process request.
	return RequestPtr->Process();
}

bool FApiEventSubsystem::Update(const FApiUpdateEventMetadata& InMetadata, TSharedPtr<FOnEventRequestCompleted> InCallback) {
	if (!InCallback) {
		return false;
	}
	
	// Callback should be bound otherwise it has no sense to make request.
	if (!InCallback->IsBound()) {
		return false;
	}

	// Create request object.
	const auto RequestPtr = MakeShared<FApiUpdateEventRequest>();

	// Set request metadata.
	RequestPtr->RequestMetadata = InMetadata;

	// Bind request complete lambda.
	RequestPtr->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		// Check if request successful.
		if (bInSuccessful) {
			// Try to cast to target request class.
			const auto Request = StaticCastSharedPtr<FApiUpdateEventRequest>(InRequest);
			if (Request) {
				if (InCallback->IsBound()) {
					InCallback->Execute(Request->ResponseMetadata, bInSuccessful, TEXT(""));

					BroadcastApiEventUpdated(Request->ResponseMetadata);
				} else {
					LogErrorF("callback is not bound");
				}
				return;
			}
		}

		if (InCallback->IsBound()) {
			InCallback->Execute(FApiEventMetadata(), bInSuccessful, InRequest->GetErrorString());
		} else {
			LogErrorF("callback is not bound");
		}
	});

	// Process request.
	return RequestPtr->Process();
}

bool FApiEventSubsystem::Delete(const FGuid& InId, TSharedPtr<FOnGenericRequestCompleted> InCallback) {
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

					BroadcastApiEventDeleted();
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

void FApiEventSubsystem::BroadcastApiEventCreated(const FApiEventMetadata& InMetadata) const {
	if (OnEventCreated.IsBound()) {
		OnEventCreated.Broadcast(InMetadata);
	}
}

void FApiEventSubsystem::BroadcastApiEventUpdated(const FApiEventMetadata& InMetadata) const {
	if (OnEventUpdated.IsBound()) {
		OnEventUpdated.Broadcast(InMetadata);
	}
}

void FApiEventSubsystem::BroadcastApiEventDeleted() const {
	if (OnEventDeleted.IsBound()) {
		OnEventDeleted.Broadcast();
	}
}
