// Author: Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#include "VeYoutube.h"

#define LOCTEXT_NAMESPACE "FVeYoutubeModule"
VEYOUTUBE_API DEFINE_LOG_CATEGORY(LogVeYoutube);

void FVeYoutubeModule::StartupModule()
{
}

void FVeYoutubeModule::ShutdownModule()
{
    
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FVeYoutubeModule, VeYoutube)