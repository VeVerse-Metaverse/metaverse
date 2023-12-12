// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UISocialPanelWidget.h"

#include "UISocialInvitationListWidget.h"
#include "UISocialFriendListWidget.h"
#include "UISocialUserCardWidget.h"
#include "VeApi2.h"
#include "Api2AuthSubsystem.h"
#include "Animation/UMGSequencePlayer.h"

void UUISocialPanelWidget::NativeConstruct() {
	Super::NativeConstruct();

	// Collapsed by default.
	bIsCollapsed = true;

	// Set so the panel won't close after any input will lose focus.  
	bIsFocusable = true;

	bInvitationWidgetVisible = true;
	bFriendListWidgetVisible = true;

	Refresh();
}

void UUISocialPanelWidget::RegisterCallbacks() {
	Super::RegisterCallbacks();

	if (!OnFriendsButtonClickedDelegateHandle.IsValid()) {
		if (FriendsButtonWidget) {
			OnFriendsButtonClickedDelegateHandle = FriendsButtonWidget->GetOnButtonClicked().AddUObject(this, &UUISocialPanelWidget::OnFriendsButtonClicked);
		}
	}

	if (!OnInvitationButtonClickedDelegateHandle.IsValid()) {
		if (InvitationsButtonWidget) {
			OnInvitationButtonClickedDelegateHandle = InvitationsButtonWidget->GetOnButtonClicked().AddUObject(this, &UUISocialPanelWidget::OnInvitationsButtonClicked);
		}
	}

	GET_MODULE_SUBSYSTEM(AuthSubsystem, Api2, Auth);
	if (AuthSubsystem) {
		auto OnChangeAuthorizationStateCallback = [=](EAuthorizationState AuthorizationState) {
			if (AuthSubsystem->IsAuthorized()) {
				Refresh();
			} else { }
		};

		AuthSubsystem->GetOnChangeAuthorizationState().AddWeakLambda(this, OnChangeAuthorizationStateCallback);
		OnChangeAuthorizationStateCallback(AuthSubsystem->GetAuthorizationState());
	}
}

void UUISocialPanelWidget::UnregisterCallbacks() {
	Super::UnregisterCallbacks();

	if (OnFriendsButtonClickedDelegateHandle.IsValid()) {
		if (FriendsButtonWidget) {
			FriendsButtonWidget->GetOnButtonClicked().Remove(OnFriendsButtonClickedDelegateHandle);
			OnFriendsButtonClickedDelegateHandle.Reset();
		}
	}

	if (OnInvitationButtonClickedDelegateHandle.IsValid()) {
		if (InvitationsButtonWidget) {
			InvitationsButtonWidget->GetOnButtonClicked().Remove(OnInvitationButtonClickedDelegateHandle);
			OnInvitationButtonClickedDelegateHandle.Reset();
		}
	}

	GET_MODULE_SUBSYSTEM(AuthSubsystem, Api2, Auth);
	if (AuthSubsystem) {
		AuthSubsystem->GetOnChangeAuthorizationState().RemoveAll(this);
	}
}

void UUISocialPanelWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) {
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	Expand();
}

void UUISocialPanelWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent) {
	Super::NativeOnMouseLeave(InMouseEvent);
	Collapse();
}

void UUISocialPanelWidget::OnFriendsButtonClicked() {
	NavigateToFriendList();
}

void UUISocialPanelWidget::OnInvitationsButtonClicked() {
	NavigateToInvitationList();
}

void UUISocialPanelWidget::NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent) {
	Expand();

	Super::NativeOnAddedToFocusPath(InFocusEvent);
}

void UUISocialPanelWidget::NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent) {
	Collapse();

	Super::NativeOnRemovedFromFocusPath(InFocusEvent);
}

void UUISocialPanelWidget::Refresh() {
	GET_MODULE_SUBSYSTEM(AuthSubsystem, Api2, Auth);
	if (AuthSubsystem) {
		if (UserCardWidget) {
			UserCardWidget->SetUserMetadata(AuthSubsystem->GetUserMetadata());
		}
		if (SocialFriendListWidget) {
			SocialFriendListWidget->SetUserId(AuthSubsystem->GetUserId());
		}
		if (SocialInvitationListWidget) {
			SocialInvitationListWidget->SetUserId(AuthSubsystem->GetUserId());
		}
	}

	NavigateToFriendList();
}

