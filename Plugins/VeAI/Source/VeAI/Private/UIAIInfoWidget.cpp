// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UIAIInfoWidget.h"
#include "VeAiPlayerState.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"
#include "Components/UILoadingWidget.h"

#define LOCTEXT_NAMESPACE "VeAI"

void UUIAIInfoWidget::NativeConstruct() {
	Super::NativeConstruct();
}

void UUIAIInfoWidget::SetOpacity(const float InOpacity) {
	if (RootWidget) {
		RootWidget->SetContentColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, InOpacity));
	}
}

void UUIAIInfoWidget::SetAIPlayerState(AVeAiPlayerState* InPlayerState) {
	if (PlayerState.IsValid()) {
		PlayerState->GetOnAiUserMetadataUpdated().RemoveAll(this);
	}

	PlayerState = InPlayerState;

	if (PlayerState.IsValid()) {
		PlayerState->GetOnAiUserMetadataUpdated().AddUObject(this, &UUIAIInfoWidget::OnUserMetadataUpdated);

		OnUserMetadataUpdated(PlayerState->GetUserMetadata());
	}
}

void UUIAIInfoWidget::OnUserMetadataUpdated(const FVeUserMetadata& InMetadata) {
	if (NameTextWidget) {
		if (InMetadata.Name.IsEmpty()) {
			NameTextWidget->SetText(NSLOCTEXT("VeAI", "Unnamed", "Unnamed"));
		} else {
			NameTextWidget->SetText(FText::FromString(InMetadata.Name));
		}
	}

	if (TitleTextWidget) {
		if (InMetadata.Title.IsEmpty()) {
			TitleTextWidget->SetText(NSLOCTEXT("VeAI", "AI", "AI"));
			// } else {
			// 	TitleTextWidget->SetText(FText::FromString(InMetadata.Title));
		}
	}

	if (IsValid(SubtitleContainerWidget)) {
		SubtitleContainerWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UUIAIInfoWidget::OnSay(const FText& InText) {

	// Set the user voice status.
	if (IsValid(UserVoiceWidget)) {
		UserVoiceWidget->SetVoiceStatus(true);
	}

	// Reset the subtitle text.
	if (IsValid(SubtitleTextWidget)) {
		SubtitleTextWidget->SetText({});
	}

	if (IsValid(SubtitleContainerWidget)) {
		SubtitleContainerWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}

	// How long it should take to read the whole string.
	const float DelayInSeconds = InText.ToString().Len() * ResetDelaySecondsPerCharacter;

	// Get world to reach the timer manager, won't work outside of the game (i.e. in Slate).
	if (const auto* const World = GetWorld()) {
		// Clear the timers if they are already set.
		if (SayTypingTimerHandle.IsValid()) {
			World->GetTimerManager().ClearTimer(SayTypingTimerHandle);
		}

		if (SayResetTimerHandle.IsValid()) {
			World->GetTimerManager().ClearTimer(SayResetTimerHandle);
		}

		// Set the timers.
		auto& TimerManager = World->GetTimerManager();

		FTimerDelegate TypingTimerDelegate;
		TypingTimerDelegate.BindWeakLambda(this, [=] {
			// Update the text symbol by symbol.
			if (IsValid(SubtitleTextWidget)) {
				SubtitleTextWidget->SetText(FText::FromString(InText.ToString().Left(SubtitleTextWidget->GetText().ToString().Len() + 1)));
			}
		});
		TimerManager.SetTimer(SayTypingTimerHandle, TypingTimerDelegate, TypingCharactersPerSecond, true);

		FTimerDelegate ResetTimerDelegate;
		ResetTimerDelegate.BindWeakLambda(this, [&] {
			// Reset the voice status.
			if (IsValid(UserVoiceWidget)) {
				UserVoiceWidget->SetVoiceStatus(false);
			}

			// Reset text.
			if (IsValid(SubtitleTextWidget)) {
				SubtitleTextWidget->SetText(FText{});
				if (IsValid(SubtitleContainerWidget)) {
					SubtitleContainerWidget->SetVisibility(ESlateVisibility::Collapsed);
				}
			}

			TimerManager.ClearTimer(SayTypingTimerHandle);
			TimerManager.ClearTimer(SayResetTimerHandle);
		});
		TimerManager.SetTimer(SayResetTimerHandle, ResetTimerDelegate, DelayInSeconds, false);
	}
}

#undef LOCTEXT_NAMESPACE
