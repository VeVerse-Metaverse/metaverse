// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "VeRpc.h"

#include "Modules/ModuleManager.h"
#include "RpcWebSocketsServiceSubsystem.h"
#include "RpcWebSocketsTextChatSubsystem.h"

DEFINE_LOG_CATEGORY(LogVeRpc);

IMPLEMENT_GAME_MODULE(FVeRpcModule, VeRpc);

void FVeRpcModule::StartupModule() {
	AddSubsystem(FRpcWebSocketsServiceSubsystem::Name, MakeShared<FRpcWebSocketsServiceSubsystem>());
	AddSubsystem(FRpcWebSocketsTextChatSubsystem::Name, MakeShared<FRpcWebSocketsTextChatSubsystem>());
	ISubsystemModule::StartupModule();
}

void FVeRpcModule::ShutdownModule() {
	ISubsystemModule::ShutdownModule();
}

FVeRpcModule* FVeRpcModule::Get() {
	if (FModuleManager::Get().IsModuleLoaded(TEXT("VeRpc"))) {
		return FModuleManager::Get().GetModulePtr<FVeRpcModule>(TEXT("VeRpc"));
	}
	return FModuleManager::Get().LoadModulePtr<FVeRpcModule>(TEXT("VeRpc"));
}

TSharedPtr<FRpcWebSocketsServiceSubsystem> FVeRpcModule::GetWebSocketsServiceSubsystem() {
	return FindSubsystem<FRpcWebSocketsServiceSubsystem>(FRpcWebSocketsServiceSubsystem::Name);
}

TSharedPtr<FRpcWebSocketsTextChatSubsystem> FVeRpcModule::GetWebSocketsTextChatSubsystem() {
	return FindSubsystem<FRpcWebSocketsTextChatSubsystem>(FRpcWebSocketsTextChatSubsystem::Name);
}
