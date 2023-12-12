// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


#include "FileDownloadComponent.h"

#include "Download1.h"
#include "Download1Metadata.h"
#include "Download1Subsystem.h"
#include "VeShared.h"
#include "VeDownload.h"

UFileDownloadComponent::UFileDownloadComponent() {
	PrimaryComponentTick.bCanEverTick = false;
}

void UFileDownloadComponent::StartDownload(const FString& Url, const FOnDownloadProgress& OnDownloadProgress, const FOnDownloadComplete& OnDownloadComplete) {
	GET_MODULE_SUBSYSTEM(DownloadSubsystem, Download, Download1);
	if (DownloadSubsystem) {
		FDownload1RequestMetadata RequestMetadata;
		RequestMetadata.Url = Url;
		RequestMetadata.Mode = EDownload1Mode::Overwrite;
		RequestMetadata.bUseRange = false;
		RequestMetadata.Priority = 0;
		Download = DownloadSubsystem->Download(RequestMetadata);
		Download->GetOnDownloadProgress().AddWeakLambda(this, [OnDownloadProgress](const FDownload1ProgressMetadata& Metadata) {
			OnDownloadProgress.ExecuteIfBound(Metadata);
		});
		Download->GetOnDownloadComplete().AddWeakLambda(this, [OnDownloadComplete](const FDownload1ResultMetadata& Metadata) {
			OnDownloadComplete.ExecuteIfBound(Metadata);
		});
	}
}

void UFileDownloadComponent::StartDownload(const FString& Url, const FOnDownloadProgressBP& OnDownloadProgressBP, const FOnDownloadCompleteBP& OnDownloadCompleteBP) {
	GET_MODULE_SUBSYSTEM(DownloadSubsystem, Download, Download1);
	if (DownloadSubsystem) {
		FDownload1RequestMetadata RequestMetadata;
		RequestMetadata.Url = Url;
		RequestMetadata.Mode = EDownload1Mode::Overwrite;
		RequestMetadata.bUseRange = false;
		RequestMetadata.Priority = 0;
		Download = DownloadSubsystem->Download(RequestMetadata);
		Download->GetOnDownloadProgress().AddWeakLambda(this, [OnDownloadProgressBP](const FDownload1ProgressMetadata& Metadata) {
			OnDownloadProgressBP.ExecuteIfBound(Metadata);
		});
		Download->GetOnDownloadComplete().AddWeakLambda(this, [OnDownloadCompleteBP](const FDownload1ResultMetadata& Metadata) {
			OnDownloadCompleteBP.ExecuteIfBound(Metadata);
		});
	}
}
