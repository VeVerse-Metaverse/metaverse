// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "VeOAuth.h"

#include "OAuthOAuthSubsystem.h"

#define LOCTEXT_NAMESPACE "FVeOAuthModule"

void FVeOAuthModule::StartupModule() {
	AddSubsystem(FOAuthOAuthSubsystem::Name, MakeShared<FOAuthOAuthSubsystem>());
	ISubsystemModule::StartupModule();
}

void FVeOAuthModule::ShutdownModule() {
	ISubsystemModule::ShutdownModule();
}

FVeOAuthModule* FVeOAuthModule::Get() {
	if (FModuleManager::Get().IsModuleLoaded(TEXT("VeOAuth"))) {
		return FModuleManager::Get().GetModulePtr<FVeOAuthModule>(TEXT("VeOAuth"));
	}
	return FModuleManager::Get().LoadModulePtr<FVeOAuthModule>(TEXT("VeOAuth"));
}

TSharedPtr<FOAuthOAuthSubsystem> FVeOAuthModule::GetOAuthSubsystem() {
	return FindSubsystem<FOAuthOAuthSubsystem>(FOAuthOAuthSubsystem::Name);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FVeOAuthModule, VeOAuth)
