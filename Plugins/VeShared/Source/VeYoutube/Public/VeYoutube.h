// Author: Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

VEYOUTUBE_API DECLARE_LOG_CATEGORY_EXTERN(LogVeYoutube, All, All);

UENUM(BlueprintType)
enum class EYoutubeVideoQuality : uint8 {
	Tiny UMETA(DisplayName="Tiny (144p)"),
	Small UMETA(DisplayName="Small (240p)"),
	Medium UMETA(DisplayName="Medium (360p)"),
	Large UMETA(DisplayName="Large (480p)"),
	Hd UMETA(DisplayName="HD (720p)"),
	FullHd UMETA(DisplayName="Full HD (1080p)"),
	None UMETA(DisplayName="None")
};

class FVeYoutubeModule : public IModuleInterface {
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
