// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "ApiServerSubsystem.h"


const FName FApiServerSubsystem::Name = FName("ApiServerSubsystem");

void FApiServerSubsystem::Initialize() {
}

void FApiServerSubsystem::Shutdown() {
}

FGuid FApiServerSubsystem::GetRegisteredServerId() const {
	return RegisteredServerId;
}

#if 0
void FApiServerSubsystem::GetScheduledWorld(TSharedRef<FOnSpaceRequestCompleted> InCallback) {

	const auto Request = MakeShared<FApiGetScheduledSpaceRequest>();
	
	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, bInSuccessful, InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, false, TEXT("not processed"));
	}
}
#endif

void FApiServerSubsystem::GetWorld(const FGuid& InId, const TSharedRef<FOnSpaceRequestCompleted> InCallback) {

	const auto Request = MakeShared<FApiGetSpaceRequest>();

	Request->RequestEntityId = InId;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, bInSuccessful, InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, false, TEXT("not processed"));
	}
}

#if 0
void FApiServerSubsystem::GetServer(const FGuid& InId, TSharedRef<FOnServerRequestCompleted> InCallback) {
	const auto Request = MakeShared<FApiGetServerRequest>();

	Request->RequestEntityId = InId;

	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, bInSuccessful, InRequest->GetErrorString());
	});

	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, false, TEXT("not processed"));
	}
}
#endif

#if 1
void FApiServerSubsystem::GetMatchingServer(const FGuid& InSpaceId, TSharedRef<FOnServerRequestCompleted> InCallback) const {
	// Create request object.
	const auto Request = MakeShared<FApiGetMatchingServerRequest>();

	// Set request metadata.
	Request->RequestSpaceId = InSpaceId;

	// Bind request complete lambda.
	Request->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		InCallback->ExecuteIfBound(Request->ResponseMetadata, bInSuccessful, InRequest->GetErrorString());
	});

	// Process request.
	if (!Request->Process()) {
		InCallback->ExecuteIfBound({}, false, TEXT("not processed"));
	}
}
#endif

bool FApiServerSubsystem::GetBatch(const IApiBatchQueryRequestMetadata& InMetadata, TSharedPtr<FOnServerBatchRequestCompleted> InCallback) {
	if (!InCallback) {
		return false;
	}

	// Callback should be bound otherwise it has no sense to make request.
	if (!InCallback->IsBound()) {
		return false;
	}

	// Create request object.
	const auto RequestPtr = MakeShared<FApiServerBatchRequest>();

	// Set request metadata.
	RequestPtr->RequestMetadata = InMetadata;

	// Bind request complete lambda.
	RequestPtr->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		// Check if request successful.
		if (bInSuccessful) {
			// Try to cast to target request class.
			const auto Request = StaticCastSharedPtr<FApiServerBatchRequest>(InRequest);
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
			InCallback->Execute(FApiServerBatchMetadata(), bInSuccessful, InRequest->GetErrorString());
		} else {
			LogErrorF("callback is not bound");
		}
	});

	// Process request.
	return RequestPtr->Process();
}

bool FApiServerSubsystem::GetBatchForUser(const IApiUserBatchRequestMetadata& InMetadata, TSharedPtr<FOnServerBatchRequestCompleted> InCallback) {
	if (!InCallback) {
		return false;
	}

	// Callback should be bound otherwise it has no sense to make request.
	if (!InCallback->IsBound()) {
		return false;
	}

	// Create request object.
	const auto RequestPtr = MakeShared<FApiUserServerBatchRequest>();

	// Set request metadata.
	RequestPtr->RequestMetadata = InMetadata;

	// Bind request complete lambda.
	RequestPtr->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		// Check if request successful.
		if (bInSuccessful) {
			// Try to cast to target request class.
			const auto Request = StaticCastSharedPtr<FApiUserServerBatchRequest>(InRequest);
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
			InCallback->Execute(FApiServerBatchMetadata(), bInSuccessful, InRequest->GetErrorString());
		} else {
			LogErrorF("callback is not bound");
		}
	});

	// Process request.
	return RequestPtr->Process();
}

