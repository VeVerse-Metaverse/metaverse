// Author: Egor A. Pristavka

#include "SocialPresenceSubsystem.h"

#include "VeShared.h"
#include "VeApi2.h"
#include "Api2AuthSubsystem.h"


const FName FSocialPresenceSubsystem::Name = FName("SocialPresenceSubsystem");


FSocialPresenceSubsystem::FSocialPresenceSubsystem() {
}

void FSocialPresenceSubsystem::Initialize() {
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

void FSocialPresenceSubsystem::Shutdown() {
	GET_MODULE_SUBSYSTEM(AuthSubsystem, Api2, Auth);
	if (AuthSubsystem) {
		if (AuthSubsystem_OnChangeAuthorizationStateHandle.IsValid()) {
			AuthSubsystem->GetOnChangeAuthorizationState().Remove(AuthSubsystem_OnChangeAuthorizationStateHandle);
		}
	}
}

void FSocialPresenceSubsystem::AuthSubsystem_OnUserLoginCallback() {
}

void FSocialPresenceSubsystem::AuthSubsystem_OnUserLogoutCallback() {
}
