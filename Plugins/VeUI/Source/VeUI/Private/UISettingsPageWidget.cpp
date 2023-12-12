// Authors: Khusan T.Yadgarov, Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UISettingsPageWidget.h"

#include "Api2Request.h"
#include "Api2UserSubsystem.h"
#include "UINotificationData.h"
#include "Components/UIButtonWidget.h"
#include "Components/UICommonOptionButtonsWidget.h"
#include "Components/UICommonOptionSliderWidget.h"
#include "Components/UICommonOptionSwitcherWidget.h"
#include "UIPageManagerWidget.h"
#include "VeApi2.h"
#include "VeShared.h"
#include "VeUI.h"
#include "GameFramework/GameUserSettings.h"
#include "Subsystems/UINotificationSubsystem.h"

#define LOCTEXT_NAMESPACE "VeUI"


void UUISettingsPageWidget::RegisterCallbacks() {
	Super::RegisterCallbacks();

	if (!OnSubmitButtonClickedDelegateHandle.IsValid()) {
		if (IsValid(SubmitButtonWidget)) {
			OnSubmitButtonClickedDelegateHandle = SubmitButtonWidget->GetOnButtonClicked().AddUObject(this, &UUISettingsPageWidget::OnSubmitButtonClicked);
		}
	}

	if (!OnCancelButtonClickedDelegateHandle.IsValid()) {
		if (IsValid(CancelButtonWidget)) {
			OnCancelButtonClickedDelegateHandle = CancelButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
				GetPageManager()->ShowPrevPage();
			});
		}
	}

	if (!OnOverallQualityOptionClickedDelegateHandle.IsValid()) {
		if (OverallQualityOptionWidget) {
			OnOverallQualityOptionClickedDelegateHandle = OverallQualityOptionWidget->GetOnOptionChanged().AddUObject(this, &UUISettingsPageWidget::OnOverallQualityOptionChanged);
		}
	}

	if (!OnClearDownloadCacheClickedDelegateHandle.IsValid()) {
		if (ClearDownloadCacheButtonWidget) {
			OnClearDownloadCacheClickedDelegateHandle = ClearDownloadCacheButtonWidget->GetOnButtonClicked().AddUObject(this, &UUISettingsPageWidget::OnClearDownloadCacheClicked);
		}
	}
}

void UUISettingsPageWidget::UnregisterCallbacks() {
	Super::UnregisterCallbacks();

	if (OnSubmitButtonClickedDelegateHandle.IsValid()) {
		if (SubmitButtonWidget) {
			SubmitButtonWidget->GetOnButtonClicked().Remove(OnSubmitButtonClickedDelegateHandle);
		}
		OnSubmitButtonClickedDelegateHandle.Reset();
	}

	if (OnCancelButtonClickedDelegateHandle.IsValid()) {
		if (CancelButtonWidget) {
			CancelButtonWidget->GetOnButtonClicked().Remove(OnCancelButtonClickedDelegateHandle);
		}
		OnCancelButtonClickedDelegateHandle.Reset();
	}

	if (OnOverallQualityOptionClickedDelegateHandle.IsValid()) {
		if (OverallQualityOptionWidget) {
			OverallQualityOptionWidget->GetOnOptionChanged().Remove(OnOverallQualityOptionClickedDelegateHandle);
		}
		OnOverallQualityOptionClickedDelegateHandle.Reset();
	}

	if (OnClearDownloadCacheClickedDelegateHandle.IsValid()) {
		if (ClearDownloadCacheButtonWidget) {
			ClearDownloadCacheButtonWidget->GetOnButtonClicked().Remove(OnClearDownloadCacheClickedDelegateHandle);
		}
		OnClearDownloadCacheClickedDelegateHandle.Reset();
	}
}

void UUISettingsPageWidget::NativeOnRefresh() {
	Super::NativeOnRefresh();

	UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();

	float CurrentScaleNormalized;
	float CurrentScaleValue;
	float MinScaleValue;
	float MaxScaleValue;
	Settings->GetResolutionScaleInformationEx(CurrentScaleNormalized, CurrentScaleValue, MinScaleValue, MaxScaleValue);

	ResolutionScaleOptionWidget->SetMaxValue(MaxScaleValue);
	ResolutionScaleOptionWidget->SetMinValue(MinScaleValue);
	ResolutionScaleOptionWidget->SetValue(CurrentScaleValue);

	ViewDistanceOptionWidget->SetValue(Settings->GetViewDistanceQuality());
	AntiAliasingOptionWidget->SetValue(Settings->GetAntiAliasingQuality());
	PostProcessingOptionWidget->SetValue(Settings->GetPostProcessingQuality());
	ShadowsOptionWidget->SetValue(Settings->GetShadowQuality());
	TexturesOptionWidget->SetValue(Settings->GetTextureQuality());
	EffectsOptionWidget->SetValue(Settings->GetVisualEffectQuality());
	FoliageOptionWidget->SetValue(Settings->GetFoliageQuality());
	ShadingOptionWidget->SetValue(Settings->GetShadingQuality());
}