bool FApiServerSubsystem::Create(const FApiUpdateServerMetadata& InMetadata, const TSharedPtr<FOnServerRequestCompleted> InCallback) const {
	if (!InCallback) {
		return false;
	}

	// Callback should be bound otherwise it has no sense to make request.
	if (!InCallback->IsBound()) {
		return false;
	}

	// Create request object.
	const auto RequestPtr = MakeShared<FApiRegisterServerRequest>();

	// Set request metadata.
	RequestPtr->RequestMetadata = InMetadata;

	// Bind request complete lambda.
	RequestPtr->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		// Check if request successful.
		if (bInSuccessful) {
			// Try to cast to target request class.
			const auto Request = StaticCastSharedPtr<FApiRegisterServerRequest>(InRequest);
			if (Request) {
				if (InCallback->IsBound()) {
					InCallback->Execute(Request->ResponseMetadata, bInSuccessful, TEXT(""));

					BroadcastApiServerRegistered(Request->ResponseMetadata);
				} else {
					LogErrorF("callback is not bound");
				}
				return;
			}
		}

		if (InCallback->IsBound()) {
			InCallback->Execute(FApiServerMetadata{}, bInSuccessful, InRequest->GetErrorString());
		} else {
			LogErrorF("callback is not bound");
		}
	});

	// Process request.
	return RequestPtr->Process();
}

bool FApiServerSubsystem::Update(const FApiUpdateServerMetadata& InMetadata, const TSharedPtr<FOnServerRequestCompleted> InCallback) const {
	if (!InCallback) {
		return false;
	}

	// Callback should be bound otherwise it has no sense to make request.
	if (!InCallback->IsBound()) {
		return false;
	}

	// Create request object.
	const auto RequestPtr = MakeShared<FApiUpdateServerRequest>();

	// Set request metadata.
	RequestPtr->RequestMetadata = InMetadata;

	// Bind request complete lambda.
	RequestPtr->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		// Check if request successful.
		if (bInSuccessful) {
			// Try to cast to target request class.
			if (const auto Request = StaticCastSharedPtr<FApiUpdateServerRequest>(InRequest)) {
				if (InCallback->IsBound()) {
					InCallback->Execute(Request->ResponseMetadata, bInSuccessful, TEXT(""));
				} else {
					LogErrorF("callback is not bound");
				}
				return;
			}
		}

		if (InCallback->IsBound()) {
			InCallback->Execute(FApiServerMetadata{}, bInSuccessful, InRequest->GetErrorString());
		} else {
			LogErrorF("callback is not bound");
		}
	});

	// Process request.
	return RequestPtr->Process();
}

