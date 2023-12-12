// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "VR/UISpaceDetailWidget.h"

#include "UIPlayerController.h"


void UUISpaceDetailWidget::RegisterCallbacks() {
	if (CloseButtonWidget) {
		if (!OnCloseButtonWidgetDelegateHandle.IsValid()) {
			OnCloseButtonWidgetDelegateHandle = CloseButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
				if (OnClosed.IsBound()) {
					OnClosed.Broadcast();
				}
			});
		}
	}
	
	if (!OnSpaceEditButtonClickedDelegateHandle.IsValid()) {
		if (EditButtonWidget) {
			OnSpaceEditButtonClickedDelegateHandle = EditButtonWidget->GetOnButtonClicked().AddUObject(this, &UUISpaceDetailWidget::OnSpaceFormButtonClicked);
		}
	}

	if (!OnSpaceVisitButtonClickedDelegateHandle.IsValid()) {
		if (VisitButtonWidget) {
			OnSpaceVisitButtonClickedDelegateHandle = VisitButtonWidget->GetOnButtonClicked().AddUObject(this, &UUISpaceDetailWidget::OnSpaceVisitButtonClicked);
		}
	}
}

void UUISpaceDetailWidget::UnregisterCallbacks() {
	if (OnCloseButtonWidgetDelegateHandle.IsValid()) {
		if (CloseButtonWidget) {
			CloseButtonWidget->GetOnButtonClicked().Remove(OnCloseButtonWidgetDelegateHandle);
			OnCloseButtonWidgetDelegateHandle.Reset();
		}
	}
	
	if (OnSpaceEditButtonClickedDelegateHandle.IsValid()) {
		if (EditButtonWidget) {
			EditButtonWidget->GetOnButtonClicked().Remove(OnSpaceEditButtonClickedDelegateHandle);
			OnSpaceEditButtonClickedDelegateHandle.Reset();
		}
	}

	if (OnSpaceVisitButtonClickedDelegateHandle.IsValid()) {
		if (VisitButtonWidget) {
			VisitButtonWidget->GetOnButtonClicked().Remove(OnSpaceVisitButtonClickedDelegateHandle);
			OnSpaceVisitButtonClickedDelegateHandle.Reset();
		}
	}
}

void UUISpaceDetailWidget::OnSpaceFormButtonClicked() {
	if (Metadata) {
		if (OnSpaceEditButtonClicked.IsBound()) {
			OnSpaceEditButtonClicked.Broadcast(Metadata);
		}
	}
}

void UUISpaceDetailWidget::OnSpaceVisitButtonClicked() {
	if (Metadata) {
		if (!IsRunningDedicatedServer()) {
			if (AUIPlayerController* PlayerController = GetOwningPlayer<AUIPlayerController>()) {
				FString SpaceIdString = Metadata->Id.ToString();
				PlayerController->OpenLevel(FName(Metadata->Map), true, FString::Printf(TEXT("?SpaceId=%s"), *SpaceIdString));
			}
		}
	}
}

void UUISpaceDetailWidget::NativeConstruct() {


	Super::NativeConstruct();
}

void UUISpaceDetailWidget::SetMetadata(TSharedPtr<FApiSpaceMetadata> InSpace) {
	Metadata = InSpace;

	if (NameTextWidget) {
		if (Metadata->Name.IsEmpty()) {
			NameTextWidget->SetText(NSLOCTEXT("VeUI", "SpaceNameUnknown", "Unnamed"));
		} else {
			NameTextWidget->SetText(FText::FromString(Metadata->Name));
		}
	}

	if (DescriptionTextWidget) {
		if (Metadata->Description.IsEmpty()) {
			DescriptionTextWidget->SetText(NSLOCTEXT("VeUI", "MSpaceDescriptionUnknown", "No description"));
		} else {
			DescriptionTextWidget->SetText(FText::FromString(Metadata->Description));
		}
	}

	if (ViewCountTextWidget) {
		ViewCountTextWidget->SetText(FText::Format(NSLOCTEXT("VeUI", "SpaceViewCountFormat", "{0}"), FText::AsNumber(Metadata->Views)));
	}

	//
	// if (LikeCountTextWidget) {
	// 	LikeCountTextWidget->SetText(FText::Format(NSLOCTEXT("VeUI", "SpaceLikeCountFormat", "{0}"), FText::AsNumber(Space->Likes)));
	// }
	//
	// if (DislikeCountTextWidget) {
	// 	DislikeCountTextWidget->SetText(FText::Format(NSLOCTEXT("VeUI", "SpaceLikeCountFormat", "{0}"), FText::AsNumber(Space->Dislikes)));
	// }

	// if (RatingTextWidget) {
	// 	if (Space->Likes + Space->Dislikes > 0) {
	// 		const float Value = (static_cast<float>(Space->Likes) / (Space->Likes + Space->Dislikes) * 5 - 1) / 4 * 5;
	// 		RatingTextWidget->SetText(FText::Format(NSLOCTEXT("VeUI", "SpaceRatingFormat", "{0}"), FText::AsNumber(Value)));
	// 	} else {
	// 		RatingTextWidget->SetText(NSLOCTEXT("VeUI", "SpaceRatingNone", "Not rated"));
	// 	}
	// }
}
