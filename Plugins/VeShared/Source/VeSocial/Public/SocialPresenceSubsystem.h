// Author: Egor A. Pristavka

#pragma once

#include "VeShared.h"
#include "ApiUserMetadata.h"


class VESOCIAL_API FSocialPresenceSubsystem final : public TSharedFromThis<FSocialPresenceSubsystem>, public IModuleSubsystem {
	DECLARE_EVENT(FSocialPresenceSubsystem, FSocialPresenceEvent);

public:
	const static FName Name;
	virtual void Initialize() override;
	virtual void Shutdown() override;

	FSocialPresenceSubsystem();

protected:
private:
	void AuthSubsystem_OnUserLoginCallback();
	void AuthSubsystem_OnUserLogoutCallback();

	FGuid UserId;

	FDelegateHandle AuthSubsystem_OnChangeAuthorizationStateHandle;

};
