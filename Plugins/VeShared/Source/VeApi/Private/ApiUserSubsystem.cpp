// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "ApiUserSubsystem.h"

#include "ApiFileUploadRequest.h"

const FName FApiUserSubsystem::Name = FName("ApiUserSubsystem");

void FApiUserSubsystem::Initialize() {
}

void FApiUserSubsystem::Shutdown() {
}

bool FApiUserSubsystem::GetUser(const FGuid& InId, const TSharedPtr<FOnUserRequestCompleted> InCallback) {
	if (!InCallback) {
		return false;
	}

	// Callback should be bound otherwise it has no sense to make request.
	if (!InCallback->IsBound()) {
		return false;
	}

	// Create request object.
	const auto RequestPtr = MakeShared<FApiGetUserRequest>();

	// Set request metadata.
	RequestPtr->RequestEntityId = InId;

	// Bind request complete lambda.
	RequestPtr->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		// Check if request successful.
		if (bInSuccessful) {
			// Try to cast to target request class.
			const auto Request = StaticCastSharedPtr<FApiGetUserRequest>(InRequest);
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
			InCallback->Execute(FApiUserMetadata(), bInSuccessful, InRequest->GetErrorString());
		} else {
			LogErrorF("callback is not bound");
		}
	});

	// Process request.
	return RequestPtr->Process();
}

bool FApiUserSubsystem::GetUserByEthAddress(const FString& InAddress, TSharedPtr<FOnUserRequestCompleted> InCallback) {
	if (!InCallback) {
		return false;
	}

	// Callback should be bound otherwise it has no sense to make request.
	if (!InCallback->IsBound()) {
		return false;
	}

	// Create request object.
	const auto RequestPtr = MakeShared<FApiGetUserByEthAddressRequest>();

	// Set request metadata.
	RequestPtr->RequestEthAddress = InAddress;

	// Bind request complete lambda.
	RequestPtr->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		// Check if request successful.
		if (bInSuccessful) {
			// Try to cast to target request class.
			const auto Request = StaticCastSharedPtr<FApiGetUserByEthAddressRequest>(InRequest);
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
			InCallback->Execute(FApiUserMetadata(), bInSuccessful, InRequest->GetErrorString());
		} else {
			LogErrorF("callback is not bound");
		}
	});

	// Process request.
	return RequestPtr->Process();
}

bool FApiUserSubsystem::GetBatch(const IApiBatchQueryRequestMetadata& InMetadata, TSharedPtr<FOnUserBatchRequestCompleted> InCallback) {
	if (!InCallback) {
		return false;
	}

	// Callback should be bound otherwise it has no sense to make request.
	if (!InCallback->IsBound()) {
		return false;
	}

	// Create request object.
	const auto RequestPtr = MakeShared<FApiUserBatchRequest>();

	// Set request metadata.
	RequestPtr->RequestMetadata = InMetadata;

	// Bind request complete lambda.
	RequestPtr->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		// Check if request successful.
		if (bInSuccessful) {
			// Try to cast to target request class.
			const auto Request = StaticCastSharedPtr<FApiUserBatchRequest>(InRequest);
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
			InCallback->Execute(FApiUserBatchMetadata(), bInSuccessful, InRequest->GetErrorString());
		} else {
			LogErrorF("callback is not bound");
		}
	});

	// Process request.
	return RequestPtr->Process();
}

bool FApiUserSubsystem::GetAvatarBatch(const FGuid& InUserId, const IApiBatchRequestMetadata& InMetadata, TSharedPtr<FOnFileBatchRequestCompleted> InCallback) {
	if (!InCallback) {
		return false;
	}

	if (!InCallback->IsBound()) {
		return false;
	}

	const auto RequestPtr = MakeShared<FApiGetUserAvatarsRequest>();

	// Set request metadata.
	RequestPtr->RequestMetadata = InMetadata;
	RequestPtr->RequestUserId = InUserId;

	RequestPtr->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		if (bInSuccessful) {
			const auto Request = StaticCastSharedPtr<FApiGetUserAvatarsRequest>(InRequest);
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
			InCallback->Execute(FApiFileBatchMetadata(), bInSuccessful, InRequest->GetErrorString());
		} else {
			LogErrorF("callback is not bound");
		}
	});

	return RequestPtr->Process();
}

#if 0
bool FApiUserSubsystem::GetFileBatch(const IApiBatchRequestMetadata& InMetadata, TSharedPtr<FOnFileBatchRequestCompleted> InCallback) {
	if (!InCallback) {
		return false;
	}

	if (!InCallback->IsBound()) {
		return false;
	}

	const auto RequestPtr = MakeShared<FApiGetUserFilesRequest>();

	RequestPtr->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		if (bInSuccessful) {
			const auto Request = StaticCastSharedPtr<FApiGetUserFilesRequest>(InRequest);
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
			InCallback->Execute(FApiFileBatchMetadata(), bInSuccessful, InRequest->GetErrorString());
		} else {
			LogErrorF("callback is not bound");
		}
	});

	return RequestPtr->Process();
}

