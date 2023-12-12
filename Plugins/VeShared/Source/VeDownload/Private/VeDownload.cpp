// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "VeDownload.h"

#include "Download1Subsystem.h"
#include "DownloadChunkSubsystem.h"
#include "DownloadTextureSubsystem.h"
#include "HttpDownloadSubsystem.h"
#include "FileStorageSubsystem.h"

DEFINE_LOG_CATEGORY(LogVeDownload);

IMPLEMENT_GAME_MODULE(FVeDownloadModule, VeDownload);

void FVeDownloadModule::StartupModule() {
	AddSubsystem(FDownloadChunkSubsystem::Name, MakeShared<FDownloadChunkSubsystem>());
	AddSubsystem(FDownloadTextureSubsystem::Name, MakeShared<FDownloadTextureSubsystem>());
	AddSubsystem(FDownloadDownload1Subsystem::Name, MakeShared<FDownloadDownload1Subsystem>());
	AddSubsystem(FDownloadHttpDownloadSubsystem::Name, MakeShared<FDownloadHttpDownloadSubsystem>());
	AddSubsystem(FDownloadFileStorageSubsystem::Name, MakeShared<FDownloadFileStorageSubsystem>());

	ISubsystemModule::StartupModule();
}

void FVeDownloadModule::ShutdownModule() {
	ISubsystemModule::ShutdownModule();
}

TSharedPtr<FDownloadChunkSubsystem> FVeDownloadModule::GetChunkSubsystem() const {
	return FindSubsystem<FDownloadChunkSubsystem>(FDownloadChunkSubsystem::Name);
}

TSharedPtr<FDownloadTextureSubsystem> FVeDownloadModule::GetTextureSubsystem() const {
	return FindSubsystem<FDownloadTextureSubsystem>(FDownloadTextureSubsystem::Name);
}

TSharedPtr<FDownloadDownload1Subsystem> FVeDownloadModule::GetDownload1Subsystem() const {
	return FindSubsystem<FDownloadDownload1Subsystem>(FDownloadDownload1Subsystem::Name);
}

TSharedPtr<FDownloadHttpDownloadSubsystem> FVeDownloadModule::GetHttpDownloadSubsystem() const {
	return FindSubsystem<FDownloadHttpDownloadSubsystem>(FDownloadHttpDownloadSubsystem::Name);
}

TSharedPtr<FDownloadFileStorageSubsystem> FVeDownloadModule::GetFileStorageSubsystem() const {
	return FindSubsystem<FDownloadFileStorageSubsystem>(FDownloadFileStorageSubsystem::Name);
}
