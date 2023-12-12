// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "VR/UIWidget.h"

#include "VeHUD.h"
#include "Animation/UMGSequencePlayer.h"
#include "Components/PanelWidget.h"
#include "Subsystems/UINotificationSubsystem.h"
#include "VeShared.h"
#include "VeUI.h"


bool UUIWidget::Initialize() {
	const bool bResult = Super::Initialize();

	if (bResult && IsSdk()) {
		NativeOnInitializedSdk();
	}

	return bResult;
}

void UUIWidget::NativeOnInitialized() {
	NativeOnInitializedShared();

	Super::NativeOnInitialized();
}

void UUIWidget::NativeOnInitializedShared() {
	if (RootWidget) {
		RootWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UUIWidget::NativeOnInitializedSdk() {
	NativeOnInitializedShared();
}

void UUIWidget::NativeConstruct() {
	Super::NativeConstruct();
	RegisterCallbacks();
}

void UUIWidget::NativeDestruct() {
	Super::NativeDestruct();
	UnregisterCallbacks();
}

void UUIWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) {
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	if (OnMouseEnter.IsBound()) {
		OnMouseEnter.Broadcast(InGeometry, InMouseEvent);
	}
}

void UUIWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent) {
	Super::NativeOnMouseLeave(InMouseEvent);

	if (OnMouseLeave.IsBound()) {
		OnMouseLeave.Broadcast(InMouseEvent);
	}
}

void UUIWidget::RegisterCallbacks() {
}

void UUIWidget::UnregisterCallbacks() {
}

void UUIWidget::OnNavigate() {
}

void UUIWidget::AddNotification(FUINotificationData NotificationData) {
	if (FVeUIModule* UIModule = FVeUIModule::Get()) {
		if (const TSharedPtr<FUINotificationSubsystem> NotificationSubsystem = UIModule->GetNotificationSubsystem()) {
			NotificationSubsystem->AddNotification(NotificationData);

		} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(Notificationsubsystem)); }
	} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); }
}

UPanelWidget* UUIWidget::GetRootWidget_Implementation() {
	return RootWidget;
}

void UUIWidget::ResetState_Implementation() {
}

void UUIWidget::ResetAnimationState_Implementation() {
}

AVeHUD* UUIWidget::GetHUD() const {
	if (const APlayerController* PlayerController = GetOwningPlayer()) {
		if (AHUD* HUD = PlayerController->GetHUD()) {
			return Cast<AVeHUD>(HUD);
		}
	}
	return nullptr;
}

void UUIWidget::SetIsHidden(const bool bInIsHidden) {
	bIsHidden = bInIsHidden;
}

bool UUIWidget::GetIsHidden() const {
	return bIsHidden;
}

bool UUIWidget::IsSdk() const {
#if WITH_EDITOR
	return !IsValid(GetWorld());
#else
	return false;
#endif
}

