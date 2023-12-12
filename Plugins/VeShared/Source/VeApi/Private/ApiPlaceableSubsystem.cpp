// Authors: Egor A. Pristavka, Khusan T. Yadgarov. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "ApiPlaceableSubsystem.h"
#include "ApiPlaceableRequest.h"
#include "VeApi.h"

const FName FApiPlaceableSubsystem::Name = FName("ApiPlaceableSubsystem");

void FApiPlaceableSubsystem::Initialize() {}

void FApiPlaceableSubsystem::Shutdown() {}

bool FApiPlaceableSubsystem::GetPlaceable(const FGuid& InPlaceableId, TSharedPtr<FOnPlaceableRequestCompleted> InCallback) {
	if (!InCallback) {
		return false;
	}

	// Callback should be bound otherwise it has no sense to make request.
	if (!InCallback->IsBound()) {
		return false;
	}

	// Create request.
	const auto RequestPtr = MakeShared<FApiGetPlaceableRequest>();

	// Set request metadata.
	RequestPtr->RequestEntityId = InPlaceableId;

	// Bind request complete lambda.
	RequestPtr->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		// Check if request successful.
		if (bInSuccessful) {
			// Try to cast to target request class.
			const auto Request = StaticCastSharedPtr<FApiGetPlaceableRequest>(InRequest);
			if (Request) {
				if (InCallback->IsBound()) {
					InCallback->Execute(Request->ResponseMetadata, Request->ResponseCode, TEXT(""));
				} else {
					LogErrorF("callback is not bound");
				}
				return;
			}
		}

		if (InCallback->IsBound()) {
			InCallback->Execute(FApiPlaceableMetadata(), RequestPtr->ResponseCode, InRequest->GetErrorString());
		} else {
			LogErrorF("callback is not bound");
		}
	});

	// Process request.
	return RequestPtr->Process();
}

bool FApiPlaceableSubsystem::GetPlaceableBatch(const FGuid& InSpaceId, const IApiBatchRequestMetadata& InMetadata, TSharedPtr<FOnPlaceableBatchRequestCompleted> InCallback) {
	if (!InCallback) {
		return false;
	}

	// Callback should be bound otherwise it has no sense to make request.
	if (!InCallback->IsBound()) {
		return false;
	}

	// Create request.
	const auto RequestPtr = MakeShared<FApiPlaceableBatchRequest>();

	// Set request metadata.
	RequestPtr->SpaceId = InSpaceId;
	RequestPtr->RequestMetadata = InMetadata;

	// Bind request complete lambda.
	RequestPtr->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		// Check if request successful.
		if (bInSuccessful) {
			// Try to cast to target request class.
			const auto Request = StaticCastSharedPtr<FApiPlaceableBatchRequest>(InRequest);
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
			InCallback->Execute(FApiPlaceableBatchMetadata(), bInSuccessful, InRequest->GetErrorString());
		} else {
			LogErrorF("callback is not bound");
		}
	});

	// Process request.
	return RequestPtr->Process();
}


bool FApiPlaceableSubsystem::UpdatePlaceable(const FGuid& InSpaceId, const FApiPlaceableMetadata& InPlaceableMetadata, TSharedPtr<FOnPlaceableRequestCompleted> InCallback) {
	if (!InCallback) {
		VeLogFunc("no callback");
		return false;
	}

	// Callback should be bound otherwise it has no sense to make request.
	if (!InCallback->IsBound()) {
		return false;
	}

	// Create request.
	const auto RequestPtr = MakeShared<FApiUpdateSpacePlaceableRequest>();

	// Set request metadata.
	RequestPtr->Id = InSpaceId;
	RequestPtr->RequestMetadata = InPlaceableMetadata;

	// Bind request complete lambda.
	RequestPtr->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		// Check if request successful.
		if (bInSuccessful) {
			// Try to cast to target request class.
			const auto Request = StaticCastSharedPtr<FApiUpdateSpacePlaceableRequest>(InRequest);
			if (Request) {
				if (InCallback->IsBound()) {
					InCallback->Execute(Request->ResponsePlaceableMetadata, Request->ResponseCode, TEXT(""));
				} else {
					LogErrorF("callback is not bound");
				}
				return;
			}
		}

		if (InCallback->IsBound()) {
			InCallback->Execute(FApiPlaceableMetadata{}, RequestPtr->ResponseCode, InRequest->GetErrorString());
		} else {
			LogErrorF("callback is not bound");
		}
	});

	// Process request.
	return RequestPtr->Process();
}

