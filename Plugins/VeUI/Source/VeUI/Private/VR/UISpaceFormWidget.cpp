// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "VR/UISpaceFormWidget.h"

#include "VeHUD.h"
#include "VeApi.h"
#include "VeApi2.h"
#include "VeShared.h"
#include "VeUI.h"
#include "GeometryCollection/GeometryCollectionSimulationCoreTypes.h"
#include "Api2WorldSubsystem.h"

#define LOCTEXT_NAMESPACE "VeUI"

void UUISpaceFormWidget::RegisterCallbacks() {
	if (IsValid(CloseButtonWidget)) {
		if (!OnCloseButtonClickedDelegateHandle.IsValid()) {
			OnCloseButtonClickedDelegateHandle = CloseButtonWidget->GetOnButtonClicked().AddUObject(this, &UUISpaceFormWidget::OnCloseButtonClicked);
		}
	}

	if (IsValid(CancelButtonWidget)) {
		if (!OnCancelButtonClickedDelegateHandle.IsValid()) {
			OnCancelButtonClickedDelegateHandle = CancelButtonWidget->GetOnButtonClicked().AddUObject(this, &UUISpaceFormWidget::OnCancelButtonClicked);
		}
	}

	if (IsValid(SubmitButtonWidget)) {
		if (!OnSubmitButtonClickedDelegateHandle.IsValid()) {
			OnSubmitButtonClickedDelegateHandle = SubmitButtonWidget->GetOnButtonClicked().AddUObject(this, &UUISpaceFormWidget::SaveMetadata);
		}
	}

	// if (FVeApiModule* ApiModule = FVeApiModule::Get()) {
	// 	if (const TSharedPtr<FApiSpaceSubsystem> SpaceSubsystem = ApiModule->GetSpaceSubsystem()) {
	// 		{
	// 			if (!OnSpaceCreatedDelegateHandle.IsValid()) {
	// 				OnSpaceCreatedDelegateHandle = SpaceSubsystem->GetOnCreated().AddUObject(this, &UUISpaceFormWidget::OnSpaceCreated);
	// 			}
	// 			if (!OnSpaceUpdatedDelegateHandle.IsValid()) {
	// 				OnSpaceUpdatedDelegateHandle = SpaceSubsystem->GetOnUpdated().AddUObject(this, &UUISpaceFormWidget::OnSpaceUpdated);
	// 			}
	// 		}
	// 	} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Api), STRINGIFY(Space)); }
	// } else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Api)); };

	if (IsValid(CreateNameWidget)) {
		CreateNameWidget->OnTextCommitted.AddUObject(this, &UUISpaceFormWidget::OnNameTextCommitted);
	}

	if (IsValid(CreateDescriptionWidget)) {
		CreateDescriptionWidget->OnTextCommitted.AddUObject(this, &UUISpaceFormWidget::OnDescriptionTextCommitted);
	}

	if (IsValid(PublicCheckBoxWidget)) {
		if (!PublicCheckBoxWidget->OnCheckStateChanged.IsAlreadyBound(this, &UUISpaceFormWidget::OnCheckBoxCheckStateChanged)) {
			PublicCheckBoxWidget->OnCheckStateChanged.AddDynamic(this, &UUISpaceFormWidget::OnCheckBoxCheckStateChanged);
		}
	}

}

void UUISpaceFormWidget::UnregisterCallbacks() {
	if (OnCloseButtonClickedDelegateHandle.IsValid()) {
		if (CloseButtonWidget) {
			CloseButtonWidget->GetOnButtonClicked().Remove(OnCloseButtonClickedDelegateHandle);
			OnCloseButtonClickedDelegateHandle.Reset();
		}
	}

	if (OnCancelButtonClickedDelegateHandle.IsValid()) {
		if (CancelButtonWidget) {
			CancelButtonWidget->GetOnButtonClicked().Remove(OnCancelButtonClickedDelegateHandle);
			OnCancelButtonClickedDelegateHandle.Reset();
		}
	}

	if (OnSubmitButtonClickedDelegateHandle.IsValid()) {
		if (SubmitButtonWidget) {
			SubmitButtonWidget->GetOnButtonClicked().Remove(OnSubmitButtonClickedDelegateHandle);
			OnSubmitButtonClickedDelegateHandle.Reset();
		}
	}

	// if (FVeApiModule* ApiModule = FVeApiModule::Get()) {
	// 	if (const TSharedPtr<FApiSpaceSubsystem> SpaceSubsystem = ApiModule->GetSpaceSubsystem()) {
	// 		{
	// 			if (OnSpaceCreatedDelegateHandle.IsValid()) {
	// 				SpaceSubsystem->GetOnCreated().Remove(OnSpaceCreatedDelegateHandle);
	// 				OnSpaceCreatedDelegateHandle.Reset();
	// 			}
	// 			if (OnSpaceUpdatedDelegateHandle.IsValid()) {
	// 				SpaceSubsystem->GetOnUpdated().Remove(OnSpaceUpdatedDelegateHandle);
	// 				OnSpaceUpdatedDelegateHandle.Reset();
	// 			}
	// 		}
	// 	} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Api), STRINGIFY(Space)); }
	// } else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Api)); };
}