UUMGSequencePlayer* UUISocialPanelWidget::Expand(const bool bAnimate, const bool bPlaySound) {
	if (GetIsCollapsed()) {
		if (bPlaySound && ExpandSound) {
			PlaySound(ExpandSound);
		}

		if (bAnimate) {
			if (ExpandAnimation) {
				UUMGSequencePlayer* SequencePlayer = PlayAnimationForward(ExpandAnimation);
				SequencePlayer->OnSequenceFinishedPlaying().AddWeakLambda(this, [this](UUMGSequencePlayer&) {
					SetIsCollapsed(false);
				});
				return SequencePlayer;
			}

			if (CollapseAnimation) {
				UUMGSequencePlayer* SequencePlayer = PlayAnimationReverse(CollapseAnimation);
				SequencePlayer->OnSequenceFinishedPlaying().AddWeakLambda(this, [this](UUMGSequencePlayer&) {
					SetIsCollapsed(false);
				});
				return SequencePlayer;
			}
		}
	}

	SetIsCollapsed(false);
	return nullptr;
}

UUMGSequencePlayer* UUISocialPanelWidget::Collapse(const bool bAnimate, const bool bPlaySound) {
	if (!GetIsCollapsed()) {
		if (bPlaySound && CollapseSound) {
			PlaySound(CollapseSound);
		}

		if (bAnimate) {
			if (CollapseAnimation) {
				UUMGSequencePlayer* SequencePlayer = PlayAnimationForward(CollapseAnimation);
				if (SequencePlayer) {
					SequencePlayer->OnSequenceFinishedPlaying().AddWeakLambda(this, [this](UUMGSequencePlayer&) {
						SetIsCollapsed(true);
					});
					return SequencePlayer;
				}
			}

			if (ExpandAnimation) {
				UUMGSequencePlayer* SequencePlayer = PlayAnimationReverse(ExpandAnimation);
				if (SequencePlayer) {
					SequencePlayer->OnSequenceFinishedPlaying().AddWeakLambda(this, [this](UUMGSequencePlayer&) {
						SetIsCollapsed(true);
					});
					return SequencePlayer;
				}
			}
		}
	}

	SetIsCollapsed(true);
	return nullptr;
}

void UUISocialPanelWidget::SetIsCollapsed(const bool bInIsCollapsed) {
	bIsCollapsed = bInIsCollapsed;
}

bool UUISocialPanelWidget::GetIsCollapsed() const {
	return bIsCollapsed;
}

void UUISocialPanelWidget::ShowInvitationWidget(const bool bAnimate, const bool bPlaySound) {
	if (SocialInvitationListWidget) {
		if (!bInvitationWidgetVisible) {
			bInvitationWidgetVisible = true;
			// SocialInvitationListWidget->ResetState();
			// SocialInvitationListWidget->ResetAnimationState();
			SocialInvitationListWidget->Show();
		}
		SocialInvitationListWidget->Refresh();
	}
}

void UUISocialPanelWidget::ShowFriendListWidget(const bool bAnimate, const bool bPlaySound) {
	if (SocialFriendListWidget) {
		if (!bFriendListWidgetVisible) {
			bFriendListWidgetVisible = true;
			// SocialFriendListWidget->ResetState();
			// SocialFriendListWidget->ResetAnimationState();
			SocialFriendListWidget->Show();
		}
		SocialFriendListWidget->Refresh();
	}
}

void UUISocialPanelWidget::HideInvitationWidget(const bool bAnimate, const bool bPlaySound) {
	if (bInvitationWidgetVisible) {
		bInvitationWidgetVisible = false;
		if (SocialInvitationListWidget) {
			SocialInvitationListWidget->Hide();
		}
	}
}

void UUISocialPanelWidget::HideFriendListWidget(const bool bAnimate, const bool bPlaySound) {
	if (bFriendListWidgetVisible) {
		bFriendListWidgetVisible = false;
		if (SocialFriendListWidget) {
			SocialFriendListWidget->Hide();
		}
	}
}

void UUISocialPanelWidget::NavigateToInvitationList(const bool bAnimate, const bool bPlaySound) {
	ShowInvitationWidget();
	HideFriendListWidget();
}

void UUISocialPanelWidget::NavigateToFriendList(const bool bAnimate, const bool bPlaySound) {
	HideInvitationWidget();
	ShowFriendListWidget();
}
