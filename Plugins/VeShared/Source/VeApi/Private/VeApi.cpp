// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#include "VeApi.h"

#include "ApiAuthSubsystem.h"
#include "ApiObjectSubsystem.h"
#include "ApiPropertySubsystem.h"
#include "Modules/ModuleManager.h"

DEFINE_LOG_CATEGORY(LogVeApi);

IMPLEMENT_GAME_MODULE(FVeApiModule, VeApi);

void FVeApiModule::StartupModule() {
	// 	if (GConfig) {
	// #if UE_EDITOR
	// 		GConfig->GetString(TEXT("VeApi"), TEXT("EditorKey"), EditorKey, GEditorIni);
	// #endif
	//
	// #if UE_SERVER
	// 		GConfig->GetString(TEXT("VeApi"), TEXT("ServerKey"), ServerKey, GGameIni);
	// #endif
	// 	}
	AddSubsystem(FApiAuthSubsystem::Name, MakeShared<FApiAuthSubsystem>());
	AddSubsystem(FApiEventSubsystem::Name, MakeShared<FApiEventSubsystem>());
	AddSubsystem(FApiPersonaSubsystem::Name, MakeShared<FApiPersonaSubsystem>());
	AddSubsystem(FApiSpaceSubsystem::Name, MakeShared<FApiSpaceSubsystem>());
	AddSubsystem(FApiPlaceableSubsystem::Name, MakeShared<FApiPlaceableSubsystem>());
	AddSubsystem(FApiTemplateSubsystem::Name, MakeShared<FApiTemplateSubsystem>());
	AddSubsystem(FApiUserSubsystem::Name, MakeShared<FApiUserSubsystem>());
	AddSubsystem(FApiObjectSubsystem::Name, MakeShared<FApiObjectSubsystem>());
	AddSubsystem(FApiFileUploadSubsystem::Name, MakeShared<FApiFileUploadSubsystem>());
	AddSubsystem(FApiServerSubsystem::Name, MakeShared<FApiServerSubsystem>());
	AddSubsystem(FApiPortalSubsystem::Name, MakeShared<FApiPortalSubsystem>());
	AddSubsystem(FApiPropertySubsystem::Name, MakeShared<FApiPropertySubsystem>());

	ISubsystemModule::StartupModule();
}

void FVeApiModule::ShutdownModule() {
	ISubsystemModule::ShutdownModule();
}

TSharedPtr<FApiPropertySubsystem> FVeApiModule::GetPropertySubsystem() const {
	return FindSubsystem<FApiPropertySubsystem>(FApiPropertySubsystem::Name);
}

//
// FString FVeApiModule::GetApiKey() {
// #if UE_EDITOR
// 	if (ApiUser.ApiKey.IsEmpty()) {
// 		ApiUser.ApiKey = EditorKey;
// 	}
// #endif
//
// #if UE_SERVER
// 	if (ApiUser.ApiKey.IsEmpty()) {
// 		ApiUser.ApiKey = ServerKey;
// 	}
// #endif
//
// 	return ApiUser.ApiKey;
// }