bool FApiUserSubsystem::GetAvatarMesh(const FGuid& InUserId, TSharedPtr<FOnFileRequestCompleted> InCallback) {
	if (!InCallback) {
		return false;
	}

	if (!InCallback->IsBound()) {
		return false;
	}

	const auto RequestPtr = MakeShared<FApiGetUserAvatarMeshRequest>();

	RequestPtr->RequestUserId = InUserId;

	RequestPtr->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		if (bInSuccessful) {
			const auto Request = StaticCastSharedPtr<FApiGetUserAvatarMeshRequest>(InRequest);
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
			InCallback->Execute(FApiFileMetadata(), bInSuccessful, InRequest->GetErrorString());
		} else {
			LogErrorF("callback is not bound");
		}
	});

	return RequestPtr->Process();
}

bool FApiUserSubsystem::GetMe(TSharedPtr<FOnUserRequestCompleted> InCallback) {
	if (!InCallback) {
		return false;
	}

	// Callback should be bound otherwise it has no sense to make request.
	if (!InCallback->IsBound()) {
		return false;
	}

	// Create request object.
	const auto RequestPtr = MakeShared<FApiGetUserMeRequest>();

	// Bind request complete lambda.
	RequestPtr->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		// Check if request successful.
		if (bInSuccessful) {
			// Try to cast to target request class.
			const auto Request = StaticCastSharedPtr<FApiGetUserMeRequest>(InRequest);
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
			InCallback->Execute(FApiUserMetadata(), bInSuccessful, InRequest->GetErrorString());
		} else {
			LogErrorF("callback is not bound");
		}
	});

	// Process request.
	return RequestPtr->Process();
}
#endif

bool FApiUserSubsystem::Follow(const FGuid& InId, TSharedPtr<FOnGenericRequestCompleted> InCallback) {
	if (!InCallback) {
		return false;
	}

	// Callback should be bound otherwise it has no sense to make request.
	if (!InCallback->IsBound()) {
		return false;
	}

	// Create request object.
	const auto RequestPtr = MakeShared<FApiUserFollowRequest>();

	// Set request metadata.
	RequestPtr->RequestUserId = InId;

	// Bind request complete lambda.
	RequestPtr->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		// Check if request successful.
		if (bInSuccessful) {
			// Try to cast to target request class.
			const auto Request = StaticCastSharedPtr<FApiUserFollowRequest>(InRequest);
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

bool FApiUserSubsystem::Unfollow(const FGuid& InId, TSharedPtr<FOnGenericRequestCompleted> InCallback) {
	if (!InCallback) {
		return false;
	}

	// Callback should be bound otherwise it has no sense to make request.
	if (!InCallback->IsBound()) {
		return false;
	}

	// Create request object.
	const auto RequestPtr = MakeShared<FApiUserUnfollowRequest>();

	// Set request metadata.
	RequestPtr->RequestUserId = InId;

	// Bind request complete lambda.
	RequestPtr->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		// Check if request successful.
		if (bInSuccessful) {
			// Try to cast to target request class.
			const auto Request = StaticCastSharedPtr<FApiUserUnfollowRequest>(InRequest);
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

bool FApiUserSubsystem::IsFollowing(const FGuid& InFollowerId, const FGuid& InLeaderId, TSharedPtr<FOnOkRequestCompleted> InCallback) {
	if (!InCallback) {
		return false;
	}

	// Callback should be bound otherwise it has no sense to make request.
	if (!InCallback->IsBound()) {
		return false;
	}

	// Create request object.
	const auto RequestPtr = MakeShared<FApiUserIsFollowingRequest>();

	// Set request metadata.
	RequestPtr->RequestFollowerId = InFollowerId;
	RequestPtr->RequestLeaderId = InLeaderId;

	// Bind request complete lambda.
	RequestPtr->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		// Check if request successful.
		if (bInSuccessful) {
			// Try to cast to target request class.
			const auto Request = StaticCastSharedPtr<FApiUserIsFollowingRequest>(InRequest);
			if (Request) {
				if (InCallback->IsBound()) {
					InCallback->Execute(Request->ResponseMetadata.bOk, bInSuccessful, TEXT(""));
				} else {
					LogErrorF("callback is not bound");
				}
				return;
			}
		}

		if (InCallback->IsBound()) {
			InCallback->Execute(false, bInSuccessful, InRequest->GetErrorString());
		} else {
			LogErrorF("callback is not bound");
		}
	});

	// Process request.
	return RequestPtr->Process();
}

bool FApiUserSubsystem::GetFollowers(const FGuid& InId, const IApiFollowersRequestMetadata& InMetadata, TSharedPtr<FOnUserBatchRequestCompleted> InCallback) {
	if (!InCallback) {
		return false;
	}

	// Callback should be bound otherwise it has no sense to make request.
	if (!InCallback->IsBound()) {
		return false;
	}

	// Create request object.
	const auto RequestPtr = MakeShared<FApiUserGetFollowersRequest>();

	// Set request metadata.
	RequestPtr->RequestMetadata = static_cast<IApiBatchQueryRequestMetadata>(InMetadata);
	RequestPtr->RequestUserId = InId;
	RequestPtr->RequestIncludeFriends = InMetadata.IncludeFriends;

	// Bind request complete lambda.
	RequestPtr->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		// Check if request successful.
		if (bInSuccessful) {
			// Try to cast to target request class.
			const auto Request = StaticCastSharedPtr<FApiUserGetFollowersRequest>(InRequest);
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
			InCallback->Execute(FApiUserBatchMetadata(), bInSuccessful, InRequest->GetErrorString());
		} else {
			LogErrorF("callback is not bound");
		}
	});

	// Process request.
	return RequestPtr->Process();
}

