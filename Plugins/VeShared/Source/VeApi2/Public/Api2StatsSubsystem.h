// Authors: Khusan Yadgarov, Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "Api2Request.h"
#include "ApiStatsMetadata.h"
#include "VeShared.h"

typedef TDelegate<void(const FApiStatsMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError)> FOnStatsRequestCompleted;

class VEAPI2_API FApi2StatsRequest final : public FApi2Request {
public:
	virtual bool Process() override;
	FApiStatsMetadata ResponseMetadata;
protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

class VEAPI2_API FApi2StatsSubsystem final : public IModuleSubsystem {

public:
	static FName Name;

	virtual void Initialize() override;
	virtual void Shutdown() override;

	void Get(TSharedRef<FOnStatsRequestCompleted> InCallback) const;
};
