// Authors: Khusan Yadgarov, Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "VeApi2.h"

#include "Api2AiSubsystem.h"
#include "Api2AnalyticsSubsystem.h"
#include "Api2AppSubsystem.h"
#include "Api2AuthSubsystem.h"
#include "Api2FileSubsystem.h"
#include "Api2PackageSubsystem.h"
#include "Api2ObjectSubsystem.h"
#include "Api2OpenSeaSubsystem.h"
#include "Api2PersonaSubsystem.h"
#include "Api2PlaceableSubsystem.h"
#include "Api2WorldSubsystem.h"
#include "Api2UserSubsystem.h"
#include "Api2PortalSubsystem.h"
#include "Api2RatingSubsystem.h"
#include "Api2SharedAccessSubsystem.h"
#include "Api2StatsSubsystem.h"
#include "Api2ServerSubsystem.h"
#include "Api2SpeechRecognitionSubsystem.h"

#define LOCTEXT_NAMESPACE "FVeApi2Module"

DEFINE_LOG_CATEGORY(LogVeApi2);

void FVeApi2Module::StartupModule() {
	AddSubsystem(FApi2AuthSubsystem::Name, MakeShared<FApi2AuthSubsystem>());
	AddSubsystem(FApi2UserSubsystem::Name, MakeShared<FApi2UserSubsystem>());
	AddSubsystem(FApi2PackageSubsystem::Name, MakeShared<FApi2PackageSubsystem>());
	AddSubsystem(FApi2WorldSubsystem::Name, MakeShared<FApi2WorldSubsystem>());
	AddSubsystem(FApi2PortalSubsystem::Name, MakeShared<FApi2PortalSubsystem>());
	AddSubsystem(FApi2PlaceableSubsystem::Name, MakeShared<FApi2PlaceableSubsystem>());
	AddSubsystem(FApi2FileSubsystem::Name, MakeShared<FApi2FileSubsystem>());
	AddSubsystem(FApi2StatsSubsystem::Name, MakeShared<FApi2StatsSubsystem>());
	AddSubsystem(FApi2OpenSeaSubsystem::Name, MakeShared<FApi2OpenSeaSubsystem>());
	AddSubsystem(FApi2RatingSubsystem::Name, MakeShared<FApi2RatingSubsystem>());
	AddSubsystem(FApi2ObjectSubsystem::Name, MakeShared<FApi2ObjectSubsystem>());
	AddSubsystem(FApi2SharedAccessSubsystem::Name, MakeShared<FApi2SharedAccessSubsystem>());
	AddSubsystem(FApi2ServerSubsystem::Name, MakeShared<FApi2ServerSubsystem>());
	AddSubsystem(FApi2AnalyticsSubsystem::Name, MakeShared<FApi2AnalyticsSubsystem>());
	AddSubsystem(FApi2PersonaSubsystem::Name, MakeShared<FApi2PersonaSubsystem>());
	AddSubsystem(FApi2AppSubsystem::Name, MakeShared<FApi2AppSubsystem>());
	AddSubsystem(FApi2AiSubsystem::Name, MakeShared<FApi2AiSubsystem>());
	AddSubsystem(FApi2SpeechRecognitionSubsystem::Name, MakeShared<FApi2SpeechRecognitionSubsystem>());

	ISubsystemModule::StartupModule();
}

void FVeApi2Module::ShutdownModule() { }

FString FVeApi2Module::GetDeployment() {
	if (IsRunningDedicatedServer()) {
		return TEXT("Server");
	}
	return TEXT("Client");
}

FString FVeApi2Module::GetPlatform() {
	return FPlatformProcess::GetBinariesSubdirectory();
}

TSharedPtr<FApi2AuthSubsystem> FVeApi2Module::GetAuthSubsystem() const {
	return FindSubsystem<FApi2AuthSubsystem>(FApi2AuthSubsystem::Name);
}