void UUISettingsPageWidget::NativeOnInitializedShared() {
	Super::NativeOnInitializedShared();

	GET_MODULE_SUBSYSTEM(UserSubsystem, Api2, User);
	if (UserSubsystem) {
		if (ChangeNameWidget) {
			TSharedRef<FOnUserRequestCompleted2> CallbackPtr = MakeShareable(new FOnUserRequestCompleted2());
			CallbackPtr->BindWeakLambda(this, [=](const FApiUserMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError) mutable {
				if (InResponseCode == EApi2ResponseCode::Ok) {
					ChangeNameWidget->SetText(FText::FromString(InMetadata.Name));
				}
			});
			UserSubsystem->GetMe(CallbackPtr);
		}

		if (ChangeNameButtonWidget) {
			ChangeNameButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [=] {
				auto NewName = ChangeNameWidget->GetText().ToString();

				// 1. Create a callback.
				TSharedPtr<FOnOkRequestCompleted2> CallbackPtr = MakeShareable(new FOnOkRequestCompleted2());
				// 2. Bind callback.
				CallbackPtr->BindWeakLambda(this, [=](const bool bInOk, EApi2ResponseCode InResponseCode, const FString& InError) mutable {
					GET_MODULE_SUBSYSTEM(NotificationSubsystem, UI, Notification);
					if (NotificationSubsystem) {
						FUINotificationData NotificationData;
						if (InResponseCode == EApi2ResponseCode::Ok) {
							NotificationData.Type = EUINotificationType::Success;
							NotificationData.Header = LOCTEXT("UserNameChangeSuccess", "Success");
							NotificationData.Message = FText::FromString("Successfully changed username.");
						} else {
							NotificationData.Type = EUINotificationType::Failure;
							NotificationData.Header = LOCTEXT("UserNameChangeError", "Error");
							NotificationData.Message = FText::FromString("Failed to change a user name, try another name.");
						}
						NotificationSubsystem->AddNotification(NotificationData);
					}

					CallbackPtr.Reset();
				});
				// 3. Make the request using the callback.
				UserSubsystem->ChangeUserName(NewName, CallbackPtr);

			});
		}
	}
}

void UUISettingsPageWidget::OnSubmitButtonClicked() {
	UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();

	Settings->SetResolutionScaleValueEx(ResolutionScaleOptionWidget->GetValue());
	Settings->SetViewDistanceQuality(ViewDistanceOptionWidget->GetValue());
	Settings->SetAntiAliasingQuality(AntiAliasingOptionWidget->GetValue());
	Settings->SetPostProcessingQuality(PostProcessingOptionWidget->GetValue());
	Settings->SetShadowQuality(ShadowsOptionWidget->GetValue());
	Settings->SetTextureQuality(TexturesOptionWidget->GetValue());
	Settings->SetVisualEffectQuality(EffectsOptionWidget->GetValue());
	Settings->SetFoliageQuality(FoliageOptionWidget->GetValue());
	Settings->SetShadingQuality(ShadingOptionWidget->GetValue());

	Settings->ApplySettings(true);
	Settings->SaveSettings();

	GetPageManager()->ShowPrevPage();
}

void UUISettingsPageWidget::OnOverallQualityOptionChanged() {
	const int32 Value = OverallQualityOptionWidget->GetValue();

	ViewDistanceOptionWidget->SetValue(Value);
	AntiAliasingOptionWidget->SetValue(Value);
	PostProcessingOptionWidget->SetValue(Value);
	ShadowsOptionWidget->SetValue(Value);
	TexturesOptionWidget->SetValue(Value);
	EffectsOptionWidget->SetValue(Value);
	FoliageOptionWidget->SetValue(Value);
	ShadingOptionWidget->SetValue(Value);
}

void UUISettingsPageWidget::OnClearDownloadCacheClicked() {
	// Get the cache directory
	const FString CacheDir = FPaths::ProjectSavedDir() + TEXT("dlcache/");
	// Delete the cache directory
	const bool bSuccessful = IFileManager::Get().DeleteDirectory(*CacheDir, false, true);

	GET_MODULE_SUBSYSTEM(NotificationSubsystem, UI, Notification);
	if (NotificationSubsystem) {
		FUINotificationData NotificationData;
		if (bSuccessful) {
			NotificationData.Type = EUINotificationType::Success;
			NotificationData.Header = LOCTEXT("ClearDownloadCacheSuccess", "Success");
			NotificationData.Message = FText::FromString("Successfully cleared the download cache.");
		} else {
			NotificationData.Type = EUINotificationType::Failure;
			NotificationData.Header = LOCTEXT("ClearDownloadCacheError", "Error");
			NotificationData.Message = FText::FromString("Failed to clear download cache.");
		}
		NotificationSubsystem->AddNotification(NotificationData);
	}

	// Recreate the cache directory
	IFileManager::Get().MakeDirectory(*CacheDir);
}


#undef LOCTEXT_NAMESPACE
