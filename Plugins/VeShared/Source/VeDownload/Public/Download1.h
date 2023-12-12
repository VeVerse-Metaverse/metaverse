// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "Download1Metadata.h"

class FVeDownload1Task;

/**
 * Scheduled download.
 * @details Manages internal asynchronous download task and provides callback interface for requesters. Safe to store in containers using smart pointers. Uses chunked download to download big files if server supports byte range requests.
 * @note Can have issues downloading files bigger than 2GiB using current UE HTTP implementation due to 31-bit TArray indexation.
 */
class FVeDownload1 {
	FDownload1RequestMetadata RequestMetadata;
	FDownload1ProgressMetadata ProgressMetadata;
	FDownload1ResultMetadata ResultMetadata;

	TSharedPtr<FVeDownload1Task> Task;

	bool bIsProcessing = false;
	bool bIsComplete = false;

	FOnDownload1Progress OnDownloadProgress;
	FOnDownload1Complete OnDownloadComplete;

	void OnTaskDownloadProgressCallback(const FDownload1ProgressMetadata& InMetadata);
	void OnTaskDownloadBlockCallback(const FDownload1ProgressMetadata& InMetadata);
	void OnTaskDownloadCompleteCallback(const FDownload1ResultMetadata& InMetadata);

	/** Cancels and clears the task */
	void ClearTask();

public:
	explicit FVeDownload1(const FDownload1RequestMetadata& InRequestMetadata);
	~FVeDownload1();

	FOnDownload1Progress& GetOnDownloadProgress() {
		return OnDownloadProgress;
	}

	FOnDownload1Complete& GetOnDownloadComplete() {
		return OnDownloadComplete;
	}

	const FDownload1RequestMetadata& GetRequestMetadata() const {
		return RequestMetadata;
	}

	const FDownload1ResultMetadata& GetResultMetadata() {
		return ResultMetadata;
	}

	const FDownload1ProgressMetadata& GetProgressMetadata() const {
		return ProgressMetadata;
	}

	uint32 GetPriority() const;
	bool IsProcessing() const;

	EDownload1Code Process();
	void Cancel(bool bDeleteFile = false) const;

	/** Reset busy status and update task priority. todo: test */
	void Restart(int32 NewPriority = 0);
};
