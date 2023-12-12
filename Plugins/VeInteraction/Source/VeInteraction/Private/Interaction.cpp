// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

// Copyright 2019-2020 Egor Pristavka. All Rights Reserved.

// #include "InteractionPrivatePCH.h"
#include "Interaction.h"
#include "Modules/ModuleManager.h"


#include "Enums/InteractionType.h"
#include "VisualLogger/VisualLoggerTypes.h"

DEFINE_LOG_CATEGORY(LogInteraction);
DEFINE_LOG_CATEGORY(LogVeInteraction);
DEFINE_LOG_CATEGORY(LogInteractionTest);

#define LOCTEXT_NAMESPACE "FVeInteractionModule"

void FVeInteractionModule::StartupModule() {
    UE_LOG(LogInteraction, Log, TEXT("Interaction module started."));
}

void FVeInteractionModule::ShutdownModule() {
    UE_LOG(LogInteraction, Log, TEXT("Interaction module shut down."));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_GAME_MODULE(FVeInteractionModule, VeInteraction);
