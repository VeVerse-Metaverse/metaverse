// Author: Nikolay Bulatov, Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#include "VeLiveLinkLauncher.h"

#include "VeLiveLinkSourceFactory.h"
#include "VeLiveLink.h"


UVeLiveLinkLauncher::UVeLiveLinkLauncher() { }

UVeLiveLinkLauncher::~UVeLiveLinkLauncher() { }

void UVeLiveLinkLauncher::Tick(float DeltaTime) {
	if (!Source.IsValid()) {
		Source = UVeLiveLinkSourceFactory::CreateLiveLinkSource();
	}
}

void UVeLiveLinkLauncher::Destroy() {
	if (auto* VeLiveLinkModule = FModuleManager::GetModulePtr<FVeLiveLinkModule>("VeLiveLink")) {
		VeLiveLinkModule->DestroyLiveLinkStarter();
	}
}
