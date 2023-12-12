// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#include "VeSocial.h"

#include "Modules/ModuleManager.h"
#include "SocialFriendsSubsystem.h"
#include "SocialPresenceSubsystem.h"

DEFINE_LOG_CATEGORY(LogVeSocial);

IMPLEMENT_GAME_MODULE(FVeSocialModule, VeSocial);


void FVeSocialModule::StartupModule() {
	AddSubsystem(FSocialFriendsSubsystem::Name, MakeShared<FSocialFriendsSubsystem>());
	
	ISubsystemModule::StartupModule();
}

void FVeSocialModule::ShutdownModule() {
	ISubsystemModule::ShutdownModule();
}

FVeSocialModule* FVeSocialModule::Get() {
	if (FModuleManager::Get().IsModuleLoaded(TEXT("VeSocial"))) {
		return FModuleManager::Get().GetModulePtr<FVeSocialModule>(TEXT("VeSocial"));
	}
	return FModuleManager::Get().LoadModulePtr<FVeSocialModule>(TEXT("VeSocial"));
}

TSharedPtr<FSocialFriendsSubsystem> FVeSocialModule::GetFriendsSubsystem() {
	return FindSubsystem<FSocialFriendsSubsystem>(FSocialFriendsSubsystem::Name);
}

TSharedPtr<FSocialPresenceSubsystem> FVeSocialModule::GetPresenceSubsystem() {
	return FindSubsystem<FSocialPresenceSubsystem>(FSocialPresenceSubsystem::Name);
}
