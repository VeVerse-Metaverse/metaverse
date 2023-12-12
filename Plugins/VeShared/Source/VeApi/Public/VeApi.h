// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "ApiAuthSubsystem.h"
#include "ApiEventSubsystem.h"
#include "ApiFileUploadSubsystem.h"
#include "ApiObjectSubsystem.h"
#include "ApiPersonaSubsystem.h"
#include "ApiPlaceableSubsystem.h"
#include "ApiPortalSubsystem.h"
#include "ApiServerSubsystem.h"
#include "ApiSpaceSubsystem.h"
#include "ApiTemplateSubsystem.h"
#include "ApiUserSubsystem.h"
#include "VeShared.h"

VEAPI_API DECLARE_LOG_CATEGORY_EXTERN(LogVeApi, All, All);

class VEAPI_API FVeApiModule final : public ISubsystemModule {
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static FVeApiModule* Get() {
		if (FModuleManager::Get().IsModuleLoaded(TEXT("VeApi"))) {
			return FModuleManager::GetModulePtr<FVeApiModule>(TEXT("VeApi"));
		}

		if (GIsRunning) {
			return FModuleManager::LoadModulePtr<FVeApiModule>(TEXT("VeApi"));
		}

		return nullptr;
	}

	FApiAuthSubsystemPtr GetAuthSubsystem() {
		return FindSubsystem<FApiAuthSubsystem>(FApiAuthSubsystem::Name);
	}

	FApiSpaceSubsystemPtr GetSpaceSubsystem() {
		return FindSubsystem<FApiSpaceSubsystem>(FApiSpaceSubsystem::Name);
	}

	FApiPlaceableSubsystemPtr GetPlaceableSubsystem() {
		return FindSubsystem<FApiPlaceableSubsystem>(FApiPlaceableSubsystem::Name);
	}

	FApiTemplateSubsystemPtr GetTemplateSubsystem() {
		return FindSubsystem<FApiTemplateSubsystem>(FApiTemplateSubsystem::Name);
	}

	FApiUserSubsystemPtr GetUserSubsystem() {
		return FindSubsystem<FApiUserSubsystem>(FApiUserSubsystem::Name);
	}

	FApiObjectSubsystemPtr GetObjectSubsystem() {
		return FindSubsystem<FApiObjectSubsystem>(FApiObjectSubsystem::Name);
	}

	FApiFileUploadSubsystemPtr GetFileUploadSubsystem() {
		return FindSubsystem<FApiFileUploadSubsystem>(FApiFileUploadSubsystem::Name);
	}

	FApiPersonaSubsystemPtr GetPersonaSubsystem() {
		return FindSubsystem<FApiPersonaSubsystem>(FApiPersonaSubsystem::Name);
	}

	FApiServerSubsystemPtr GetServerSubsystem() {
		return FindSubsystem<FApiServerSubsystem>(FApiServerSubsystem::Name);
	}
	
	FApiPortalSubsystemPtr GetPortalSubsystem() {
		return FindSubsystem<FApiPortalSubsystem>(FApiPortalSubsystem::Name);
	}

	FApiEventSubsystemPtr GetEventSubsystem() {
		return FindSubsystem<FApiEventSubsystem>(FApiEventSubsystem::Name);
	}
	
	TSharedPtr<class FApiPropertySubsystem> GetPropertySubsystem() const;

};
