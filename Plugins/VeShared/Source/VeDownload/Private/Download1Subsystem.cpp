// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "Download1Subsystem.h"

#include "Download1.h"
#include "VeDownload.h"

const FName FDownloadDownload1Subsystem::Name = FName("FDownloadDownloadSubsystem");

void FDownloadDownload1Subsystem::Initialize() {}

void FDownloadDownload1Subsystem::Shutdown() {
	StopTick();

	[[maybe_unused]] FRWScopeLock Lock(ScheduledDownloadMapLock, SLT_ReadOnly);
	ScheduledDownloadMap.Empty();
}

void FDownloadDownload1Subsystem::StartTick() {
	VeLogTestFunc("enter");

	if (!TickerHandle.IsValid()) {
		TickerHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateRaw(this, &FDownloadDownload1Subsystem::Tick), TickInterval);
	}
}

bool FDownloadDownload1Subsystem::Tick(const float /*DeltaTime*/) {
	VeLogTestFunc("enter");

	[[maybe_unused]] FRWScopeLock Lock(ScheduledDownloadMapLock, SLT_ReadOnly);

	// Limit active downloads number.
	while (ActiveDownloadMap.Num() < MaxActiveDownloadNum) {
		VeLogTestFunc("selecting next scheduled download");

		FString SelectedDownloadKey = "";
		TSharedPtr<FVeDownload1> SelectedDownload = nullptr;

		if (ScheduledDownloadMap.Num() == 0) {
			VeLogTestFunc("no scheduled tasks");
			StopTick();
			return true;
		}

		if (ScheduledDownloadMap.Num() == 1) {
			VeLogTestFunc("only one scheduled task, no need to iterate");
			auto It = ScheduledDownloadMap.CreateConstIterator();
			SelectedDownloadKey = It.Key();
			SelectedDownload = It.Value();
		} else {
			VeLogTestFunc("iterate through scheduled downloads (%d) to select one with the highest priority", ScheduledDownloadMap.Num());
			for (const auto& ScheduledDownload : ScheduledDownloadMap) {
				if (ScheduledDownload.Value->IsProcessing()) {
					VeLogTestFunc("task is busy, already active: %s", *ScheduledDownload.Key);
					continue;
				}

				if (!SelectedDownload) {
					VeLogTestFunc("initial download: %s", *ScheduledDownload.Key);
					SelectedDownloadKey = ScheduledDownload.Key;
					SelectedDownload = ScheduledDownload.Value;
					if (SelectedDownload->GetPriority() == HighestPriority) {
						VeLogTestFunc("initial download has the top priority, selecting it: %s", *ScheduledDownload.Key);
						break;
					}

					continue;
				}

				if (SelectedDownload->GetPriority() == HighestPriority) {
					VeLogTestFunc("selecting a top priority task");
					break;
				}

				if (SelectedDownload->GetPriority() < ScheduledDownload.Value->GetPriority()) {
					VeLogTestFunc("re-selecting a higher priority task");
					SelectedDownloadKey = ScheduledDownload.Key;
					SelectedDownload = ScheduledDownload.Value;
				}
			}
		}

		if (!SelectedDownload) {
			// todo: check if this code block ever runs, can it be removed?
			ensure(0);
			VeLogTestFunc("no task selected, no more tasks scheduled");
			if (ScheduledDownloadMap.Num() == 0) {
				StopTick();
			}
		}

		// Move to the active download map.
		VeLogTestFunc("move download schedule");
		ActiveDownloadMap.Emplace(SelectedDownloadKey, SelectedDownload);
		ScheduledDownloadMap.Remove(SelectedDownloadKey);

		// Process different error codes
		if (SelectedDownload->Process() == EDownload1Code::Completed) {
			VeLogTestFunc("failed to process selected download, select the next one");
			continue;
		}

		VeLogTestFunc("start a selected download, exit loop");
		break;
	}

	return true;
}

void FDownloadDownload1Subsystem::StopTick() {
	VeLogTestFunc("enter");

	if (TickerHandle.IsValid()) {
		FTSTicker::GetCoreTicker().RemoveTicker(TickerHandle);
		TickerHandle.Reset();
	}
}

TSharedPtr<FVeDownload1> FDownloadDownload1Subsystem::Download(const FDownload1RequestMetadata& RequestMetadata) {
	VeLogTestFunc("enter");

	TSharedPtr<FVeDownload1> Download;

	{
		[[maybe_unused]] FRWScopeLock Lock(ScheduledDownloadMapLock, SLT_ReadOnly);
		// Check if there is already a download with the same URL
		if (ScheduledDownloadMap.Contains(RequestMetadata.Url)) {
			Download = ScheduledDownloadMap[RequestMetadata.Url];
			// Update priority
			Download->Restart(RequestMetadata.Priority);
			// Return existing download so the requester can subscribe to it
			return Download;
		}
	}

	{
		[[maybe_unused]] FRWScopeLock Lock(ScheduledDownloadMapLock, SLT_Write);
		// Create a new download
		ScheduledDownloadMap.Emplace(RequestMetadata.Url, MakeShared<FVeDownload1>(RequestMetadata));
		Download = ScheduledDownloadMap[RequestMetadata.Url];
		Download->GetOnDownloadComplete().AddRaw(this, &FDownloadDownload1Subsystem::OnDownloadCompleteCallback);
	}

	OnDownloadRequest.Broadcast(RequestMetadata);

	StartTick();

	return Download;
}

void FDownloadDownload1Subsystem::OnDownloadCompleteCallback(const FDownload1ResultMetadata& InMetadata) {
	VeLogTestFunc("enter");

	// Remove completed task
	{
		[[maybe_unused]] const FRWScopeLock Lock(ScheduledDownloadMapLock, SLT_Write);
		ActiveDownloadMap.Remove(InMetadata.RequestMetadata.Url);
	}
}