bool FApiPlaceableSubsystem::UpdatePlaceableTransform(const FApiUpdatePlaceableTransformMetadata& InPlaceableMetadata, TSharedPtr<FOnPlaceableRequestCompleted> InCallback) {
	if (!InCallback) {
		return false;
	}

	// Callback should be bound otherwise it has no sense to make request.
	if (!InCallback->IsBound()) {
		return false;
	}

	// Create request.
	const auto RequestPtr = MakeShared<FApiUpdatePlaceableTransformRequest>();

	// Set request metadata.
	RequestPtr->RequestMetadata = InPlaceableMetadata;

	// Bind request complete lambda.
	RequestPtr->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		// Check if request successful.
		if (bInSuccessful) {
			// Try to cast to target request class.
			const auto Request = StaticCastSharedPtr<FApiUpdatePlaceableTransformRequest>(InRequest);
			if (Request) {
				if (InCallback->IsBound()) {
					InCallback->Execute(Request->ResponseMetadata, Request->ResponseCode, TEXT(""));
				} else {
					LogErrorF("callback is not bound");
				}
				return;
			}
		}

		if (InCallback->IsBound()) {
			InCallback->Execute({}, RequestPtr->ResponseCode, InRequest->GetErrorString());
		} else {
			LogErrorF("callback is not bound");
		}
	});

	// Process request.
	return RequestPtr->Process();
}

void FApiPlaceableSubsystem::UpdatePlaceableEntity(const FGuid& InPlaceableId, const FGuid& InEntityId, TSharedRef<FOnPlaceableRequestCompleted> InCallback) {
	// Create request.
	const auto Request = MakeShared<FApiUpdatePlaceableEntityRequest>();

	// Set request metadata.
	Request->RequestPlaceableId = InPlaceableId;
	Request->RequestEntityId = InEntityId;

	// Bind request complete lambda.
	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, Request->ResponseCode, InRequest->GetErrorString());
	});

	// Process request.
	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, EApiResponseCode::Failed, TEXT("not processed"));
	}
}

bool FApiPlaceableSubsystem::DeletePlaceable(const FGuid& InPlaceableId, TSharedPtr<FOnGenericRequestCompleted> InCallback) {
	if (!InCallback) {
		return false;
	}

	// Callback should be bound otherwise it has no sense to make request.
	if (!InCallback->IsBound()) {
		return false;
	}

	// Create request.
	const auto RequestPtr = MakeShared<FApiDeletePlaceableRequest>();

	// Set request metadata.
	RequestPtr->Id = InPlaceableId;

	// Bind request complete lambda.
	RequestPtr->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		// Check if request successful.
		if (bInSuccessful) {
			// Try to cast to target request class.
			const auto Request = StaticCastSharedPtr<FApiDeletePlaceableRequest>(InRequest);
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

bool FApiPlaceableSubsystem::GetPlaceableClassBatch(const FApiPlaceableClassBatchQueryRequestMetadata& InMetadata, TSharedPtr<FOnPlaceableClassBatchRequestCompleted> InCallback) {
	if (!InCallback) {
		return false;
	}

	// Callback should be bound otherwise it has no sense to make request.
	if (!InCallback->IsBound()) {
		return false;
	}

	// Create request.
	const auto RequestPtr = MakeShared<FApiGetPlaceableBatchClassRequest>();

	// Set request metadata.
	RequestPtr->RequestMetadata = InMetadata;

	// Bind request complete lambda.
	RequestPtr->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		// Check if request successful.
		if (bInSuccessful) {
			// Try to cast to target request class.
			const auto Request = StaticCastSharedPtr<FApiGetPlaceableBatchClassRequest>(InRequest);
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
			InCallback->Execute(FApiPlaceableClassBatchMetadata(), bInSuccessful, InRequest->GetErrorString());
		} else {
			LogErrorF("callback is not bound");
		}
	});

	// Process request.
	return RequestPtr->Process();
}

