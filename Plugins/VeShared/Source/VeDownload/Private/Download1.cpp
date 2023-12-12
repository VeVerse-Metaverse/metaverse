// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "Download1.h"

#include "Download1Task.h"
#include "VeDownload.h"
#include "VeShared.h"

void FVeDownload1::OnTaskDownloadProgressCallback(const FDownload1ProgressMetadata& InMetadata) {
	VeLogTestFunc("enter");

	ProgressMetadata = InMetadata;
	// Notify subscribers on progress update
	OnDownloadProgress.Broadcast(ProgressMetadata);
}

void FVeDownload1::OnTaskDownloadBlockCallback(const FDownload1ProgressMetadata& InMetadata) {
	VeLogTestFunc("enter");

	// Notify subscribers on progress update
	ProgressMetadata = InMetadata;
	OnDownloadProgress.Broadcast(ProgressMetadata);

	// Current chunk download has completed, so unlock the download
	bIsProcessing = false;
}

void FVeDownload1::OnTaskDownloadCompleteCallback(const FDownload1ResultMetadata& InMetadata) {
	VeLogTestFunc("enter");

	// Notify subscribers that download has been completed
	ResultMetadata = InMetadata;
	OnDownloadComplete.Broadcast(InMetadata);

	bIsProcessing = false;
	bIsComplete = true;

	ClearTask();
}

FVeDownload1::FVeDownload1(const FDownload1RequestMetadata& InRequestMetadata) {
	VeLogTestFunc("enter");

	RequestMetadata = InRequestMetadata;
	ProgressMetadata = {};
	ResultMetadata = {};
	Task = MakeShared<FVeDownload1Task>(RequestMetadata);
	Task->GetOnDownloadProgress().BindRaw(this, &FVeDownload1::OnTaskDownloadProgressCallback);
	Task->GetOnDownloadChunk().BindRaw(this, &FVeDownload1::OnTaskDownloadBlockCallback);
	Task->GetOnDownloadComplete().BindRaw(this, &FVeDownload1::OnTaskDownloadCompleteCallback);
}

FVeDownload1::~FVeDownload1() {
	VeLogTestFunc("enter");

	Cancel();
	ClearTask();
}

EDownload1Code FVeDownload1::Process() {
	VeLogTestFunc("enter");

	return Task->Process();
}

void FVeDownload1::Cancel(const bool bDeleteFile) const {
	VeLogTestFunc("enter");

	if (Task) {
		Task->Cancel(bDeleteFile);
	}
}

uint32 FVeDownload1::GetPriority() const {
	VeLogTestFunc("enter");

	return RequestMetadata.Priority;
}

bool FVeDownload1::IsProcessing() const {
	VeLogTestFunc("enter");

	return bIsProcessing;
}

void FVeDownload1::Restart(const int32 NewPriority) {
	VeLogTestFunc("enter");

	RequestMetadata.Priority = NewPriority;
	bIsProcessing = false;
}

void FVeDownload1::ClearTask() {
	VeLogTestFunc("task: %p", Task.Get());
	if (Task) {
		Task->GetOnDownloadProgress().Unbind();
		Task->GetOnDownloadChunk().Unbind();
		Task->GetOnDownloadComplete().Unbind();
		Task.Reset();
	}
}
