// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "BlockchainNftSubsystem.h"

const FName FBlockchainNftSubsystem::Name = "BlockchainNftSubsystem";

void FBlockchainNftSubsystem::Initialize() {
}

void FBlockchainNftSubsystem::Shutdown() {
}

bool FBlockchainNftSubsystem::GetOwnerOf(const uint32 InId, const TSharedPtr<FOnOwnerOfRequestCompleted> InCallback) const {
	if (!InCallback) {
		return false;
	}

	// Callback should be bound otherwise it has no sense to make request.
	if (!InCallback->IsBound()) {
		return false;
	}

	// Create request.
	const auto RequestPtr = MakeShared<FBlockchainOwnerOfRequest>();

	// Set request metadata.
	RequestPtr->RequestTokenId = InId;

	// Bind request complete lambda.
	RequestPtr->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		// Check if request successful.
		if (bInSuccessful) {
			if (InCallback->IsBound()) {
				InCallback->Execute(RequestPtr->ResponseOwnerAddress, bInSuccessful, TEXT(""));
			} else {
				LogErrorF("callback is not bound");
			}
			return;
		}

		if (InCallback->IsBound()) {
			InCallback->Execute({}, bInSuccessful, InRequest->GetErrorString());
		} else {
			LogErrorF("callback is not bound");
		}
	});

	// Process request.
	return RequestPtr->Process();
}

bool FBlockchainNftSubsystem::GetTokenUri(const uint32 InId, const TSharedPtr<FOnTokenUriRequestCompleted> InCallback) const {
	if (!InCallback) {
		return false;
	}

	// Callback should be bound otherwise it has no sense to make request.
	if (!InCallback->IsBound()) {
		return false;
	}

	// Create request.
	const auto RequestPtr = MakeShared<FBlockchainTokenUriRequest>();

	// Set request metadata.
	RequestPtr->RequestTokenId = InId;

	// Bind request complete lambda.
	RequestPtr->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		// Check if request successful.
		if (bInSuccessful) {
			if (InCallback->IsBound()) {
				InCallback->Execute(RequestPtr->ResponseMetadata, bInSuccessful, TEXT(""));
			} else {
				LogErrorF("callback is not bound");
			}
			return;
		}

		if (InCallback->IsBound()) {
			InCallback->Execute({}, bInSuccessful, InRequest->GetErrorString());
		} else {
			LogErrorF("callback is not bound");
		}
	});

	// Process request.
	return RequestPtr->Process();
}
