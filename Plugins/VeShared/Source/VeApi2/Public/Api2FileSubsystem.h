// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


#pragma once
#include "Api2Request.h"
#include "ApiEntityMetadata.h"
#include "ApiFileMetadata.h"
#include "ApiRequest.h"
#include "VeShared.h"


class VEAPI2_API FApi2FileUploadRequestMetadata final : public FApiUploadFileMetadata {
public:
	explicit FApi2FileUploadRequestMetadata(bool SetCurrentPlatform = false);

	FString Mime = FString();
	int64 Version = 0;
	FString Deployment = FString();
	FString Platform = FString();
	int32 Width = 0;
	int32 Height = 0;
	int64 Variant = 0;
	FString OriginalPath;
	FString Hash;

	virtual TSharedPtr<FJsonObject> ToJson() override;
	virtual FString ToUrlParams() override;
};

class VEAPI2_API FApi2FileUploadMemoryRequestMetadata final : public IApiMetadata {
public:
	FApi2FileUploadMemoryRequestMetadata();

	/** Id of the entity to attach the file to. */
	FGuid Id;

	/** Name of the file. */
	FString FileName;
	
	/** Type of the file. */
	FString FileType;

	/** Source data of the source file. */
	TArray<uint8> FileBytes;

	/** File mime type. */
	FString MimeType;

	virtual TSharedPtr<FJsonObject> ToJson() override;
	virtual FString ToUrlParams() override;
};

class VEAPI2_API FApi2FileLinkRequestMetadata final : public IApiMetadata {
public:
	FApi2FileLinkRequestMetadata();

	FString Id = FString();
	FString Type = FString();
	FString Url = FString();
	FString Mime = FString();
	int64 Size = 0;
	int64 Version = 0;
	FString Deployment = FString();
	FString Platform = FString();
	int32 Width = 0;
	int32 Height = 0;
	int64 Variant = 0;

	void SetFileType(const FString& InMimeType, const FString& InUrl);
	
	virtual TSharedPtr<FJsonObject> ToJson() override;
};

class VEAPI2_API FApi2FileUploadRequest final : public FApi2Request {
public:
	virtual bool Process() override;

	FApi2FileUploadRequestMetadata RequestMetadata;
	FApiFileMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

class VEAPI2_API FApi2FileUploadMemoryRequest final : public FApi2Request {
public:
	virtual bool Process() override;

	FApi2FileUploadMemoryRequestMetadata RequestMetadata;
	FApiFileMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

class VEAPI2_API FApi2FileLinkRequest final : public FApi2Request {
public:
	virtual bool Process() override;

	FApi2FileLinkRequestMetadata RequestMetadata;
	FApiFileMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};

class VEAPI2_API FApi2FileDeleteRequest final : public FApi2Request {
public:
	virtual bool Process() override;

	FGuid Id;
	FApiFileMetadata ResponseMetadata;

protected:
	virtual bool OnComplete(const FHttpRequestPtr& Request, const FHttpResponsePtr& Response, bool bSuccessful) override;
};


typedef TDelegate<void(const FApiFileBatchMetadata& /*InMetadata*/, const bool /*bInSuccessful*/, const FString& /*Error*/)> FOnFileBatchRequestCompleted;
typedef TDelegate<void(const FApiFileMetadata& /*InMetadata*/, const bool /*bInSuccessful*/, const FString& /*InError*/)> FOnFileRequestCompleted;
typedef TDelegate<void(const FApiFileMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError)> FOnFileRequestCompleted2;

class VEAPI2_API FApi2FileSubsystem final : public IModuleSubsystem {

public:
	static FName Name;

	virtual void Initialize() override;
	virtual void Shutdown() override;
	
	void Upload(const FApi2FileUploadRequestMetadata& InMetadata, const TSharedRef<FOnFileRequestCompleted> InCallback) const;
	void Upload2(const FApi2FileUploadRequestMetadata& InMetadata, const TSharedRef<FOnFileRequestCompleted2> InCallback) const;
	void UploadMemory(const FApi2FileUploadMemoryRequestMetadata& InMetadata, const TSharedRef<FOnFileRequestCompleted> InCallback) const;
	void Link(const FApi2FileLinkRequestMetadata& InMetadata, const TSharedRef<FOnFileRequestCompleted> InCallback) const;

	void Delete(const FGuid& Id, const TSharedRef<FOnGenericRequestCompleted2> InCallback) const;

	DECLARE_EVENT_ThreeParams(FApiFileUploadSubsystem, FOnFileUploaded, const FApiFileMetadata& /* Metadata */, const bool /* bSuccessful */, const FString& /* Error */);

};
