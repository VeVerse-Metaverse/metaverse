// Author: Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "Engine/DeveloperSettings.h"
#include "OpenSeaConfig.generated.h"

UCLASS(Config=OpenSeaConfig, defaultconfig, meta = (DisplayName = "OpenSea Config"))
class VEOPENSEA_API UOpenSeaConfig : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	static UOpenSeaConfig* Get()
	{
		return StaticClass()->GetDefaultObject<UOpenSeaConfig>();
	}

	const FString& GetXApiKey() const
	{
		return XApiKey;
	}

private:
	// TODO: Set it via editor.
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FString XApiKey{"bf90adf0105b44ba88580811d337a7c8"};
};
