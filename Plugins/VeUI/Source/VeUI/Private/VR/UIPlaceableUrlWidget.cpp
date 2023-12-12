// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "VR/UIPlaceableUrlWidget.h"

#include "Subsystems/UIBreadcrumbSubsystem.h"
#include "VeUI.h"

void UUIPlaceableUrlWidget::RegisterCallbacks() {
	if (!OnCloseButtonClickedDelegateHandle.IsValid()) {
		if (CloseButtonWidget) {
			OnCloseButtonClickedDelegateHandle = CloseButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
				if (FVeUIModule* UIModule = FVeUIModule::Get()) {
					if (const TSharedPtr<FUIBreadcrumbSubsystem> BreadcrumbSubsystem = UIModule->GetBreadcrumbSubsystem()) {
						{
							BreadcrumbSubsystem->Pop();
						}
					} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(Breadcrumb)); }
				} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); };

				if (OnClosedDelegate.IsBound()) {
					OnClosedDelegate.Broadcast();
				}
			});
		}
	}
	                  
	if (SubmitButtonWidget) {
		if (!OnSubmitButtonClickedDelegateHandle.IsValid()) {
			OnSubmitButtonClickedDelegateHandle = SubmitButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIPlaceableUrlWidget::OnSubmitButtonClicked);
		}
	}
	
	// if (IsValid(UrlInputWidget)) {
	// 	if (!OnUrlPersonInputDelegateHandle.IsValid()) {
	// 		if (!UrlInputWidget->OnTextCommitted.IsBound()) {
	// 			OnUrlPersonInputDelegateHandle = UrlInputWidget->OnTextCommitted.AddWeakLambda(this, [this](const FText& InText, const ETextCommit::Type InCommitMethod) {
	// 				OnUrlTextCommittedCallback(InText, InCommitMethod);
	// 			});
	// 		}
	// 	}
	// }
	
	Super::RegisterCallbacks();
}

void UUIPlaceableUrlWidget::UnregisterCallbacks() {
	if (OnCloseButtonClickedDelegateHandle.IsValid()) {
		if (CloseButtonWidget) {
			CloseButtonWidget->GetOnButtonClicked().Remove(OnCloseButtonClickedDelegateHandle);
			OnCloseButtonClickedDelegateHandle.Reset();
		}
	}
	
	if (OnSubmitButtonClickedDelegateHandle.IsValid()) {
		if (SubmitButtonWidget) {
			SubmitButtonWidget->GetOnButtonClicked().Remove(OnSubmitButtonClickedDelegateHandle);
			OnSubmitButtonClickedDelegateHandle.Reset();
		}
	}
	
	if (OnUrlPersonInputDelegateHandle.IsValid()) {
		if (UrlInputWidget) {
			UrlInputWidget->OnTextCommitted.Remove(OnUrlPersonInputDelegateHandle);
			OnUrlPersonInputDelegateHandle.Reset();
		}
	}
	
	Super::UnregisterCallbacks();
}

// void UUIPlaceableUrlWidget::OnNavigate() {
// 	Super::OnNavigate();
// }

void UUIPlaceableUrlWidget::NativeConstruct() {
	//SubmitButtonWidget->SetIsEnabled(false);
	Super::NativeConstruct();
}

void UUIPlaceableUrlWidget::OnUrlTextCommittedCallback(const FText& InText, ETextCommit::Type InCommitMethod) {
	// UrlInputWidget->ValidationRegexPattern = "^" + InText.ToString() + "$";
	if (!InText.ToString().IsEmpty()) {
		// UrlInputWidget->Validate();
		if (!UrlInputWidget->Validate()) {
			SubmitButtonWidget->SetIsEnabled(false);
		} else {
			SubmitButtonWidget->SetIsEnabled(true);
		}
	}
}

void UUIPlaceableUrlWidget::OnSubmitButtonClicked() {
	if (OnClosedDelegate.IsBound()) {
		OnClosedDelegate.Broadcast();
	}

	const FString URl = UrlInputWidget->GetText().ToString();
	
	if (OnUrlChanged.IsBound()) {
		OnUrlChanged.Broadcast(URl);
	}
	
}

void UUIPlaceableUrlWidget::SetMimeType(const FString& InMimeType) {
	
}