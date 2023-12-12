// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "ApiPersonaSubsystem.h"

#include "ApiFileUploadRequest.h"

const FName FApiPersonaSubsystem::Name = FName("ApiPersonaSubsystem");

void FApiPersonaSubsystem::Initialize() {}

void FApiPersonaSubsystem::Shutdown() {}

bool FApiPersonaSubsystem::GetPersonas(const FGuid& InUserId, const IApiBatchQueryRequestMetadata InMetadata, TSharedPtr<FOnPersonaBatchRequestCompleted> InCallback) {
	if (!InCallback) {
		return false;
	}

	// Callback should be bound otherwise it has no sense to make request.
	if (!InCallback->IsBound()) {
		return false;
	}

	// Create request object.
	const auto RequestPtr = MakeShared<FApiGetPersonaBatchRequest>();

	// Set request metadata.
	RequestPtr->RequestUserId = InUserId;
	RequestPtr->RequestMetadata = InMetadata;

	// Bind request complete lambda.
	RequestPtr->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		// Check if request successful.
		if (bInSuccessful) {
			// Try to cast to target request class.
			const auto Request = StaticCastSharedPtr<FApiGetPersonaBatchRequest>(InRequest);
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
			InCallback->Execute(FApiPersonaBatchMetadata(), bInSuccessful, InRequest->GetErrorString());
		} else {
			LogErrorF("callback is not bound");
		}
	});

	// Process request.
	return RequestPtr->Process();
}

void FApiPersonaSubsystem::GetPersona(const FGuid& InId, TSharedRef<FOnPersonaRequestCompleted> InCallback) {
	const auto Request = MakeShared<FApiGetPersonaRequest>();

	Request->RequestEntityId = InId;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, bInSuccessful, InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, false, TEXT("not processed"));
	}
}

bool FApiPersonaSubsystem::PostPersonas(const FApiCreatePersonaMetadata InMetadata, TSharedPtr<FOnPersonaRequestCompleted> InCallback) {
	if (!InCallback) {
		return false;
	}

	// Callback should be bound otherwise it has no sense to make request.
	if (!InCallback->IsBound()) {
		return false;
	}

	// Create request object.
	const auto RequestPtr = MakeShared<FApiCreatePersonaRequest>();

	// Set request metadata.
	RequestPtr->RequestMetadata = InMetadata;

	// Bind request complete lambda.
	RequestPtr->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		// Check if request successful.
		if (bInSuccessful) {
			// Try to cast to target request class.
			const auto Request = StaticCastSharedPtr<FApiCreatePersonaRequest>(InRequest);
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
			InCallback->Execute(FApiPersonaMetadata(), bInSuccessful, InRequest->GetErrorString());
		} else {
			LogErrorF("callback is not bound");
		}
	});

	// Process request.
	return RequestPtr->Process();
}

bool FApiPersonaSubsystem::PostDefaultPersonas(const FGuid& InPersonaId, TSharedPtr<FOnPersonaRequestCompleted> InCallback) {
	if (!InCallback) {
		return false;
	}

	// Callback should be bound otherwise it has no sense to make request.
	if (!InCallback->IsBound()) {
		return false;
	}

	// Create request object.
	const auto RequestPtr = MakeShared<FApiDefaultPersonaRequest>();

	// Set request metadata.
	RequestPtr->PersonaId = InPersonaId;

	// Bind request complete lambda.
	RequestPtr->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		// Check if request successful.
		if (bInSuccessful) {
			// Try to cast to target request class.
			const auto Request = StaticCastSharedPtr<FApiDefaultPersonaRequest>(InRequest);
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
			InCallback->Execute(FApiPersonaMetadata(), bInSuccessful, InRequest->GetErrorString());
		} else {
			LogErrorF("callback is not bound");
		}
	});

	// Process request.
	return RequestPtr->Process();
}

bool FApiPersonaSubsystem::PatchPersonas(const FApiUpdatePersonaMetadata InMetadata, TSharedPtr<FOnPersonaRequestCompleted> InCallback) {
	if (!InCallback) {
		return false;
	}

	// Callback should be bound otherwise it has no sense to make request.
	if (!InCallback->IsBound()) {
		return false;
	}

	// Create request object.
	const auto RequestPtr = MakeShared<FApiUpdatePersonaRequest>();

	// Set request metadata.
	RequestPtr->RequestMetadata = InMetadata;

	// Bind request complete lambda.
	RequestPtr->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		// Check if request successful.
		if (bInSuccessful) {
			// Try to cast to target request class.
			const auto Request = StaticCastSharedPtr<FApiUpdatePersonaRequest>(InRequest);
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
			InCallback->Execute(FApiPersonaMetadata(), bInSuccessful, InRequest->GetErrorString());
		} else {
			LogErrorF("callback is not bound");
		}
	});

	// Process request.
	return RequestPtr->Process();
}

bool FApiPersonaSubsystem::DeletePersonas(const FApiUpdatePersonaMetadata InMetadata, TSharedPtr<FOnPersonaRequestCompleted> InCallback) {
	if (!InCallback) {
		return false;
	}

	// Callback should be bound otherwise it has no sense to make request.
	if (!InCallback->IsBound()) {
		return false;
	}

	// Create request object.
	const auto RequestPtr = MakeShared<FApiDeletePersonaRequest>();

	// Set request metadata.
	RequestPtr->RequestMetadata = InMetadata;

	// Bind request complete lambda.
	RequestPtr->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		// Check if request successful.
		if (bInSuccessful) {
			// Try to cast to target request class.
			const auto Request = StaticCastSharedPtr<FApiDeletePersonaRequest>(InRequest);
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
			InCallback->Execute(FApiPersonaMetadata(), bInSuccessful, InRequest->GetErrorString());
		} else {
			LogErrorF("callback is not bound");
		}
	});

	// Process request.
	return RequestPtr->Process();
}
