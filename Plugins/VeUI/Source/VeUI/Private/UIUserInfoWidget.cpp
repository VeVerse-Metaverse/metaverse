// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UIUserInfoWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/UILoadingWidget.h"
#include "VePlayerStateBase.h"
#include "AnimatedTexture2D.h"
#include "Components/UIImageWidget.h"

#define LOCTEXT_NAMESPACE "VeUI"


// void UUIUserInfoWidget::NativeConstruct() {
// 	Super::NativeConstruct();
// }

// void UUIUserInfoWidget::NativeDestruct() {
// 	Super::NativeDestruct();
// }

void UUIUserInfoWidget::SetPlayerState(AVePlayerStateBase* InPlayerState) {
	if (PlayerState.IsValid()) {
		PlayerState->GetOnUserMetadataUpdated().RemoveAll(this);
		PlayerState->GetOnIsSpeakingChanged().RemoveAll(this);
	}

	PlayerState = InPlayerState;

	if (PlayerState.IsValid()) {
		PlayerState->GetOnUserMetadataUpdated().AddUObject(this, &UUIUserInfoWidget::OnUserMetadataUpdated);
		PlayerState->GetOnIsSpeakingChanged().AddUObject(this, &UUIUserInfoWidget::OnIsSpeakingChanged);

		OnUserMetadataUpdated(PlayerState->GetUserMetadata());
		OnIsSpeakingChanged(PlayerState->GetIsSpeaking());
	}
}

void UUIUserInfoWidget::OnUserMetadataUpdated(const FVeUserMetadata& InMetadata) {
	if (NameTextWidget) {
		if (InMetadata.Name.IsEmpty()) {
			NameTextWidget->SetText(NSLOCTEXT("VeUI", "Unnamed", "Unnamed"));
		} else {
			NameTextWidget->SetText(FText::FromString(InMetadata.Name));
		}
	}

	if (TitleTextWidget) {
		if (InMetadata.Title.IsEmpty()) {
			TitleTextWidget->SetText(NSLOCTEXT("VeUI", "User", "User"));
		} else {
			TitleTextWidget->SetText(FText::FromString(InMetadata.Title));
		}
	}

	if (RatingTextWidget) {
		RatingTextWidget->SetText(FText::Format(LOCTEXT("RatingFormat", "{0}"), InMetadata.Rating));
	}

	if (LevelTextWidget) {
		LevelTextWidget->SetText(FText::Format(LOCTEXT("LevelFormat", "{0}"), InMetadata.Level));
	}

	if (AvatarWidget) {
		if (!InMetadata.AvatarUrl.IsEmpty()) {
			if (AvatarWidget) {
				AvatarWidget->ShowImage(InMetadata.AvatarUrl);
			}
		}
	}
}

void UUIUserInfoWidget::OnIsSpeakingChanged(const bool bInIsSpeaking) const {
	if (UserVoiceWidget) {
		UserVoiceWidget->SetVoiceStatus(bInIsSpeaking);
	}
}


#undef LOCTEXT_NAMESPACE
