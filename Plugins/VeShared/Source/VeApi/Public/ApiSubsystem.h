// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "Subsystems/GameInstanceSubsystem.h"

// #include "ApiUserMetadata.h"
// #include "ApiServer.h"
// #include "ApiModMetadataObject.h"

#include "ApiSubsystem.generated.h"

UCLASS()
class VEAPI_API UApiSubsystem final : public UGameInstanceSubsystem {
	GENERATED_BODY()
//
// #pragma region User Authentication
//
// public:
// 	/** Cached user state including API key for further requests, etc. */
// 	FApiUserMetadata ApiUser;
//
// 	UFUNCTION(BlueprintCallable)
// 	bool IsAuthenticated() const;
//
// 	/** Fires when any user change is detected, e.g. initial login, name and description change, etc. */
// 	DECLARE_EVENT_OneParam(UApiSubsystem, FOnApiUserChanged, const FApiUserMetadata&);
// 	/** Broadcasts user data whenever user data is changed. */
// 	FOnApiUserChanged OnApiUserChanged;
//
// 	/** Fires when user is authenticated initially using login and password. */
// 	DECLARE_EVENT_ThreeParams(UApiSubsystem, FOnApiUserAuthenticated, const FApiUserMetadata&, bool, const FString&);
// 	/** Broadcasts user data when user authentication attempt made. */
// 	FOnApiUserAuthenticated OnApiUserAuthenticated;
//
// 	/** Authenticate user using email and password. Will store the user data in the subsystem for further use and will broadcast the authenticated delegate. */
// 	bool Authenticate(const FApiUserLoginMetadata& LoginData);
//
// 	/** Authenticate user using email and password. Will store the user data in the subsystem for further use and will broadcast the authenticated delegate. */
// 	UFUNCTION(BlueprintCallable)
// 	bool AuthenticateApi(const FString& ApiKey);
//
// 	/** Fires when user is authenticated initially using login and password. */
// 	DECLARE_EVENT_ThreeParams(UApiSubsystem, FOnApiUserRegistered, const FApiUserMetadata&, bool, const FString&);
//
// 	/** Broadcasts user data when user authentication attempt made. */
// 	FOnApiUserRegistered OnApiUserRegistered;
//
// 	/** Register the user. Will store the user data in the subsystem for further use and will broadcast the authenticated delegate. */
// 	bool Register(const FApiRegisterUserMetadata& RegisterData);
//
// 	/** Fires when user is authenticated initially using login and password. */
// 	DECLARE_EVENT_OneParam(UApiSubsystem, FOnApiUserPasswordReset, bool);
//
// 	/** Broadcasts user data when user authentication attempt made. */
// 	FOnApiUserPasswordReset OnApiUserPasswordReset;
//
// 	/** Request the user's password reset. If the email matches existing user's email, the system will send reset password link via email to the user. */
// 	UFUNCTION(BlueprintCallable)
// 	bool ResetPassword(const FString& Email);
//
// 	/** Fires when user is logged out. */
// 	DECLARE_EVENT(UApiSubsystem, FOnApiUserLogout);
//
// 	FOnApiUserLogout OnApiUserLogout;
//
// 	UFUNCTION(BlueprintCallable)
// 	void Logout();
//
// private:
// 	/** Updates the user data and triggers the corresponding delegate. */
// 	void SetApiUser(const FApiUserMetadata& InUser);
//
// #pragma endregion
//
// #pragma region Server Browser and Matchmaking
// public:
// 	DECLARE_EVENT_OneParam(UApiSubsystem, FOnApiServersLoaded, const FApiServerRefBatch&);
//
// 	FOnApiServersLoaded OnApiServersLoaded;
//
// 	bool GetServers(const FApiServerQuery& Query = FApiServerQuery());
// #pragma endregion

// #pragma region Mod Browser
// 	DECLARE_EVENT_OneParam(UApiSubsystem, FOnApiModsLoaded, const FApiModRefBatch&);
// 	DECLARE_EVENT_OneParam(UApiSubsystem, FOnApiSpacesLoaded, const FApiSpaceRefBatch&);
//
// 	FOnApiModsLoaded OnApiModsLoaded;
// 	FOnApiSpacesLoaded OnApiSpacesLoaded;
//
// 	bool GetMods(const FApiModQuery& Query = FApiModQuery());
// 	bool GetSpaces(const FApiSpaceRefQuery& Query = FApiSpaceRefQuery());
// #pragma endregion


// #pragma region Mod file download
	// DECLARE_EVENT_OneParam(UApiSubsystem, FOnFileDownloadComplete, bool)
	// FOnFileDownloadComplete OnFileDownloadComplete;

	// bool GetModFile(const FString& Url);
// #pragma endregion
};
