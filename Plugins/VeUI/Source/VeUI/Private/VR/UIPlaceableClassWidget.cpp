// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "VR/UIPlaceableClassWidget.h"

#include "Subsystems/UIBreadcrumbSubsystem.h"
#include "VeUI.h"

void UUIPlaceableClassWidget::RegisterCallbacks() {
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
			OnSubmitButtonClickedDelegateHandle = SubmitButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIPlaceableClassWidget::OnSubmitButtonClicked);
		}
	}

	Super::RegisterCallbacks();
}

void UUIPlaceableClassWidget::UnregisterCallbacks() {
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
	
	// if (OnUrlPersonInputDelegateHandle.IsValid()) {
		// if (UrlInputWidget) {
			// UrlInputWidget->OnTextCommitted.Remove(OnUrlPersonInputDelegateHandle);
			// OnUrlPersonInputDelegateHandle.Reset();
		// }
	// }
	PlaceableClassComboBoxWidget->OnSelectionChanged.RemoveAll(this);

	Super::UnregisterCallbacks();
}

// void UUIPlaceableClassWidget::OnNavigate() {
// 	Super::OnNavigate();
// }

void UUIPlaceableClassWidget::OnClassSelectionChanged(FString String, ESelectInfo::Type Arg) {
	VeLogFunc("Class selection changed: %s", *String);
}

void UUIPlaceableClassWidget::NativeConstruct() {
	//SubmitButtonWidget->SetIsEnabled(false);
	for (auto Option : OptionClassMap) {
		PlaceableClassComboBoxWidget->AddOption(Option.Key);
	}

	PlaceableClassComboBoxWidget->OnSelectionChanged.AddDynamic(this, &UUIPlaceableClassWidget::OnClassSelectionChanged);

	Super::NativeConstruct();
}

// void UUIPlaceableClassWidget::OnUrlTextCommittedCallback(const FText& InText, ETextCommit::Type InCommitMethod) {
// 	UrlInputWidget->ValidationRegexPattern = "^" + InText.ToString() + "$";
// 	if (!InText.ToString().IsEmpty()) {
// 		UrlInputWidget->Validate();
// 		if (!UrlInputWidget->Validate()) {
// 			SubmitButtonWidget->SetIsEnabled(false);
// 		} else {
// 			SubmitButtonWidget->SetIsEnabled(true);
// 		}
// 	}
// }

void UUIPlaceableClassWidget::OnSubmitButtonClicked() {
	if (OnClosedDelegate.IsBound()) {
		OnClosedDelegate.Broadcast();
	}

	// const FString URl = UrlInputWidget->GetText().ToString();

	if (OnClassSelected.IsBound()) {
		const FString SelectedOption = PlaceableClassComboBoxWidget->GetSelectedOption();
		auto* SelectedClass = OptionClassMap.Find(SelectedOption);
		OnClassSelected.Broadcast(SelectedClass->Get());
	}
	
	// if (OnUrlChanged.IsBound()) {
		// OnUrlChanged.Broadcast(URl);
	// }
	
}

void UUIPlaceableClassWidget::SetMimeType(const FString& InMimeType) {
	
}