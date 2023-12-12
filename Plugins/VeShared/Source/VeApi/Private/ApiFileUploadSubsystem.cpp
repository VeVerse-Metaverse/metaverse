// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "ApiFileUploadSubsystem.h"
#include "ApiFileUploadRequest.h"
#include "VeShared.h"

const FName FApiFileUploadSubsystem::Name = FName("ApiFileUploadSubsystem");

void FApiFileUploadSubsystem::Initialize() {
}

void FApiFileUploadSubsystem::Shutdown() {
}

bool FApiFileUploadSubsystem::Upload(const FApiUploadFileMetadata& InMetadata, const TSharedPtr<FOnFileRequestCompleted> InCallback) const {
	// Callback should be bound otherwise it has no sense to make request.
	if (!InCallback->IsBound()) {
		return false;
	}

	// Create request object.
	const auto RequestPtr = MakeShared<FApiFileUploadRequest>();

	// Set request metadata.
	RequestPtr->RequestMetadata = InMetadata;

	// Bind request complete lambda.
	RequestPtr->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		// todo: research RequestPtr ref count. Reference count here is 3.
		// Check if request successful.
		if (bInSuccessful) {
			// Try to cast to target request class.
			if (const auto Request = StaticCastSharedPtr<FApiFileUploadRequest>(InRequest)) {
				if (InCallback->IsBound()) {
					InCallback->Execute(Request->ResponseMetadata, bInSuccessful, TEXT(""));
					BroadcastApiFileUploaded(Request->ResponseMetadata, bInSuccessful, TEXT(""));
				} else {
					LogErrorF("callback is not bound");
				}
				return;
			}
		}

		if (InCallback->IsBound()) {
			InCallback->Execute(FApiFileMetadata(), bInSuccessful, InRequest->GetErrorString());
			BroadcastApiFileUploaded(FApiFileMetadata(), bInSuccessful, InRequest->GetErrorString());
		} else {
			LogErrorF("callback is not bound");
		}
	});

	// Process request.
	return RequestPtr->Process();
}

bool FApiFileUploadSubsystem::UploadMemory(const FApiUploadMemoryFileMetadata& InMetadata, const TSharedPtr<FOnFileRequestCompleted> InCallback) const {
	// Callback should be bound otherwise it has no sense to make request.
	if (!InCallback->IsBound()) {
		return false;
	}

	// Create request object.
	const auto RequestPtr = MakeShared<FApiMemoryFileUploadRequest>();

	// Set request metadata.
	RequestPtr->RequestMetadata = InMetadata;

	// Bind request complete lambda.
	RequestPtr->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		// Check if request successful.
		if (bInSuccessful) {
			// Try to cast to target request class.
			if (const auto Request = StaticCastSharedPtr<FApiFileUploadRequest>(InRequest)) {
				if (InCallback->IsBound()) {
					InCallback->Execute(Request->ResponseMetadata, bInSuccessful, TEXT(""));
					BroadcastApiFileUploaded(Request->ResponseMetadata, bInSuccessful, TEXT(""));
				} else {
					LogErrorF("callback is not bound");
				}
				return;
			}
		}

		if (InCallback->IsBound()) {
			InCallback->Execute(FApiFileMetadata(), bInSuccessful, InRequest->GetErrorString());
			BroadcastApiFileUploaded(FApiFileMetadata(), bInSuccessful, InRequest->GetErrorString());
		} else {
			LogErrorF("callback is not bound");
		}
	});

	// Process request.
	return RequestPtr->Process();
}

bool FApiFileUploadSubsystem::Replace(const FApiEntityFileLinkMetadata& InMetadata, const TSharedPtr<FOnFileRequestCompleted> InCallback) const {
	// Callback should be bound otherwise it has no sense to make request.
	if (!InCallback->IsBound()) {
		return false;
	}

	// Create request object.
	const auto RequestPtr = MakeShared<FApiEntityFileLinkRequest>();

	// Set request metadata.
	RequestPtr->RequestMetadata = InMetadata;

	// Bind request complete lambda.
	RequestPtr->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		// Check if request successful.
		if (bInSuccessful) {
			// Try to cast to target request class.
			if (const auto Request = StaticCastSharedPtr<FApiEntityFileLinkRequest>(InRequest)) {
				if (InCallback->IsBound()) {
					InCallback->Execute(Request->ResponseMetadata, bInSuccessful, TEXT(""));
					BroadcastApiFileUploaded(Request->ResponseMetadata, bInSuccessful, TEXT(""));
				} else {
					LogErrorF("callback is not bound");
				}
				return;
			}
		}

		if (InCallback->IsBound()) {
			InCallback->Execute(FApiFileMetadata(), bInSuccessful, InRequest->GetErrorString());
			BroadcastApiFileUploaded(FApiFileMetadata(), bInSuccessful, InRequest->GetErrorString());
		} else {
			LogErrorF("callback is not bound");
		}
	});

	// Process request.
	return RequestPtr->Process();
}

void FApiFileUploadSubsystem::BroadcastApiFileUploaded(const FApiFileMetadata& Metadata, const bool bSuccessful, const FString& Error) const {
	if (OnFileUploaded.IsBound()) {
		OnFileUploaded.Broadcast(Metadata, bSuccessful, Error);
	}
}
