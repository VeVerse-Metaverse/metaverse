// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "ApiEntityMetadata.h"
#include "ApiFileMetadata.h"
#include "ApiPersonaMetadata.h"
#include "ApiPresenceMetadata.h"

/** User entity public reference. */
class VEAPI_API FApiUserMetadata final : public FApiEntityMetadata {
public:
	/** User api key to sign requests. */
	FString ApiKey = FString();

	/** User name. */
	FString Name = FString();

	/** User name. */
	FString Email = FString();

	/** User title. */
	FString Title = FString();

	/** User profile description. */
	FString Description = FString();

	/** User level. */
	int32 Level = 0;

	/** User rank. */
	FString Rank = FString();

	/** User likes. */
	int32 TotalLikes = 0;

	/** User dislikes. */
	int32 TotalDislikes = 0;

	/** User experience. */
	int32 Experience = 0;

	/** User location data. */
	FString Geolocation = FString();

	TArray<FApiFileMetadata> Files;

	FApiPresenceMetadata PresenceMetadata = {};

	FApiPersonaMetadata DefaultPersonaMetadata;

	/** Is user profile active, if true, then the user can authenticate and use the application. */
	bool bIsActive = false;

	/** Does the user have admin privileges or not. */
	bool bIsAdmin = false;

	/** Is the user globally muted by the administrator and can not participate in voice chat. */
	bool bIsMuted = false;

	/** Is the user banned by the administrator and can not login or communicate with the API. */
	bool bIsBanned = false;

	/** Is the user permitted the application to use his email address to send notification emails. */
	bool bAllowEmails = false;

	/** Ethereum public address. */
	FString EthAddress;

	/** Rating calculation. -1 = no rating. */
	float GetRating() const;

	FString GetDefaultAvatarUrl() const;
	EApiPresenceStatus GetStatus() const;
	FString GetRank() const;

	/** Current level progress. */
	float GetLevelExperience() const;

	virtual bool FromJson(const TSharedPtr<FJsonObject> JsonObject) override;
};

/** Used for user login requests. */
class VEAPI_API FApiUserLoginMetadata final : public IApiMetadata {
public:
	FString Email;
	FString Password;

	virtual TSharedPtr<FJsonObject> ToJson() override;
};

/** Used for user registration requests. */
class VEAPI_API FApiRegisterUserMetadata final : public IApiMetadata {
public:
	FString Email = FString();
	FString Name = FString();
	FString Password = FString();
	FString PasswordConfirmation = FString();
	FString InviteCode = FString();

	virtual TSharedPtr<FJsonObject> ToJson() override;
};

/** Used for user update password requests. */
class VEAPI_API FApiUpdateUserPasswordMetadata final : public IApiMetadata {
public:
	FString Password = FString();
	FString NewPassword = FString();
	FString NewPasswordConfirmation = FString();

	virtual TSharedPtr<FJsonObject> ToJson() override;
};

/** Used for user reset password requests. */
class VEAPI_API FApiResetUserPasswordMetadata final : public IApiMetadata {
public:
	FString Email = FString();

	virtual TSharedPtr<FJsonObject> ToJson() override;
};

/** Used for user profile update requests. */
class VEAPI_API FApiUpdateUserMetadata final : public IApiMetadata {
public:
	FString Name = FString();
	FString Description = FString();

	virtual TSharedPtr<FJsonObject> ToJson() override;
};

typedef TApiBatchMetadata<class FApiUserMetadata/*, class UApiUserMetadataObject*/> FApiUserBatchMetadata;
