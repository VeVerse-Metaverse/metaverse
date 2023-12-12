// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once
#include "Api2Request.h"
#include "ApiEntityMetadata.h"
#include "ApiRatingMetadata.h"
#include "VeShared.h"

/** Used to update mods. */
class VEAPI2_API FApi2UpdateLikeRequest final : public FApi2Request {
public:
	virtual bool Process() override;

	FApiRatingMetadata RequestMetadata;
	FApiRatingMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

/** Used to update mods. */
class VEAPI2_API FApi2UpdateDislikeRequest final : public FApi2Request {
public:
	virtual bool Process() override;

	FApiRatingMetadata RequestMetadata;
	FApiRatingMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

/** Used to update mods. */
class VEAPI2_API FApi2UpdateUnlikeRequest final : public FApi2Request {
public:
	virtual bool Process() override;

	FApiRatingMetadata RequestMetadata;
	FApiRatingMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};


typedef TDelegate<void(const FApiRatingMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError)> FOnRatingRequestCompleted2;


class VEAPI2_API FApi2RatingSubsystem final : public IModuleSubsystem {

public:
	static FName Name;

	virtual void Initialize() override;
	virtual void Shutdown() override;
	
	/**
	 * Requests the Rating update.
	 * @param InMetadata Request metadata.
	 * @param InCallback Complete callback delegate.
	 */
	void UpdateLike(const FGuid& Id, TSharedRef<FOnRatingRequestCompleted2> InCallback) const;
	void UpdateDisLike(const FGuid& Id, TSharedRef<FOnRatingRequestCompleted2> InCallback) const;
	void UpdateUnlike(const FGuid& Id, TSharedRef<FOnRatingRequestCompleted2> InCallback) const;
	void UpdateState(const FGuid& Id, TSharedRef<FOnRatingRequestCompleted2> InCallback) const;
	

};
