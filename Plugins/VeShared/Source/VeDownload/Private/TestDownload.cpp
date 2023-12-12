// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "TestDownload.h"

#include "Download1.h"
#include "VeDownload.h"
#include "VeShared.h"
#include "Download1Subsystem.h"


// Sets default values
ATestDownload::ATestDownload() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATestDownload::BeginPlay() {
	Super::BeginPlay();
}

void ATestDownload::StartDownload() {
	// if (Download && Download->GetProgressMetadata().Progress < 1) {
		// return;
	// }

	GET_MODULE_SUBSYSTEM(DownloadSubsystem, Download, Download1);
	if (DownloadSubsystem) {
		FDownload1RequestMetadata RequestMetadata;
		RequestMetadata.Url = Url;
		RequestMetadata.Mode = EDownload1Mode::Overwrite;
		RequestMetadata.bUseRange = false;
		RequestMetadata.Priority = 0;
		Download = DownloadSubsystem->Download(RequestMetadata);
		Download->GetOnDownloadProgress().AddUObject(this, &ATestDownload::OnDownloadProgress);
		Download->GetOnDownloadComplete().AddUObject(this, &ATestDownload::OnDownloadComplete);
	}
}

// Called every frame
void ATestDownload::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}
