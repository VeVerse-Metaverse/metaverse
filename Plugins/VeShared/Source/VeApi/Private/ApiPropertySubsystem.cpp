// Authors: Egor A. Pristavka, Khusan T. Yadgarov. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "ApiPropertySubsystem.h"


const FName FApiPropertySubsystem::Name = "ApiPropertySubsystem";

void FApiPropertySubsystem::Initialize() {
}

void FApiPropertySubsystem::Shutdown() {
}

bool FApiPropertySubsystem::GetBatch(const FGuid& EntityId, const IApiBatchQueryRequestMetadata& InMetadata, TSharedPtr<FOnPropertyBatchRequestCompleted> InCallback) {
	if (!InCallback) {
		return false;
	}

	// Callback should be bound otherwise it has no sense to make request.
	if (!InCallback->IsBound()) {
		return false;
	}

	// Create request object.
	const auto RequestPtr = MakeShared<FApiPropertyBatchRequest>();

	// Set request metadata.
	RequestPtr->EntityId = EntityId;
	RequestPtr->RequestMetadata = InMetadata;

	// Bind request complete lambda.
	RequestPtr->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		// Check if request successful.
		if (bInSuccessful) {
			// Try to cast to target request class.
			if (const auto Request = StaticCastSharedPtr<FApiPropertyBatchRequest>(InRequest)) {
				if (InCallback->IsBound()) {
					InCallback->Execute(Request->ResponseMetadata, bInSuccessful, TEXT(""));
				} else {
					LogErrorF("callback is not bound");
				}
				return;
			}
		}

		if (InCallback->IsBound()) {
			InCallback->Execute(FApiPropertyBatchMetadata(), bInSuccessful, InRequest->GetErrorString());
		} else {
			LogErrorF("callback is not bound");
		}
	});

	// Process request.
	return RequestPtr->Process();
}

bool FApiPropertySubsystem::Create(const FGuid& EntityId, const FApiUpdatePropertyMetadata& InMetadata, TSharedPtr<FOnPropertyRequestCompleted> InCallback) {
	if (!InCallback) {
		return false;
	}

	// Callback should be bound otherwise it has no sense to make request.
	if (!InCallback->IsBound()) {
		return false;
	}

	// Create request object.
	const auto RequestPtr = MakeShared<FApiCreatePropertyRequest>();

	// Set request metadata.
	RequestPtr->EntityId = EntityId;
	RequestPtr->RequestMetadata = InMetadata;

	// Bind request complete lambda.
	RequestPtr->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		// Check if request successful.
		if (bInSuccessful) {
			// Try to cast to target request class.
			if (const auto Request = StaticCastSharedPtr<FApiCreatePropertyRequest>(InRequest)) {
				if (InCallback->IsBound()) {
					InCallback->Execute(Request->ResponseMetadata, bInSuccessful, TEXT(""));
				} else {
					LogErrorF("callback is not bound");
				}
				return;
			}
		}

		if (InCallback->IsBound()) {
			InCallback->Execute(FApiPropertyMetadata(), bInSuccessful, InRequest->GetErrorString());
		} else {
			LogErrorF("callback is not bound");
		}
	});

	// Process request.
	return RequestPtr->Process();
}

bool FApiPropertySubsystem::Update(const FGuid& EntityId, const FApiUpdatePropertyMetadata& InMetadata, TSharedRef<FOnGenericRequestCompleted> InCallback) {
	// Create request object.
	const auto RequestPtr = MakeShared<FApiUpdatePropertyRequest>();

	// Set request metadata.
	RequestPtr->EntityId = EntityId;
	RequestPtr->RequestMetadata = InMetadata;

	// Bind request complete lambda.
	RequestPtr->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		// Check if request successful.
		if (bInSuccessful) {
			// Try to cast to target request class.
			if (const auto Request = StaticCastSharedPtr<FApiUpdatePropertyRequest>(InRequest)) {
				if (InCallback->IsBound()) {
					InCallback->Execute(bInSuccessful, TEXT(""));
				} else {
					LogErrorF("callback is not bound");
				}
				return;
			}
		}

		if (InCallback->IsBound()) {
			InCallback->Execute(bInSuccessful, InRequest->GetErrorString());
		} else {
			LogErrorF("callback is not bound");
		}
	});

	// Process request.
	return RequestPtr->Process();
}

void FApiPropertySubsystem::Update(const FGuid& EntityId, const TArray<FApiUpdatePropertyMetadata>& InMetadatas, int32 Index, TSharedRef<FOnGenericRequestCompleted> InCallback) {
	const auto Callback = MakeShared<FOnGenericRequestCompleted>();
	Callback->BindLambda([=](const bool bInSuccessful, const FString& InError) {
		if (!bInSuccessful) {
			InCallback->ExecuteIfBound(false, InError);
			return;
		}
		if (InMetadatas.IsValidIndex(Index + 1)) {
			Update(EntityId, InMetadatas, Index + 1, InCallback);
		} else {
			InCallback->ExecuteIfBound(true, InError);
		}
	});

	if (InMetadatas.IsValidIndex(Index)) {
		Update(EntityId, InMetadatas[Index], Callback);
	} else {
		InCallback->ExecuteIfBound(false, TEXT(""));
	}
}

bool FApiPropertySubsystem::Delete(const FGuid& EntityId, const FString& PropertyName, TSharedPtr<FOnGenericRequestCompleted> InCallback) {
	if (!InCallback) {
		return false;
	}

	// Callback should be bound otherwise it has no sense to make request.
	if (!InCallback->IsBound()) {
		return false;
	}

	// Create request object.
	const auto RequestPtr = MakeShared<FApiDeletePropertyRequest>();

	// Set request metadata.
	RequestPtr->EntityId = EntityId;
	RequestPtr->PropertyName = PropertyName;

	// Bind request complete lambda.
	RequestPtr->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		// Check if request successful.
		if (bInSuccessful) {
			if (InCallback->IsBound()) {
				InCallback->Execute(bInSuccessful, TEXT(""));
			} else {
				LogErrorF("callback is not bound");
			}
			return;
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