UUMGSequencePlayer* UUIWidget::Show(const bool bAnimate, const bool bPlaySound) {
	// Check if we already have an old animation currently playing.
	if (FadeAnimationSequencePlayer) {
		if (FadeAnimationCompleteDelegateHandle.IsValid()) {
			// Remove the delegate so the animation finish event won't be fired anymore as we switch to the new animation.
			FadeAnimationSequencePlayer->OnSequenceFinishedPlaying().Remove(FadeAnimationCompleteDelegateHandle);
		}

		// Stop the old animation as we don't need it to overlap with the new one.
		FadeAnimationSequencePlayer->Stop();

		// Reset the animation notifying that animation has been interrupted.
		const UWidgetAnimation* CurrentFadeAnimation = FadeAnimationSequencePlayer->GetAnimation();
		if (CurrentFadeAnimation) {
			if (CurrentFadeAnimation == FadeInAnimation) {
				if (OnFadeInAnimationFinished.IsBound()) {
					OnFadeInAnimationFinished.Broadcast(FadeAnimationSequencePlayer, false);
				}
			} else if (CurrentFadeAnimation == FadeOutAnimation) {
				if (OnFadeOutAnimationFinished.IsBound()) {
					OnFadeOutAnimationFinished.Broadcast(FadeAnimationSequencePlayer, false);
				}
			} else {
				LogErrorF("unknown animation");
			}
		}

		FadeAnimationSequencePlayer = nullptr;
		SetIsHidden(true);
	}

	if (GetIsHidden()) {
		if (bPlaySound && FadeInSound) {
			PlaySound(FadeInSound);
		}

		if (bAnimate) {
			if (UPanelWidget* Root = GetRootWidget()) {
				Root->SetVisibility(VisibilityBeforeFadeIn);
			}
			//GetRootWidget()->SetVisibility(VisibilityBeforeFadeIn);

			if (FadeInAnimation) {
				// Start the new animation.
				FadeAnimationSequencePlayer = PlayAnimationForward(FadeInAnimation);
				// Bind the delegate to the new animation.
				FadeAnimationCompleteDelegateHandle = FadeAnimationSequencePlayer->OnSequenceFinishedPlaying().AddUObject(this, &UUIWidget::OnShowAnimationFinishedCallback);
				return FadeAnimationSequencePlayer;
			}
		}
	}

	if (UPanelWidget* Root = GetRootWidget()) {
		Root->SetVisibility(VisibilityAfterFadeIn);
	}

	SetIsHidden(false);
	return nullptr;
}

void UUIWidget::OnShowAnimationFinishedCallback(UUMGSequencePlayer& InSequencePlayer) {
	if (GetIsHidden()) {
		// Reset root widget visibility to after fade in state.
		if (UPanelWidget* Root = GetRootWidget()) {
			Root->SetVisibility(VisibilityAfterFadeIn);
		}
		//GetRootWidget()->SetVisibility(VisibilityAfterFadeIn);

		// Reset hidden flag.
		SetIsHidden(false);
	}

	// Report uninterrupted animation finish.
	if (OnFadeInAnimationFinished.IsBound()) {
		OnFadeInAnimationFinished.Broadcast(FadeAnimationSequencePlayer, true);
	}

	// Reset the current fade animation.
	FadeAnimationSequencePlayer->OnSequenceFinishedPlaying().Remove(FadeAnimationCompleteDelegateHandle);
	FadeAnimationCompleteDelegateHandle.Reset();
	FadeAnimationSequencePlayer = nullptr;
}

void UUIWidget::OnHideAnimationFinishedCallback(UUMGSequencePlayer& InSequencePlayer) {
	if (!GetIsHidden()) {
		if (UPanelWidget* RootW = GetRootWidget()) {
			RootW->SetVisibility(VisibilityAfterFadeOut);
		}
		//GetRootWidget()->SetVisibility(VisibilityAfterFadeOut);
		SetIsHidden(true);
	}

	// Report uninterrupted animation finish.
	if (OnFadeOutAnimationFinished.IsBound()) {
		OnFadeOutAnimationFinished.Broadcast(FadeAnimationSequencePlayer, true);
	}

	FadeAnimationSequencePlayer->OnSequenceFinishedPlaying().Remove(FadeAnimationCompleteDelegateHandle);
	FadeAnimationCompleteDelegateHandle.Reset();
	FadeAnimationSequencePlayer = nullptr;
}

