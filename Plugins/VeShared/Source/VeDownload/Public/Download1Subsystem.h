// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once
#include "Download1Metadata.h"
#include "VeShared.h"

class FVeDownload1;

/** Download subsystem manages parallel file downloads */
class VEDOWNLOAD_API FDownloadDownload1Subsystem final : public IModuleSubsystem {

public:
	static const FName Name;

	virtual void Initialize() override;
	virtual void Shutdown() override;

private:
#pragma region Events
	/** Download request event */
	FOnDownload1Request OnDownloadRequest;
	/** Download progress event */
	FOnDownload1Progress OnDownloadProgress;
	/** Download complete event */
	FOnDownload1Complete OnDownloadComplete;

public:
	/** Download request event */
	FOnDownload1Request& GetOnDownloadRequest() {
		return OnDownloadRequest;
	}

	/** Download progress event */
	FOnDownload1Progress& GetOnDownloadProgress() {
		return OnDownloadProgress;
	}

	/** Download complete event */
	FOnDownload1Complete& GetOnDownloadComplete() {
		return OnDownloadComplete;
	}
#pragma endregion

	/** Schedule file download */
	TSharedPtr<FVeDownload1> Download(const FDownload1RequestMetadata& RequestMetadata);

#pragma region Tick
private:
	/** Tick interval in seconds, how often to check and schedule download tasks */
	const float TickInterval = 1.0f;
	/** Ticker delegate handle */
	FTSTicker::FDelegateHandle TickerHandle;
	/** Start ticker */
	void StartTick();
	/** Updates asynchronous tasks */
	bool Tick(float DeltaTime = 0.0f);
	/** Stops ticker */
	void StopTick();
#pragma endregion

	/** Highest download priority. */
	static constexpr uint32 HighestPriority = static_cast<uint32>(-1);

	/** Callback for the download complete events, removes the completed download from the container. */
	void OnDownloadCompleteCallback(const FDownload1ResultMetadata& InMetadata);

	/** Scheduled downloads. */
	TMap<FString, TSharedPtr<FVeDownload1>> ScheduledDownloadMap;
	/** Scheduled downloads container lock */
	FRWLock ScheduledDownloadMapLock;

	/** Active downloads. */
	TMap<FString, TSharedPtr<FVeDownload1>> ActiveDownloadMap;
	/** Scheduled downloads container lock */
	FRWLock ActiveDownloadMapLock;

	/** How many parallel download tasks are allowed. */
	const int32 MaxActiveDownloadNum = 10;
};
