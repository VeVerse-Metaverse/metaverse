// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once
#include "ApiFileUploadRequest.h"
#include "ApiObjectRequest.h"
#include "VeShared.h"

class VEAPI_API FApiFileUploadSubsystem final : public IModuleSubsystem {
public:
	const static FName Name;

	virtual void Initialize() override;
	virtual void Shutdown() override;

	DECLARE_EVENT_ThreeParams(FApiFileUploadSubsystem, FOnFileUploaded, const FApiFileMetadata& /* Metadata */, const bool /* bSuccessful */, const FString& /* Error */);

	/** Get a reference to the uploaded event. Called both for success and error cases. */
	FOnFileUploaded& GetOnUploaded() { return OnFileUploaded; }

	/**
	 * Requests the file upload.
	 * @param InMetadata Request metadata, including parent entity ID, file path.
	 * @param InCallback Complete callback delegate.
	 */
	bool Upload(const FApiUploadFileMetadata& InMetadata, TSharedPtr<FOnFileRequestCompleted> InCallback) const;
	/**
	 * Requests the file upload.
	 * @param InMetadata Request metadata, including parent entity ID, file path.
	 * @param InCallback Complete callback delegate.
	 */
	bool UploadMemory(const FApiUploadMemoryFileMetadata& InMetadata, TSharedPtr<FOnFileRequestCompleted> InCallback) const;

	/** Get a reference to the uploaded event. Called both for success and error cases. */
	FOnFileUploaded& GetOnReplace() { return OnFileUploaded; }

	/**
	 * Requests the file upload.
	 * @param InMetadata Request metadata, including parent entity ID, file path.
	 * @param InCallback Complete callback delegate.
	 */
	bool Replace(const FApiEntityFileLinkMetadata& InMetadata, TSharedPtr<FOnFileRequestCompleted> InCallback) const;

private:
	FOnFileUploaded OnFileUploaded;

	/** Notifies everyone about file upload events. Called both for success and error cases. */
	void BroadcastApiFileUploaded(const FApiFileMetadata& Metadata, bool bSuccessful, const FString& Error) const;

};

typedef TSharedPtr<FApiFileUploadSubsystem> FApiFileUploadSubsystemPtr;
