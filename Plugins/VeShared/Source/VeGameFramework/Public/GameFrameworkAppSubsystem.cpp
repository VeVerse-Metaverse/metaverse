// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "GameFrameworkAppSubsystem.h"

#include "Api2AppSubsystem.h"
#include "VeApi2.h"
#include "VeConfigLibrary.h"
#include "ApiFileMetadata.h"

FName FGameFrameworkAppSubsystem::Name = "GameFrameworkApp";

void FGameFrameworkAppSubsystem::Initialize() {}

void FGameFrameworkAppSubsystem::Shutdown() {}

bool FGameFrameworkAppSubsystem::GetAppLogoImageFileUrl(const TSharedPtr<FOnGetAppImageFileUrlCompleted> InCallback) {
	if (RequestComplete) {
		InCallback->ExecuteIfBound(AppLogoImageFile.Url);
		return true;
	}

	const FGuid AppId = FVeConfigLibrary::GetAppId();
	if (!AppId.IsValid()) {
		LogErrorF("failed to get app id");
		return false;
	}

	GET_MODULE_SUBSYSTEM(AppSubsystem, Api2, App);
	if (!AppSubsystem) {
		LogErrorF("failed to get app subsystem");
		return false;
	}

	{
		// ReSharper disable once CppLocalVariableWithNonTrivialDtorIsNeverUsed
		FScopeLock Lock(&AppLogoImageFileUrlCallbackLock);
		AppLogoImageFileUrlCallbacks.Add(InCallback);
		if (AppLogoImageFileUrlCallbacks.Num() > 1) {
			// If we already have a request in flight, just add the callback to the list.
			return true;
		}
	}

	const auto CallbackPtr = MakeShared<FOnApi2AppIdentityImageRequestCompleted>();
	CallbackPtr->BindLambda([this](const FApi2AppIdentityImageResponseMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError) mutable {
		// ReSharper disable once CppLocalVariableWithNonTrivialDtorIsNeverUsed
		FScopeLock Lock(&AppLogoImageFileUrlCallbackLock);

		RequestComplete = true;

		if (InResponseCode != EApi2ResponseCode::Ok) {
			LogErrorF("failed to request: %s", *InError);
			for (const auto& Callback : AppLogoImageFileUrlCallbacks) {
				Callback->ExecuteIfBound({});
			}
			AppLogoImageFileUrlCallbacks.Empty();
			return;
		}

		if (InMetadata.Files.Num() == 0) {
			LogWarningF("app has no logo files");
			for (const auto& Callback : AppLogoImageFileUrlCallbacks) {
				Callback->ExecuteIfBound({});
			}
			AppLogoImageFileUrlCallbacks.Empty();
			return;
		}

		for (const auto& File : InMetadata.Files) {
			if (File.Type == EApiFileType::AppLogoImage) {
				AppLogoImageFile = File;
				AppLogoImageUrl = AppLogoImageFile.Url;
				break;
			}
		}

		for (const auto& File : InMetadata.Files) {
			if (File.Type == EApiFileType::AppPlaceholderImage) {
				AppPlaceholderLogoImageFile = File;
				AppPlaceholderLogoImageUrl = AppPlaceholderLogoImageFile.Url;
				break;
			}
		}

		for (const auto& Callback : AppLogoImageFileUrlCallbacks) {
			Callback->ExecuteIfBound(AppLogoImageFile.Url);
		}
		AppLogoImageFileUrlCallbacks.Empty();
	});

	AppSubsystem->GetAppIdentityImages(AppId, CallbackPtr);

	return true;
}

bool FGameFrameworkAppSubsystem::GetAppPlaceholderImageFileUrl(const TSharedPtr<FOnGetAppImageFileUrlCompleted> InCallback) {
	if (RequestComplete) {
		InCallback->ExecuteIfBound(AppPlaceholderLogoImageFile.Url);
		return true;
	}

	const FGuid AppId = FVeConfigLibrary::GetAppId();
	if (!AppId.IsValid()) {
		LogErrorF("failed to get app id");
		return false;
	}

	GET_MODULE_SUBSYSTEM(AppSubsystem, Api2, App);
	if (!AppSubsystem) {
		LogErrorF("failed to get app subsystem");
		return false;
	}

	{
		// ReSharper disable once CppLocalVariableWithNonTrivialDtorIsNeverUsed
		FScopeLock Lock(&AppPlaceholderImageFileUrlCallbackLock);
		AppPlaceholderImageFileUrlCallbacks.Add(InCallback);
		if (AppPlaceholderImageFileUrlCallbacks.Num() > 1) {
			// If we already have a request in flight, just add the callback to the list.
			return true;
		}
	}

	const auto CallbackPtr = MakeShared<FOnApi2AppIdentityImageRequestCompleted>();
	CallbackPtr->BindLambda([this](const FApi2AppIdentityImageResponseMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError) mutable {
		// ReSharper disable once CppLocalVariableWithNonTrivialDtorIsNeverUsed
		FScopeLock Lock(&AppPlaceholderImageFileUrlCallbackLock);

		RequestComplete = true;

		if (InResponseCode != EApi2ResponseCode::Ok) {
			LogErrorF("failed to request: %s", *InError);
			for (const auto& Callback : AppPlaceholderImageFileUrlCallbacks) {
				Callback->ExecuteIfBound({});
			}
			AppPlaceholderImageFileUrlCallbacks.Empty();
			return;
		}

		if (InMetadata.Files.Num() == 0) {
			LogWarningF("app has no logo files");
			for (const auto& Callback : AppPlaceholderImageFileUrlCallbacks) {
				Callback->ExecuteIfBound({});
			}
			AppPlaceholderImageFileUrlCallbacks.Empty();
			return;
		}

		for (const auto& File : InMetadata.Files) {
			if (File.Type == EApiFileType::AppLogoImage) {
				AppLogoImageFile = File;
				AppLogoImageUrl = AppLogoImageFile.Url;
				break;
			}
		}

		for (const auto& File : InMetadata.Files) {
			if (File.Type == EApiFileType::AppPlaceholderImage) {
				AppPlaceholderLogoImageFile = File;
				AppPlaceholderLogoImageUrl = AppPlaceholderLogoImageFile.Url;
				break;
			}
		}

		LogF("calling callback for app placeholder image file url: %s", *AppPlaceholderLogoImageFile.Url);
		for (const auto& Callback : AppPlaceholderImageFileUrlCallbacks) {
			Callback->ExecuteIfBound(AppPlaceholderLogoImageFile.Url);
		}
		AppPlaceholderImageFileUrlCallbacks.Empty();
	});

	AppSubsystem->GetAppIdentityImages(AppId, CallbackPtr);

	return true;
}
