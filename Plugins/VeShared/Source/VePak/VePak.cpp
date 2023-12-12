// Author: Egor A. Pristavka

#include "VePak.h"

DEFINE_LOG_CATEGORY(LogVePak);

IMPLEMENT_GAME_MODULE(FVePakModule, VePak);

void FVePakModule::StartupModule() {
	AddSubsystem(FPakPakSubsystem::Name, MakeShared<FPakPakSubsystem>());

	ISubsystemModule::StartupModule();
}

void FVePakModule::ShutdownModule() {
	ISubsystemModule::ShutdownModule();
}

FVePakModule* FVePakModule::Get() {
	if (FModuleManager::Get().IsModuleLoaded(TEXT("VePak"))) {
		return FModuleManager::Get().GetModulePtr<FVePakModule>(TEXT("VePak"));
	}
	return FModuleManager::Get().LoadModulePtr<FVePakModule>(TEXT("VePak"));
}

FPakSubsystemPtr FVePakModule::GetPakSubsystem() {
	return FindSubsystem<FPakPakSubsystem>(FPakPakSubsystem::Name);
}
