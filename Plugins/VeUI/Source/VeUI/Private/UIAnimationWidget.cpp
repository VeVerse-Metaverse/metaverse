// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "UIAnimationWidget.h"

#include "Components/Border.h"
#include "Animation/UMGSequencePlayer.h"
#include "UIWidgetBase.h"


void UUIAnimationWidget::SetContentWidget(UUIWidgetBase* InContentWidget) {
	if (!IsValid(ContainerWidget)) {
		return;
	}

	ContainerWidget->ClearChildren();
	if (InContentWidget) {
		ContainerWidget->AddChild(InContentWidget);
	}
}

UUMGSequencePlayer* UUIAnimationWidget::Show(bool bPlayAnimation) {
	// Check if we already have an old animation currently playing.
	if (FadeAnimationSequencePlayer) {
		if (FadeAnimationFinishedDelegateHandle.IsValid()) {
			// Remove the delegate so the animation finish event won't be fired anymore as we switch to the new animation.
			FadeAnimationSequencePlayer->OnSequenceFinishedPlaying().Remove(FadeAnimationFinishedDelegateHandle);
			FadeAnimationFinishedDelegateHandle.Reset();
		}

		// Stop the old animation as we don't need it to overlap with the new one.
		FadeAnimationSequencePlayer->Stop();

		// Reset the animation notifying that animation has been interrupted.
		const UWidgetAnimation* CurrentFadeAnimation = FadeAnimationSequencePlayer->GetAnimation();
		if (CurrentFadeAnimation) {
			if (CurrentFadeAnimation == FadeInAnimation) {
				NativeOnShowFinished(false);
			} else if (CurrentFadeAnimation == FadeOutAnimation) {
				NativeOnHideFinished(false);
			} else {
				// LogErrorF("unknown animation");
			}
		}

		FadeAnimationSequencePlayer = nullptr;
	}
	
	NativeOnShowStarted();

	if (FadeInSound) {
		PlaySound(FadeInSound);
	}

	if (bPlayAnimation) {
		if (FadeInAnimation) {
			// Start the new animation.
			FadeAnimationSequencePlayer = PlayAnimationForward(FadeInAnimation);
		
			// Bind the delegate to the new animation.
			FadeAnimationFinishedDelegateHandle = FadeAnimationSequencePlayer->OnSequenceFinishedPlaying().AddWeakLambda(this, [this](UUMGSequencePlayer& InSequencePlayer) {
				NativeOnShowFinished(true);
				FadeAnimationSequencePlayer = nullptr;
			});

			return FadeAnimationSequencePlayer;
		}
	}

	NativeOnShowFinished(true);
	return nullptr;
}

UUMGSequencePlayer* UUIAnimationWidget::Hide() {
	// Check if we already have an old animation currently playing.
	if (FadeAnimationSequencePlayer) {
		if (FadeAnimationFinishedDelegateHandle.IsValid()) {
			// Remove the delegate so the animation finish event won't be fired anymore as we switch to the new animation.
			FadeAnimationSequencePlayer->OnSequenceFinishedPlaying().Remove(FadeAnimationFinishedDelegateHandle);
			FadeAnimationFinishedDelegateHandle.Reset();
		}

		// Stop the old animation as we don't need it to overlap with the new one.
		FadeAnimationSequencePlayer->Stop();

		// Reset the animation notifying that animation has been interrupted.
		const UWidgetAnimation* CurrentFadeAnimation = FadeAnimationSequencePlayer->GetAnimation();
		if (CurrentFadeAnimation) {
			if (CurrentFadeAnimation == FadeInAnimation) {
				NativeOnShowFinished(false);
			} else if (CurrentFadeAnimation == FadeOutAnimation) {
				NativeOnHideFinished(false);
			} else {
				// LogErrorF("unknown animation");
			}
		}

		FadeAnimationSequencePlayer = nullptr;
	}

	NativeOnHideStarted();
	
	if (FadeOutSound) {
		PlaySound(FadeOutSound);
	}

	if (FadeOutAnimation) {
		// Start the new animation using fallback fade out animation if fade in animation is not configured.
		FadeAnimationSequencePlayer = PlayAnimationForward(FadeOutAnimation);
	
		// Bind the delegate to the new animation.
		FadeAnimationFinishedDelegateHandle = FadeAnimationSequencePlayer->OnSequenceFinishedPlaying().AddWeakLambda(this, [this](UUMGSequencePlayer& InSequencePlayer) {
			NativeOnHideFinished(true);
			FadeAnimationSequencePlayer = nullptr;
		});
		
		return FadeAnimationSequencePlayer;
	}

	NativeOnHideFinished(true);
	return nullptr;
}

void UUIAnimationWidget::NativeOnShowStarted() {
	OnShowStarted.Broadcast(this);
	
	if (auto* Root = GetRootWidget()) {
		Root->SetVisibility(VisibilityBeforeFadeIn);
	}
}

void UUIAnimationWidget::NativeOnShowFinished(bool bInHasCompleted) {
	OnShowFinished.Broadcast(this);

	if (auto* Root = GetRootWidget()) {
		Root->SetVisibility(VisibilityAfterFadeIn);
	}
}

void UUIAnimationWidget::NativeOnHideStarted() {
	OnHideStarted.Broadcast(this);
	
	if (auto* Root = GetRootWidget()) {
		Root->SetVisibility(VisibilityBeforeFadeOut);
	}
}

void UUIAnimationWidget::NativeOnHideFinished(bool bInHasCompleted) {
	OnHideFinished.Broadcast(this);

	if (auto* Root = GetRootWidget()) {
		Root->SetVisibility(VisibilityAfterFadeOut);
	}
}
