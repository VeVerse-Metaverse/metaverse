// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once
#include "ApiUserRequest.h"
#include "VeShared.h"

class VEAPI_API FApiAuthSubsystem final : public IModuleSubsystem {

public:
	const static FName Name;

	virtual void Initialize() override;
	virtual void Shutdown() override;

	DECLARE_EVENT_ThreeParams(FApiAuthSubsystem, FOnUserRegister, const FApiUserMetadata& /* Metadata */, const bool /* bSuccessful */, const FString& /* Error */);

	DECLARE_EVENT_ThreeParams(FApiAuthSubsystem, FOnUserLogin, const FApiUserMetadata& /* Metadata */, const bool /* bSuccessful */, const FString& /* Error */);

	DECLARE_EVENT(FApiAuthSubsystem, FOnUserLogout)

	DECLARE_EVENT_OneParam(FApiAuthSubsystem, FOnUserChange, const FApiUserMetadata& /* Metadata */);

private:
	/** Pending register request. Valid only during the request. */
	TSharedPtr<FApiRegisterUserRequest> ApiUserRegisterRequest = nullptr;

	/** Pending login request. Valid only during the request. */
	TSharedPtr<FApiUserLoginRequest> ApiUserLoginRequest = nullptr;

	/** Authenticated user metadata. */
	FApiUserMetadata UserMetadata = FApiUserMetadata();

	FOnUserRegister OnUserRegister;
	FOnUserLogin OnUserLogin;
	FOnUserLogout OnUserLogout;
	FOnUserChange OnUserChange;

	/** Notify everyone about user registration. */
	void BroadcastApiUserRegister(bool bSuccessful, const FString& Error) const;

	/** Notify everyone about user login. */
	void BroadcastApiUserLogin(bool bSuccessful, const FString& Error) const;

	/** Notify everyone about user logout. */
	void BroadcastApiUserLogout() const;

	/** Notify everyone about user change. */
	void BroadcastApiUserChange() const;

	static void SaveApiKeyToDisk(const FString& InApiKey);
	static void DeleteApiKeyFromDisk();
	static FString LoadApiKeyFromDisk();

public:
	/** Set authenticated user metadata. */
	void SetUser(const FApiUserMetadata& Metadata);

	/** Initiate register process. Subscribe for the OnUserRegister delegate to process results. */
	bool Register(const FApiRegisterUserMetadata& InMetadata);

	/** Initiate login process. Subscribe for the OnUserLogin delegate to process results. */
	// bool Login(const FApiUserLoginMetadata& InMetadata);

	/** Logs the user out and resets the user information. */
	// void Logout();

	/** Get the logged in user metadata. */
	FApiUserMetadata GetUserMetadata() const { return UserMetadata; }

	// void RefreshUserMetadata();

	/** Get the logged in user Id. */
	const FGuid& GetUserId() const { return UserMetadata.Id; }

	bool IsAdmin() const { return UserMetadata.bIsAdmin; }

	/** Get the logged in user api key. */
	FString GetUserApiKey() const;
	FString GetUserEthAddress() const;

	/** Check if the user is currently logged in and can send signed requests to the API. */
	bool IsLoggedIn() const;

	/** Get user register delegate reference. */
	FOnUserRegister& GetOnUserRegister() { return OnUserRegister; }

	/** Get user login delegate reference. */
	// FOnUserLogin& GetOnUserLogin() { return OnUserLogin; }

	/** Get user logout delegate reference. */
	// FOnUserLogout& GetOnUserLogout() { return OnUserLogout; }

	/** Get user change delegate reference. */
	FOnUserChange& GetOnUserChange() { return OnUserChange; }

};

typedef TSharedPtr<FApiAuthSubsystem> FApiAuthSubsystemPtr;
