// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "VeShared.h"
#include "Modules/ModuleManager.h"

VEAPI2_API DECLARE_LOG_CATEGORY_EXTERN(LogVeApi2, All, All);

class VEAPI2_API FVeApi2Module final : public ISubsystemModule {
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static FVeApi2Module* Get() {
		if (FModuleManager::Get().IsModuleLoaded(TEXT("VeApi2"))) {
			return FModuleManager::GetModulePtr<FVeApi2Module>(TEXT("VeApi2"));
		}

		if (GIsRunning) {
			return FModuleManager::LoadModulePtr<FVeApi2Module>(TEXT("VeApi2"));
		}

		return nullptr;
	}

	static FString GetDeployment();
	static FString GetPlatform();

	TSharedPtr<class FApi2AuthSubsystem> GetAuthSubsystem() const;
	TSharedPtr<class FApi2UserSubsystem> GetUserSubsystem() const;
	TSharedPtr<class FApi2PackageSubsystem> GetPackageSubsystem() const;
	TSharedPtr<class FApi2FileSubsystem> GetFileSubsystem() const;
	TSharedPtr<class FApi2WorldSubsystem> GetWorldSubsystem() const;
	TSharedPtr<class FApi2PortalSubsystem> GetPortalSubsystem() const;
	TSharedPtr<class FApi2PlaceableSubsystem> GetPlaceableSubsystem() const;
	TSharedPtr<class FApi2StatsSubsystem> GetStatsSubsystem() const;
	TSharedPtr<class FApi2OpenSeaSubsystem> GetOpenSeaSubsystem() const;
	TSharedPtr<class FApi2RatingSubsystem> GetRatingSubsystem() const;
	TSharedPtr<class FApi2ObjectSubsystem> GetObjectSubsystem() const;
	TSharedPtr<class FApi2SharedAccessSubsystem> GetSharedAccessSubsystem() const;
	TSharedPtr<class FApi2ServerSubsystem> GetServerSubsystem() const;
	TSharedPtr<class FApi2AnalyticsSubsystem> GetAnalyticsSubsystem() const;
	TSharedPtr<class FApi2AppSubsystem> GetAppSubsystem() const;
	TSharedPtr<class FApi2AiSubsystem> GetAiSubsystem() const;
	TSharedPtr<class FApi2PersonaSubsystem> GetPersonaSubsystem() const;
	TSharedPtr<class FApi2SpeechRecognitionSubsystem> GetSpeechRecognitionSubsystem() const;
};
