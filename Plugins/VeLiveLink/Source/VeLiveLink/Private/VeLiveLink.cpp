// Author: Nikolay Bulatov, Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#include "VeLiveLink.h"

#include "VeLiveLinkLauncher.h"

DEFINE_LOG_CATEGORY(LogVeLiveLink);

#define LOCTEXT_NAMESPACE "VeLiveLink"

void FVeLiveLinkModule::StartupModule() {
	if (!IsRunningDedicatedServer()) {
		LiveLinkLauncher = new UVeLiveLinkLauncher();
	}
}

void FVeLiveLinkModule::ShutdownModule() {
	DestroyLiveLinkStarter();
}

void FVeLiveLinkModule::DestroyLiveLinkStarter() {
	if (LiveLinkLauncher) {
		delete LiveLinkLauncher;
		LiveLinkLauncher = nullptr;
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FVeLiveLinkModule, VeLiveLink)
