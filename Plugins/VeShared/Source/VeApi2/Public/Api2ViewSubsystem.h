// Authors: Khusan Yadgarov, Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once
#include "Api2Request.h"
#include "ApiViewMetadata.h"
#include "VeShared.h"

/** Used to update mods. */
class VEAPI2_API FApi2UpdateViewRequest final : public FApi2Request {
public:
	virtual bool Process() override;

	FApiViewMetadata RequestMetadata;
	FApiViewMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};


typedef TDelegate<void(const FApiViewMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError)> FOnViewRequestCompleted2;


class VEAPI2_API FApi2ViewSubsystem final : public IModuleSubsystem {

public:
	static FName Name;

	virtual void Initialize() override;
	virtual void Shutdown() override;
	
	/**
	 * Requests the View update.
	 * @param InMetadata Request metadata.
	 * @param InCallback Complete callback delegate.
	 */
	void UpdateView(const FGuid& Id, TSharedRef<FOnViewRequestCompleted2> InCallback) const;

};

typedef TSharedPtr<FApi2ViewSubsystem> FApi2ViewSubsystemPtr;
