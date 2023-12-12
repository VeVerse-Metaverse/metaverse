// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


#pragma once

enum class EVeWorldState : uint8 {
	None,

	Error,
	Error_Unavailable,
	Error_DownloadFail,
	Error_MountFail,

	PendingDownloadPackage,
	// StartDownloadPackage()
	PackageDownloading,
	PackageDownloaded,
	// MountPackage()
	PackageMounted,
	// StartMatchingServer()
	ServerMatching,
	ServerOnline,
	// StartTeleportWorld()
	Teleporting
};

enum class EVePackageState : uint8 {
	None,
	Error,

	/** Package not available. */
	Error_Unavailable,

	/** Failed to download package file. */
	Error_DownloadFail,

	/** Failed to mount package. */
	Error_MountFail,
	
	/** Package file not downloaded and not mounted. */
	NotDownloaded,

	/** Package file downloaded, but not mounted. */
	Downloaded,

	/** Package file mounted. */
	Mounted
};

enum class EVeMatchingServerState : uint8 {
	None,
	Error,
	Offline,
	WaitServer,
	Online
};

struct FVeDownloadPackageFile {
	FGuid Id;
	int64 Size = 0;
	FString Url;
};

struct FVeDownloadPackageList {
	TArray<FVeDownloadPackageFile> ExtraContentFiles;
	TArray<FVeDownloadPackageFile> PakFiles;
	int64 TotalSize = 0;
	int64 DownloadedSize = 0;
	int32 Attempts = 0;
};

typedef TDelegate<void(EVePackageState InState)> FOnPackageState;
typedef TDelegate<void(EVeMatchingServerState InState)> FOnMatchingServerState;
typedef TDelegate<void(bool InState)> FOnOpenWorldState;
