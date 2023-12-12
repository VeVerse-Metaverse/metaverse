// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once
#include "Api2Request.h"
#include "ApiFileMetadata.h"
#include "VeShared.h"

class VEAPI2_API FApi2AppIdentityImageRequestMetadata final : public IApiMetadata {
public:
	FApi2AppIdentityImageRequestMetadata() = default;
	explicit FApi2AppIdentityImageRequestMetadata(const FGuid& InId);

	FGuid Id;

	virtual TSharedPtr<FJsonObject> ToJson() override;
	virtual FString ToUrlParams() override;
};

class VEAPI2_API FApi2AppIdentityImageResponseMetadata final : public IApiMetadata {
public:
	TArray<FApiFileMetadata> Files;

	bool FromJsonArray(const TArray<TSharedPtr<FJsonValue>> Json);
};

class VEAPI2_API FApi2AppIdentityImageRequest final : public FApi2Request {
public:
	FApi2AppIdentityImageRequestMetadata Request;
	FApi2AppIdentityImageResponseMetadata Response;

	virtual bool Process() override;
	virtual bool OnComplete(const FHttpRequestPtr& InRequest, const FHttpResponsePtr& InResponse, bool bSuccessful) override;
};

typedef TDelegate<void(const FApi2AppIdentityImageResponseMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError)> FOnApi2AppIdentityImageRequestCompleted;

class VEAPI2_API FApi2AppSubsystem final : public IModuleSubsystem {
public:
	virtual void Initialize() override;
	virtual void Shutdown() override;

	void GetAppIdentityImages(const FGuid& Id, TSharedRef<FOnApi2AppIdentityImageRequestCompleted> InCallback) const;

	static FName Name;
};
