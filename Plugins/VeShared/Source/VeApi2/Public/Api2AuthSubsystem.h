// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


#pragma once
#include "Api2Request.h"
#include "ApiEntityMetadata.h"
#include "ApiUserMetadata.h"
#include "VeShared.h"


enum class EAuthorizationState {
	// Before authenticate
	Unknown,

	// Token failed
	Unauthenticated,

	Unauthorized,
	Authorized,

	LogoutSuccess,
	LoginFail,
	LoginSuccess,
};


class VEAPI2_API FApi2TokenMetadata final : public FApiEntityMetadata {
public:
	FString Token = FString();

	virtual bool FromJson(const TSharedPtr<FJsonObject> JsonObject) override;
};

class VEAPI2_API FApi2UserLoginRequest final : public FApi2Request {
public:
	virtual bool Process() override;

	FApiUserLoginMetadata RequestMetadata;
	FApi2TokenMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

class VEAPI2_API FApi2AuthSubsystem final : public IModuleSubsystem {

	DECLARE_EVENT_OneParam(FApi2AuthSubsystem, FOnAuthorizationEvent, EAuthorizationState AuthorizationState);

	typedef TMulticastDelegate<void(const FApiUserMetadata&)> FOnUserChanged;

public:
	static FName Name;

	virtual void Initialize() override;
	virtual void Shutdown() override;

public:
	/** Initiate login process. Subscribe for the OnUserLogin delegate to process results. */
	void Login(const FApiUserLoginMetadata& InMetadata);

	/** Login using token received from the OAuth process. */
	void LoginWithOAuthToken(const FString& InToken);

	/** Logs the user out and resets the user information. */
	void Logout();

	/** Get the logged in user api key. */
	const FString& GetSessionToken() const {
		return SessionToken;
	}

	/** Get the logged in user metadata. */
	const FApiUserMetadata& GetUserMetadata() const {
		return UserMetadata;
	}

	/** Get the logged in user Id. */
	const FGuid& GetUserId() const {
		return UserMetadata.Id;
	}

	bool IsAdmin() const {
		return UserMetadata.bIsAdmin;
	}

	FOnUserChanged& GetOnUserChanged() {
		return OnUserChanged;
	}

	/** Get the logged in user api key. (APIv1) */
	const FString& GetUserApiKey() const {
		return UserMetadata.ApiKey;
	}

	bool IsAuthorized() const {
		return UserMetadata.Id.IsValid();
	}

	FOnAuthorizationEvent& GetOnChangeAuthorizationState() {
		return OnChangeAuthorizationState;
	}

	EAuthorizationState GetAuthorizationState() const {
		return AuthorizationState;
	}

	const FString& GetAuthorizationMessage() const {
		return AuthorizationMessage;
	}

private:
	static bool SaveSession(const FString& InToken);

protected:
	void SetAuthorizationState(EAuthorizationState InAuthorizationState, const FString& Message);

private:
	EAuthorizationState AuthorizationState = EAuthorizationState::Unknown;
	FOnAuthorizationEvent OnChangeAuthorizationState;
	FString AuthorizationMessage;

	FOnUserChanged OnUserChanged;

	/** Authenticated user metadata. */
	FApiUserMetadata UserMetadata = FApiUserMetadata();

	/** Set authenticated user metadata. */
	void SetUser(const FApiUserMetadata& InMetadata);

	/** Session token. */
	FString SessionToken;

	void RequestUserMetadata(TSharedRef<FOnGenericRequestCompleted2> InCallback);

	static bool LoadSession(FString& OutToken);
	void ClearSession();

};