void UUISpaceFormWidget::NativeConstruct() {
	Super::NativeConstruct();
}

void UUISpaceFormWidget::SetMetadata(const TSharedPtr<FApiSpaceMetadata> InSpace) {
	bIsDirty = false;

	MapWidget->AddOption(TEXT("/ArtheonGallery/Showcase/Maps/ArtGallery"));

	if (!InSpace.IsValid()) {
		Metadata = MakeShared<FApiSpaceMetadata>();
	} else {
		Metadata = InSpace;
	}

	if (!Metadata->Map.IsEmpty()) {
		MapWidget->SetSelectedOption(Metadata->Map);
	} else {
		MapWidget->SetSelectedIndex(0);
		Metadata->Map = MapWidget->GetOptionAtIndex(0);
	}

	// if (Metadata->Type.IsEmpty()) {
	// 	Metadata->Type = TEXT("artheon");
	// }

	CreateNameWidget->SetText(FText::FromString(Metadata->Name));
	CreateDescriptionWidget->SetText(FText::FromString(Metadata->Description));
	PublicCheckBoxWidget->SetIsChecked(Metadata->bPublic);
}

void UUISpaceFormWidget::SaveMetadata() {
	if (bIsRequestInProgress) {
		return;
	}

	bIsRequestInProgress = true;

	if (!Metadata.IsValid()) {
		return;
	}

	if (!Metadata->Id.IsValid()) {
		GET_MODULE_SUBSYSTEM(SpaceSubsystem, Api2, World);
		if (SpaceSubsystem) {
			const auto CallbackPtr = MakeShared<FOnWorldRequestCompleted2>();
			CallbackPtr->BindWeakLambda(this, [this, CallbackPtr](const FApiSpaceMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError) {
				const bool bSuccessful = InResponseCode == EApi2ResponseCode::Ok;
				if (!bSuccessful) {
					VeLogErrorFunc("Failed to request: %s", *InError);
				}
				// Finish processing.
				// SetIsProcessing(false);
				OnSpaceCreated(InMetadata, bSuccessful, InError);
				bIsRequestInProgress = false;
			});

			SpaceSubsystem->Create(FApiUpdateSpaceMetadata(*Metadata), CallbackPtr);
		}
	} else {
		GET_MODULE_SUBSYSTEM(SpaceSubsystem, Api, Space);
		if (SpaceSubsystem) {
			const auto CallbackPtr = MakeShared<FOnSpaceRequestCompleted>();
			CallbackPtr->BindWeakLambda(this, [this, CallbackPtr](const FApiSpaceMetadata& InMetadata, const bool bInSuccessful, const FString& InError) {
				if (!bInSuccessful) {
					VeLogErrorFunc("Failed to request: %s", *InError);
				}
				// Finish processing.
				// SetIsProcessing(false);
				OnSpaceUpdated(InMetadata, bInSuccessful, InError);
				bIsRequestInProgress = false;
			});

			if (!SpaceSubsystem->Update(FApiUpdateSpaceMetadata(*Metadata), CallbackPtr)) {
				CallbackPtr->ExecuteIfBound({}, false, TEXT("not processed"));
			}
		}
	}
}

void UUISpaceFormWidget::OnSpaceCreated(const FApiSpaceMetadata& /**InMetadata*/, const bool bSuccessful, const FString& Error) {
	if (OnClosed.IsBound()) {
		OnClosed.Broadcast(Metadata);
	}
	Metadata = nullptr;
	bIsDirty = false;

	FUINotificationData NotificationData;
	NotificationData.Lifetime = 3.f;
	if (bSuccessful) {
		NotificationData.Header = LOCTEXT("CreateSpaceSuccessNotificationHeader", "Success");
		NotificationData.Type = EUINotificationType::Success;
		NotificationData.Message = LOCTEXT("CreateSpaceSuccessNotificationMessage", "Successfully created a space.");
	} else {
		NotificationData.Header = LOCTEXT("CreateSpaceErrorNotificationHeader", "Error");
		NotificationData.Type = EUINotificationType::Failure;
		NotificationData.Message = LOCTEXT("CreateSpaceErrorNotificationMessage", "Failed to create a space.");
		LogWarningF("Failed to create a space: %s", *Error);
	}

	AddNotification(NotificationData);
}

