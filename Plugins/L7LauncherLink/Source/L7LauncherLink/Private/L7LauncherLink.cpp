// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "L7LauncherLink.h"

DEFINE_LOG_CATEGORY(LogL7LauncherLink);

#define LOCTEXT_NAMESPACE "FL7LauncherLinkModule"

void FL7LauncherLinkModule::StartupModule() {
	// if (!IsRunningCommandlet() && !IsRunningDedicatedServer() && !IsRunningCookCommandlet()) {
	// 	ConnectionManager = MakeUnique<FL7LLSocketConnectionManager>(TEXT("127.0.0.1"), 13731);
	// 	if (ConnectionManager) {
	// 		ConnectionManager->StartConnection();
	// 	}
	// }
}

void FL7LauncherLinkModule::ShutdownModule() {
	// if (ConnectionManager) {
		// ConnectionManager.Reset();
	// }
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FL7LauncherLinkModule, L7LauncherLink)
