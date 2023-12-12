// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class VEQRCODE_API FVeQRCodeModule : public IModuleInterface {
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static UTexture2D* GenerateQrCode(const FString Source, const FColor& Foreground = FColor::Black, const FColor& Background = FColor::White);
};