#if 0
bool FApiServerSubsystem::Delete(const FGuid& InServerId, const TSharedPtr<FOnGenericRequestCompleted> InCallback) const {
	if (!InCallback) {
		return false;
	}

	// Callback should be bound otherwise it has no sense to make request.
	if (!InCallback->IsBound()) {
		return false;
	}

	// Create request object.
	const auto RequestPtr = MakeShared<FApiUnregisterServerRequest>();

	// Set request metadata.
	RequestPtr->RequestServerId = InServerId;

	// Bind request complete lambda.
	RequestPtr->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		// Check if request successful.
		if (bInSuccessful) {
			// Try to cast to target request class.
			const auto Request = StaticCastSharedPtr<FApiUnregisterServerRequest>(InRequest);
			if (Request) {
				if (InCallback->IsBound()) {
					InCallback->Execute(bInSuccessful, TEXT(""));

					BroadcastApiServerUnregistered();
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
#endif

bool FApiServerSubsystem::Connect(const FGuid& InServerId, const FGuid& InUserId, TSharedPtr<FOnGenericRequestCompleted> InCallback) {
	if (!InCallback) {
		return false;
	}

	// Callback should be bound otherwise it has no sense to make request.
	if (!InCallback->IsBound()) {
		return false;
	}

	// Create request object.
	const auto RequestPtr = MakeShared<FApiServerPlayerConnectRequest>();

	// Set request metadata.
	RequestPtr->RequestServerId = InServerId;
	RequestPtr->RequestUserId = InUserId;

	// Bind request complete lambda.
	RequestPtr->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		// Check if request successful.
		if (bInSuccessful) {
			// Try to cast to target request class.
			const auto Request = StaticCastSharedPtr<FApiServerPlayerConnectRequest>(InRequest);
			if (Request) {
				if (InCallback->IsBound()) {
					InCallback->Execute(bInSuccessful, TEXT(""));

					BroadcastApiServerPlayerConnected(InUserId);
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

bool FApiServerSubsystem::Disconnect(const FGuid& InServerId, const FGuid& InUserId, TSharedPtr<FOnGenericRequestCompleted> InCallback) {
	if (!InCallback) {
		return false;
	}

	// Callback should be bound otherwise it has no sense to make request.
	if (!InCallback->IsBound()) {
		return false;
	}

	// Create request object.
	const auto RequestPtr = MakeShared<FApiServerPlayerDisconnectRequest>();

	// Set request metadata.
	RequestPtr->RequestServerId = InServerId;
	RequestPtr->RequestUserId = InUserId;

	// Bind request complete lambda.
	RequestPtr->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		// Check if request successful.
		if (bInSuccessful) {
			// Try to cast to target request class.
			const auto Request = StaticCastSharedPtr<FApiServerPlayerDisconnectRequest>(InRequest);
			if (Request) {
				if (InCallback->IsBound()) {
					InCallback->Execute(bInSuccessful, TEXT(""));

					BroadcastApiServerPlayerDisconnected(InUserId);
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

#if 0
bool FApiServerSubsystem::Heartbeat(const FGuid& InServerId, const FString& InStatus, const FString& InDetails, const TSharedPtr<FOnGenericRequestCompleted> InCallback) const {
	if (!InCallback) {
		return false;
	}

	// Callback should be bound otherwise it has no sense to make request.
	if (!InCallback->IsBound()) {
		return false;
	}

	// Create request object.
	const auto RequestPtr = MakeShared<FApiServerHeartbeatRequest>();

	// Set request metadata.
	RequestPtr->RequestServerId = InServerId;
	RequestPtr->RequestStatus = InStatus;
	RequestPtr->RequestDetails = InDetails;

	// Bind request complete lambda.
	RequestPtr->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		// Check if request successful.
		if (bInSuccessful) {
			// Try to cast to target request class.
			const auto Request = StaticCastSharedPtr<FApiServerHeartbeatRequest>(InRequest);
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
			InCallback->Execute(bInSuccessful, InRequest->GetErrorString());
		} else {
			LogErrorF("callback is not bound");
		}
	});

	// Process request.
	return RequestPtr->Process();
}
#endif

void FApiServerSubsystem::BroadcastApiServerRegistered(const FApiServerMetadata& InMetadata) const {
	if (OnServerRegistered.IsBound()) {
		OnServerRegistered.Broadcast(InMetadata);
	}
}

void FApiServerSubsystem::BroadcastApiServerUnregistered() const {
	if (OnServerUnregistered.IsBound()) {
		OnServerUnregistered.Broadcast();
	}
}

void FApiServerSubsystem::BroadcastApiServerPlayerConnected(const FGuid& InPlayerId) const {
	if (OnServerPlayerConnected.IsBound()) {
		OnServerPlayerConnected.Broadcast(InPlayerId);
	}
}

void FApiServerSubsystem::BroadcastApiServerPlayerDisconnected(const FGuid& InPlayerId) const {
	if (OnServerPlayerDisconnected.IsBound()) {
		OnServerPlayerDisconnected.Broadcast(InPlayerId);
	}
}
