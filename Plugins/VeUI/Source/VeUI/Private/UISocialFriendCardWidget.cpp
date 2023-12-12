// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UISocialFriendCardWidget.h"

#include "ApiUserMetadataObject.h"
#include "VeShared.h"
#include "AnimatedTexture2D.h"


#define LOCTEXT_NAMESPACE "VeUI"


// void UUISocialFriendCardWidget::NativeConstruct() {
// 	Super::NativeConstruct();
// }

void UUISocialFriendCardWidget::NativeOnListItemObjectSet(UObject* ListItemObject) {
	if (const UApiUserMetadataObject* UserObject = Cast<UApiUserMetadataObject>(ListItemObject)) {
		TSharedPtr<FApiUserMetadata> UserMetadata = UserObject->Metadata;
		if (UserMetadata.IsValid()) {
			
			if (NameTextWidget) {
				if (UserMetadata->Name.IsEmpty()) {
					NameTextWidget->SetText(NSLOCTEXT("VeUI", "UserNameUnknown", "Unnamed"));
				} else {
					NameTextWidget->SetText(FText::FromString(UserMetadata->Name));
				}
			}

			SetStatus(UserMetadata->GetStatus());

			if (PreviewImageWidget) {
				if (!PreviewImageWidget->ShowImage(UserMetadata->Files, EApiFileType::ImageAvatar)) {
					PreviewImageWidget->ShowImage(UserMetadata->GetDefaultAvatarUrl());
				}else {
					PreviewImageWidget->ShowDefaultImage();
				}
			}
		}
	}
}

void UUISocialFriendCardWidget::SetStatus(EApiPresenceStatus Status) {
	FLinearColor Color;
	FText Text;
	
	switch (Status) {
	case EApiPresenceStatus::Offline:
		Color = FColor(0xff808080);
		Text = NSLOCTEXT("VeUI", "OfflineUserStatus", "Offline");
		break;
	case EApiPresenceStatus::Available:
		Color = FColor(0xff0080ff);
		Text = NSLOCTEXT("VeUI", "OnlineUserStatus", "Online");
		break;
	case EApiPresenceStatus::Playing:
		Color = FColor(0xff00e015);
		Text = NSLOCTEXT("VeUI", "PlayingUserStatus", "Playing");
		break;
	}
	
	if (AvatarBorderWidget) {
		AvatarBorderWidget->SetBrushColor(Color);
	}
	if (StatusTextWidget) {
		StatusTextWidget->SetColorAndOpacity(Color);
		StatusTextWidget->SetText(Text);
	}
}

#undef LOCTEXT_NAMESPACE