TSharedPtr<FApi2UserSubsystem> FVeApi2Module::GetUserSubsystem() const {
	return FindSubsystem<FApi2UserSubsystem>(FApi2UserSubsystem::Name);
}

TSharedPtr<FApi2PackageSubsystem> FVeApi2Module::GetPackageSubsystem() const {
	return FindSubsystem<FApi2PackageSubsystem>(FApi2PackageSubsystem::Name);
}

TSharedPtr<FApi2FileSubsystem> FVeApi2Module::GetFileSubsystem() const {
	return FindSubsystem<FApi2FileSubsystem>(FApi2FileSubsystem::Name);
}

TSharedPtr<FApi2WorldSubsystem> FVeApi2Module::GetWorldSubsystem() const {
	return FindSubsystem<FApi2WorldSubsystem>(FApi2WorldSubsystem::Name);
}

TSharedPtr<FApi2PortalSubsystem> FVeApi2Module::GetPortalSubsystem() const {
	return FindSubsystem<FApi2PortalSubsystem>(FApi2PortalSubsystem::Name);
}

TSharedPtr<FApi2PlaceableSubsystem> FVeApi2Module::GetPlaceableSubsystem() const {
	return FindSubsystem<FApi2PlaceableSubsystem>(FApi2PlaceableSubsystem::Name);
}

TSharedPtr<FApi2StatsSubsystem> FVeApi2Module::GetStatsSubsystem() const {
	return FindSubsystem<FApi2StatsSubsystem>(FApi2StatsSubsystem::Name);
}

TSharedPtr<FApi2OpenSeaSubsystem> FVeApi2Module::GetOpenSeaSubsystem() const {
	return FindSubsystem<FApi2OpenSeaSubsystem>(FApi2OpenSeaSubsystem::Name);
}

TSharedPtr<FApi2RatingSubsystem> FVeApi2Module::GetRatingSubsystem() const {
	return FindSubsystem<FApi2RatingSubsystem>(FApi2RatingSubsystem::Name);
}

TSharedPtr<FApi2ObjectSubsystem> FVeApi2Module::GetObjectSubsystem() const {
	return FindSubsystem<FApi2ObjectSubsystem>(FApi2ObjectSubsystem::Name);
}

TSharedPtr<FApi2SharedAccessSubsystem> FVeApi2Module::GetSharedAccessSubsystem() const {
	return FindSubsystem<FApi2SharedAccessSubsystem>(FApi2SharedAccessSubsystem::Name);
}

TSharedPtr<FApi2ServerSubsystem> FVeApi2Module::GetServerSubsystem() const {
	return FindSubsystem<FApi2ServerSubsystem>(FApi2ServerSubsystem::Name);
}

TSharedPtr<FApi2AnalyticsSubsystem> FVeApi2Module::GetAnalyticsSubsystem() const {
	return FindSubsystem<FApi2AnalyticsSubsystem>(FApi2AnalyticsSubsystem::Name);
}

TSharedPtr<FApi2PersonaSubsystem> FVeApi2Module::GetPersonaSubsystem() const {
	return FindSubsystem<FApi2PersonaSubsystem>(FApi2PersonaSubsystem::Name);
}

TSharedPtr<FApi2SpeechRecognitionSubsystem> FVeApi2Module::GetSpeechRecognitionSubsystem() const {
	return FindSubsystem<FApi2SpeechRecognitionSubsystem>(FApi2SpeechRecognitionSubsystem::Name);
}

TSharedPtr<FApi2AppSubsystem> FVeApi2Module::GetAppSubsystem() const {
	return FindSubsystem<FApi2AppSubsystem>(FApi2AppSubsystem::Name);
}

TSharedPtr<FApi2AiSubsystem> FVeApi2Module::GetAiSubsystem() const {
	return FindSubsystem<FApi2AiSubsystem>(FApi2AiSubsystem::Name);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FVeApi2Module, VeApi2)
