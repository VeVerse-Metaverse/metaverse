// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "VeAI.h"

#include "CogAiDetailsCustomization.h"

DEFINE_LOG_CATEGORY(LogVeAI);

#define LOCTEXT_NAMESPACE "FVeAIModule"

void FVeAIModule::StartupModule() {
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

#if WITH_EDITORONLY_DATA
	RegisterCogAiDetailsCustomization();
#endif
}

void FVeAIModule::ShutdownModule() {
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

#if WITH_EDITORONLY_DATA
	UnregisterCogAiDetailsCustomization();
#endif
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FVeAIModule, VeAI)