UUMGSequencePlayer* UUIWidget::Hide(const bool bAnimate, const bool bPlaySound) {
	// Check if we already have an old animation currently playing.
	if (FadeAnimationSequencePlayer) {
		if (FadeAnimationCompleteDelegateHandle.IsValid()) {
			// Remove the delegate so the animation finish event won't be fired anymore as we switch to the new animation.
			FadeAnimationSequencePlayer->OnSequenceFinishedPlaying().Remove(FadeAnimationCompleteDelegateHandle);
		}

		// Stop the old animation as we don't need it to overlap with the new one.
		FadeAnimationSequencePlayer->Stop();

		// Reset the animation notifying that animation has been interrupted.
		const UWidgetAnimation* CurrentFadeAnimation = FadeAnimationSequencePlayer->GetAnimation();
		if (CurrentFadeAnimation) {
			if (CurrentFadeAnimation == FadeInAnimation) {
				if (OnFadeInAnimationFinished.IsBound()) {
					OnFadeInAnimationFinished.Broadcast(FadeAnimationSequencePlayer, false);
				}
			} else if (CurrentFadeAnimation == FadeOutAnimation) {
				if (OnFadeOutAnimationFinished.IsBound()) {
					OnFadeOutAnimationFinished.Broadcast(FadeAnimationSequencePlayer, false);
				}
			} else {
				LogErrorF("unknown animation");
			}
		}

		FadeAnimationSequencePlayer = nullptr;
		SetIsHidden(false);
	}

	if (!GetIsHidden()) {
		if (bPlaySound && FadeOutSound) {
			PlaySound(FadeOutSound);
		}

		if (bAnimate) {
			if (UPanelWidget* Root = GetRootWidget()) {
				Root->SetVisibility(VisibilityBeforeFadeOut);
			}

			if (FadeOutAnimation) {
				// Start the new animation using fallback fade out animation if fade in animation is not configured.
				FadeAnimationSequencePlayer = PlayAnimationForward(FadeOutAnimation);
				// Bind the delegate to the new animation.
				FadeAnimationCompleteDelegateHandle = FadeAnimationSequencePlayer->OnSequenceFinishedPlaying().AddUObject(this, &UUIWidget::OnHideAnimationFinishedCallback);
				return FadeAnimationSequencePlayer;
			}
		}
	}

	if (UPanelWidget* Root = GetRootWidget()) {
		Root->SetVisibility(VisibilityAfterFadeOut);
	}

	SetIsHidden(true);
	return nullptr;
}

void UUIWidget::HideAndDestroy(const bool bAnimate, const bool bPlaySound) {
	if (!GetIsHidden()) {

		if (bPlaySound && FadeOutSound) {
			PlaySound(FadeOutSound);
		}

		if (bAnimate) {
			if (FadeOutAnimation) {
				UUMGSequencePlayer* SequencePlayer = PlayAnimationForward(FadeOutAnimation);
				if (SequencePlayer) {
					if (UPanelWidget* Root = GetRootWidget()) {
						Root->SetVisibility(VisibilityBeforeFadeOut);
					}
					//GetRootWidget()->SetVisibility(VisibilityBeforeFadeOut);

					SequencePlayer->OnSequenceFinishedPlaying().AddWeakLambda(this, [this](UUMGSequencePlayer&) {
						if (UPanelWidget* Root = GetRootWidget()) {
							Root->SetVisibility(VisibilityAfterFadeOut);
						}
						//GetRootWidget()->SetVisibility(VisibilityAfterFadeOut);
						SetIsHidden(true);
						RemoveFromParent();
					});

					return;
				}
			}
		}
	}
	if (UPanelWidget* Root = GetRootWidget()) {
		Root->SetVisibility(VisibilityAfterFadeOut);
	}
	//GetRootWidget()->SetVisibility(VisibilityAfterFadeOut);
	SetIsHidden(true);
	RemoveFromParent();
}

FText UUIWidget::GetLocalizedErrorText(const FString& ErrorString) const {
	FText ErrorText;
	if (FText::FindText(TEXT("VeUI"), ErrorString, ErrorText)) {
		return ErrorText;
	}
	return FText::FromString(ErrorString);
}

void UUIWidget::SetWidgetFocus() const {
}

void UUIWidget::SetWidgetKeyboardFocus() const {
}

bool UUIWidget::ReceiveKeyEvent(const FKeyEvent& KeyEvent) {
	return false;
}

bool UUIWidget::ReceiveCharacterInput(const FString& InCharacter) {
	return false;
}

UApiSubsystem* UUIWidget::GetApiSubsystem() const {
	if (const UGameInstance* GameInstance = GetGameInstance()) {
		return GameInstance->GetSubsystem<UApiSubsystem>();
	}
	return nullptr;
}
