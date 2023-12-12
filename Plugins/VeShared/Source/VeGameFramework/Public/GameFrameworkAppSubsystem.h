// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once
#include "VeShared.h"
#include "ApiFileMetadata.h"

typedef TDelegate<void(const FString& Url)> FOnGetAppImageFileUrlCompleted;

class VEGAMEFRAMEWORK_API FGameFrameworkAppSubsystem final : public IModuleSubsystem {
public:
	static FName Name;
	virtual void Initialize() override;
	virtual void Shutdown() override;

private:
	/**
	 * @brief App logo image file. Used as app icon, in loading screen, etc.
	 */
	FApiFileMetadata AppLogoImageFile{};

	/**
	 * @brief App placeholder image file. Used when entity has no image or requested image is not available.
	 */
	FApiFileMetadata AppPlaceholderLogoImageFile{};

	FString AppLogoImageUrl;
	FString AppPlaceholderLogoImageUrl;
	
	bool RequestComplete = false;

public:
	/**
	 * @brief Get app logo image file url. Used as app icon, in loading screen, etc.
	 * @param InCallback Callback
	 * @return true if request was sent
	 */
	bool GetAppLogoImageFileUrl(TSharedPtr<FOnGetAppImageFileUrlCompleted> InCallback);

	FString GetAppLogo() const { return AppLogoImageUrl; }

	/**
	 * @brief Pending app image file URL callback list. Filled when logo image file URL is requested.
	 */
	TArray<TSharedPtr<FOnGetAppImageFileUrlCompleted>> AppLogoImageFileUrlCallbacks;
	FCriticalSection AppLogoImageFileUrlCallbackLock;

	/**
	 * @brief Get app placeholder image file url. Used when entity has no image or requested image is not available.
	 * @param InCallback Callback
	 * @return true if request was sent
	 */
	bool GetAppPlaceholderImageFileUrl(TSharedPtr<FOnGetAppImageFileUrlCompleted> InCallback);

	FString GetAppPlaceholderLogo() const { return AppPlaceholderLogoImageUrl; }
	/**
	 * @brief Pending app image file URL callback list. Filled when placeholder image file URL is requested.
	 */
	TArray<TSharedPtr<FOnGetAppImageFileUrlCompleted>> AppPlaceholderImageFileUrlCallbacks;
	FCriticalSection AppPlaceholderImageFileUrlCallbackLock;

};
