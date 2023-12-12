// Author: Egor A. Pristavka

#include "SocialFriendsSubsystem.h"

#include "VeShared.h"
#include "VeApi2.h"
#include "Api2AuthSubsystem.h"
#include "Api2UserSubsystem.h"


const FName FSocialFriendsSubsystem::Name = FName("SocialFriendsSubsystem");


FSocialFriendsSubsystem::FSocialFriendsSubsystem() {}

void FSocialFriendsSubsystem::Initialize() {
	GET_MODULE_SUBSYSTEM(AuthSubsystem, Api2, Auth);
	if (AuthSubsystem) {
		auto OnChangeAuthorizationStateCallback = [=](EAuthorizationState AuthorizationState) {
			if (AuthSubsystem->IsAuthorized()) {
				UserId = AuthSubsystem->GetUserMetadata().Id;
				AuthSubsystem_OnUserLoginCallback();
			} else {
				AuthSubsystem_OnUserLogoutCallback();
				UserId = FGuid();
			}
		};

		AuthSubsystem_OnChangeAuthorizationStateHandle = AuthSubsystem->GetOnChangeAuthorizationState().AddLambda(OnChangeAuthorizationStateCallback);
	}
}

void FSocialFriendsSubsystem::Shutdown() {
	GET_MODULE_SUBSYSTEM(AuthSubsystem, Api2, Auth);
	if (AuthSubsystem) {
		if (AuthSubsystem_OnChangeAuthorizationStateHandle.IsValid()) {
			AuthSubsystem->GetOnChangeAuthorizationState().Remove(AuthSubsystem_OnChangeAuthorizationStateHandle);
		}
	}
}

void FSocialFriendsSubsystem::Refresh() {
	IApiBatchQueryRequestMetadata RequestMetadata;
	RequestMetadata.Offset = 0;
	RequestMetadata.Limit = 100;

	GET_MODULE_SUBSYSTEM(UserSubsystem, Api2, User);
	if (UserSubsystem) {
		TSharedPtr<FOnUserBatchRequestCompleted2> CallbackPtr = MakeShareable(new FOnUserBatchRequestCompleted2());

		CallbackPtr->BindLambda([=](const FApiUserBatchMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError) mutable {
			const bool bInSuccessful = InResponseCode == EApi2ResponseCode::Ok;
			if (!bInSuccessful) {
				LogErrorF("failed to request: %s", *InError);
			}

			OnFriendsResponse(InMetadata, bInSuccessful, InError);

			CallbackPtr.Reset();
		});

		// 3. Make the request using the callback.
		UserSubsystem->GetFriends(UserId, RequestMetadata, CallbackPtr);
	}
}

void FSocialFriendsSubsystem::OnFriendsResponse(const FApiUserBatchMetadata& Metadata, bool bSuccessful, const FString& Error) {
	Users = Metadata.Entities;
	OnFriendListChanged.Broadcast();
}

void FSocialFriendsSubsystem::AuthSubsystem_OnUserLoginCallback() {
#if !WITH_DEV_AUTOMATION_TESTS
	Refresh();
#endif
}

void FSocialFriendsSubsystem::AuthSubsystem_OnUserLogoutCallback() {
#if !WITH_DEV_AUTOMATION_TESTS
	Users.Empty();
	OnFriendListChanged.Broadcast();
#endif
}