bool FApiUserSubsystem::GetExperience(const FGuid& InId, TSharedPtr<FOnUserRequestCompleted> InCallback) {
	if (!InCallback) {
		return false;
	}

	// Callback should be bound otherwise it has no sense to make request.
	if (!InCallback->IsBound()) {
		return false;
	}

	// Create request object.
	const auto RequestPtr = MakeShared<FApiUserGetExperience>();

	// Set request metadata.
	RequestPtr->RequestUserId = InId;

	// Bind request complete lambda.
	RequestPtr->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		// Check if request successful.
		if (bInSuccessful) {
			// Try to cast to target request class.
			const auto Request = StaticCastSharedPtr<FApiUserGetExperience>(InRequest);
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
			InCallback->Execute(FApiUserMetadata(), bInSuccessful, InRequest->GetErrorString());
		} else {
			LogErrorF("callback is not bound");
		}
	});

	// Process request.
	return RequestPtr->Process();
}

bool FApiUserSubsystem::GetFriends(const FGuid& InId, const IApiBatchQueryRequestMetadata& InMetadata, TSharedPtr<FOnUserBatchRequestCompleted> InCallback) {
	if (!InCallback) {
		return false;
	}

	// Callback should be bound otherwise it has no sense to make request.
	if (!InCallback->IsBound()) {
		return false;
	}

	// Create request object.
	const auto RequestPtr = MakeShared<FApiUserGetFriendsRequest>();

	// Set request metadata.
	RequestPtr->RequestMetadata = InMetadata;
	RequestPtr->RequestUserId = InId;

	// Bind request complete lambda.
	RequestPtr->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest>& InRequest, const bool bInSuccessful) {
		// Check if request successful.
		if (bInSuccessful) {
			// Try to cast to target request class.
			const auto Request = StaticCastSharedPtr<FApiUserGetFriendsRequest>(InRequest);
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
			InCallback->Execute(FApiUserBatchMetadata(), bInSuccessful, InRequest->GetErrorString());
		} else {
			LogErrorF("callback is not bound");
		}
	});

	// Process request.
	return RequestPtr->Process();
}

bool FApiUserSubsystem::PasswordReset(const FString& Email, TSharedPtr<FOnOkRequestCompleted> InCallback) {
	if (!Email.Len()) {
		return false;
	}

	// Create request object.
	auto RequestPtr = MakeShared<FApiResetUserPasswordRequest>();

	RequestPtr->RequestMetadata.Email = Email;

	// Bind request complete lambda.
	RequestPtr->GetOnRequestComplete().BindLambda([=](const TSharedPtr<FHttpRequest> InRequest, const bool bInSuccessful) mutable {
		// Check if request successful.
		if (bInSuccessful) {
			// Try to cast to target request class.
			const auto Request = StaticCastSharedPtr<FApiResetUserPasswordRequest>(InRequest);
			if (Request) {
				if (InCallback->IsBound()) {
					InCallback->Execute(Request->ResponseMetadata.bOk, bInSuccessful, TEXT(""));
				} else {
					LogErrorF("callback is not bound");
				}
				return;
			}
		}

		if (InCallback->IsBound()) {
			InCallback->Execute(false, bInSuccessful, InRequest->GetErrorString());
		} else {
			LogErrorF("callback is not bound");
		}

		// RequestPtr.Reset();
	});

	// Process request.
	return RequestPtr->Process();
}
