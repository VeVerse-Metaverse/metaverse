// Author: Egor A. Pristavka

#pragma once

#include "VeShared.h"
#include "ApiUserMetadata.h"


class VESOCIAL_API FSocialFriendsSubsystem final : public TSharedFromThis<FSocialFriendsSubsystem>, public IModuleSubsystem {
	DECLARE_EVENT(FSocialFriendsSubsystem, FSocialFriendsEvent);

	DECLARE_EVENT_OneParam(FSocialFriendsSubsystem, FSocialFriendsUserEvent, const TSharedPtr<FApiUserMetadata>& User);

public:
	const static FName Name;
	virtual void Initialize() override;
	virtual void Shutdown() override;

	FSocialFriendsSubsystem();

	TArray<TSharedPtr<FApiUserMetadata>> Users;

	void Refresh();

	FSocialFriendsUserEvent& GetOnFriendChanged() { return OnFriendChanged; }
	FSocialFriendsEvent& GetOnFriendListChanged() { return OnFriendListChanged; }

protected:
	void OnFriendsResponse(const FApiUserBatchMetadata& Metadata, bool bSuccessful, const FString& Error);

private:
	void AuthSubsystem_OnUserLoginCallback();
	void AuthSubsystem_OnUserLogoutCallback();

	FSocialFriendsUserEvent OnFriendChanged;
	FSocialFriendsEvent OnFriendListChanged;

	FGuid UserId;

	FDelegateHandle AuthSubsystem_OnChangeAuthorizationStateHandle;

};