void UUISpaceFormWidget::OnSpaceUpdated(const FApiSpaceMetadata& InMetadata, const bool bSuccessful, const FString& Error) {
	if (OnClosed.IsBound()) {
		OnClosed.Broadcast(Metadata);
	}

	Metadata = nullptr;
	bIsDirty = false;

	FUINotificationData NotificationData;
	NotificationData.Lifetime = 3.f;
	if (bSuccessful) {
		NotificationData.Header = LOCTEXT("UpdateSpaceSuccessNotificationHeader", "Success");
		NotificationData.Type = EUINotificationType::Success;
		NotificationData.Message = LOCTEXT("UpdateSpaceSuccessNotificationMessage", "Successfully updated a space.");
	} else {
		NotificationData.Header = LOCTEXT("UpdateSpaceErrorNotificationHeader", "Error");
		NotificationData.Type = EUINotificationType::Failure;
		NotificationData.Message = LOCTEXT("UpdateSpaceErrorNotificationMessage", "Failed to update a space.");
		LogWarningF("Failed to create a space: %s", *Error);
	}

	AddNotification(NotificationData);
}

void UUISpaceFormWidget::OnCloseButtonClicked() {
	if (!bIsDirty) {
		if (OnClosed.IsBound()) {
			OnClosed.Broadcast(Metadata);
		}
		Metadata = nullptr;
		bIsDirty = false;
		return;
	}

	FUIDialogData DialogData;
	DialogData.Type = EUIDialogType::OkCancel;
	DialogData.HeaderText = NSLOCTEXT("VeUI", "ConfirmCloseDialogHeader", "Confirm");
	DialogData.MessageText = NSLOCTEXT("VeUI", "ConfirmCloseDialogMessage", "You will lose your changes");
	DialogData.FirstButtonText = NSLOCTEXT("VeUI", "ConfirmCloseDialogButtonOk", "OK");
	DialogData.SecondButtonText = NSLOCTEXT("VeUI", "ConfirmCloseDialogButtonCancel", "Cancel");
	DialogData.bCloseOnButtonClick = true;

	FUIDialogButtonClicked OnDialogButtonClicked;
	OnDialogButtonClicked.BindUObject(this, &UUISpaceFormWidget::OnDialogWidget);

	const auto _ = GetHUD()->ShowDialog(DialogData, OnDialogButtonClicked);
}

void UUISpaceFormWidget::OnCancelButtonClicked() {
	if (!bIsDirty) {
		if (OnClosed.IsBound()) {
			OnClosed.Broadcast(Metadata);
		}
		return;
	}

	FUIDialogData DialogData;
	DialogData.Type = EUIDialogType::OkCancel;
	DialogData.HeaderText = NSLOCTEXT("VeUI", "ConfirmCloseDialogHeader", "Confirm");
	DialogData.MessageText = NSLOCTEXT("VeUI", "ConfirmCloseDialogMessage", "You will lose your changes");
	DialogData.FirstButtonText = NSLOCTEXT("VeUI", "ConfirmCloseDialogButtonOk", "OK");
	DialogData.SecondButtonText = NSLOCTEXT("VeUI", "ConfirmCloseDialogButtonCancel", "Cancel");
	DialogData.bCloseOnButtonClick = true;

	FUIDialogButtonClicked OnDialogButtonClicked;
	OnDialogButtonClicked.BindUObject(this, &UUISpaceFormWidget::OnDialogWidget);

	const auto _ = GetHUD()->ShowDialog(DialogData, OnDialogButtonClicked);
}

void UUISpaceFormWidget::OnNameTextCommitted(const FText& InText, ETextCommit::Type InTextCommitType) {
	Metadata->Name = InText.ToString();
	bIsDirty = true;
}

void UUISpaceFormWidget::OnDialogWidget(UUIDialogWidget*, const uint8& InButtonIndex) {
	if (InButtonIndex == 0) {
		if (OnClosed.IsBound()) {
			OnClosed.Broadcast(Metadata);
		}
	}
}

void UUISpaceFormWidget::OnDescriptionTextCommitted(const FText& InText, ETextCommit::Type InTextCommitType) {
	Metadata->Description = InText.ToString();
	bIsDirty = true;
}

void UUISpaceFormWidget::OnCheckBoxCheckStateChanged(const bool bIsChecked) {
	Metadata->bPublic = bIsChecked;
	bIsDirty = true;
}


#undef LOCTEXT_NAMESPACE
