// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#include "VR/UIObjectFormWidget.h"

#include "Components/UILoadingWidget.h"
#include "ApiFileUploadRequest.h"
#include "ApiFileUploadSubsystem.h"
#include "Subsystems/UIBreadcrumbSubsystem.h"
#include "VeHUD.h"
#include "VeApi.h"
#include "VeUI.h"
#include "VeShared.h"
#include "Subsystems/UINotificationSubsystem.h"

#define LOCTEXT_NAMESPACE "VeUI"

void UUIObjectFormWidget::SetIsProcessing(const bool bInIsLoading) const {
	if (LoadingWidget) {
		if (bInIsLoading) {
			LoadingWidget->Show();
		} else {
			LoadingWidget->Hide();
		}
	}
}

void UUIObjectFormWidget::NativeConstruct() {
	bFileBrowserWidgetVisible = true;
	SetIsProcessing(false);

	HideFileBrowserWidget(false, false);

	Super::NativeConstruct();
}

void UUIObjectFormWidget::RegisterCallbacks() {
	if (!OnCloseButtonClickedDelegateHandle.IsValid()) {
		if (CloseButtonWidget) {
			OnCloseButtonClickedDelegateHandle = CloseButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIObjectFormWidget::OnCloseButtonClicked);
		}
	}

	if (!OnCancelButtonClickedDelegateHandle.IsValid()) {
		if (CancelButtonWidget) {
			OnCancelButtonClickedDelegateHandle = CancelButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIObjectFormWidget::OnCancelButtonClicked);
		}
	}

	if (!OnSubmitButtonClickedDelegateHandle.IsValid()) {
		if (SubmitButtonWidget) {
			OnSubmitButtonClickedDelegateHandle = SubmitButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIObjectFormWidget::OnSubmitButtonClicked);
		}
	}

	if (!OnUploadButtonClickedDelegateHandle.IsValid()) {
		if (BrowseButtonWidget) {
			OnUploadButtonClickedDelegateHandle = BrowseButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIObjectFormWidget::OnBrowseButtonClicked);
		}
	}

	if (!OnFileBrowserClosedDelegateHandle.IsValid()) {
		if (FileBrowserWidget) {
			OnFileBrowserClosedDelegateHandle = FileBrowserWidget->OnClosed.AddWeakLambda(this, [this]() {
				HideFileBrowserWidget();
			});
		}
	}

	if (!OnFileBrowserFileSelectedDelegateHandle.IsValid()) {
		if (FileBrowserWidget) {
			OnFileBrowserFileSelectedDelegateHandle = FileBrowserWidget->OnFileSelected.AddUObject(this, &UUIObjectFormWidget::OnFileBrowserFileSelected);
		}
	}

	OnDialogButtonClickedDelegate.BindUObject(this, &UUIObjectFormWidget::OnCloseConfirmationDialogButtonClicked);

#pragma region  Check_change

	if (IsValid(CreateNameWidget)) {
		CreateNameWidget->OnTextCommitted.AddUObject(this, &UUIObjectFormWidget::OnNameTextCommitted);
	}

	if (IsValid(CreateDescriptionWidget)) {
		CreateDescriptionWidget->OnTextCommitted.AddUObject(this, &UUIObjectFormWidget::OnDescriptionTextCommitted);
	}

	if (IsValid(PublicCheckBoxWidget)) {
		if (!PublicCheckBoxWidget->OnCheckStateChanged.IsAlreadyBound(this, &UUIObjectFormWidget::OnCheckBoxCheckStateChanged)) {
			PublicCheckBoxWidget->OnCheckStateChanged.AddDynamic(this, &UUIObjectFormWidget::OnCheckBoxCheckStateChanged);
		}
	}

	if (IsValid(CreateArtistWidget)) {
		CreateArtistWidget->OnTextCommitted.AddUObject(this, &UUIObjectFormWidget::OnArtistTextCommitted);
	}

	if (IsValid(DateWidget)) {
		DateWidget->OnTextCommitted.AddUObject(this, &UUIObjectFormWidget::OnDateTextCommitted);
	}

	if (IsValid(MediumWidget)) {
		MediumWidget->OnTextCommitted.AddUObject(this, &UUIObjectFormWidget::OnMediumTextCommitted);
	}

	if (IsValid(SourceWidget)) {
		SourceWidget->OnTextCommitted.AddUObject(this, &UUIObjectFormWidget::OnSourceTextCommitted);
	}

	if (IsValid(SourceUrlWidget)) {
		SourceUrlWidget->OnTextCommitted.AddUObject(this, &UUIObjectFormWidget::OnSourceUrlTextCommitted);
	}

	if (IsValid(LicenseWidget)) {
		LicenseWidget->OnTextCommitted.AddUObject(this, &UUIObjectFormWidget::OnLicenseTextCommitted);
	}

	if (IsValid(CopyrightWidget)) {
		CopyrightWidget->OnTextCommitted.AddUObject(this, &UUIObjectFormWidget::OnCopyrightTextCommitted);
	}

	if (IsValid(CreditWidget)) {
		CreditWidget->OnTextCommitted.AddUObject(this, &UUIObjectFormWidget::OnCreditTextCommitted);
	}

	if (IsValid(OriginWidget)) {
		OriginWidget->OnTextCommitted.AddUObject(this, &UUIObjectFormWidget::OnOriginTextCommitted);
	}

	if (IsValid(LocationWidget)) {
		LocationWidget->OnTextCommitted.AddUObject(this, &UUIObjectFormWidget::OnLocationTextCommitted);
	}

#pragma endregion
}

void UUIObjectFormWidget::UnregisterCallbacks() {
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
	
	if (OnFileBrowserClosedDelegateHandle.IsValid()) {
		if (FileBrowserWidget) {
			FileBrowserWidget->OnClosed.Remove(OnFileBrowserClosedDelegateHandle);
			OnFileBrowserClosedDelegateHandle.Reset();
		}
	}

	if (OnFileBrowserFileSelectedDelegateHandle.IsValid()) {
		if (FileBrowserWidget) {
			FileBrowserWidget->OnFileSelected.Remove(OnFileBrowserFileSelectedDelegateHandle);
			OnFileBrowserFileSelectedDelegateHandle.Reset();
		}
	}

	OnDialogButtonClickedDelegate.Unbind();
}

void UUIObjectFormWidget::SetMetadata(const TSharedPtr<FApiObjectMetadata> InObject) {
	bIsDirty = false;

	if (!InObject.IsValid()) {
		Metadata = MakeShared<FApiObjectMetadata>();
	} else {
		Metadata = InObject;
	}

	CreateArtistWidget->SetText(FText::FromString(Metadata->Artist));
	TypeWidget->SetSelectedOption(Metadata->Type);
	CreateNameWidget->SetText(FText::FromString(Metadata->Name));
	CreateDescriptionWidget->SetText(FText::FromString(Metadata->Description));
	DateWidget->SetText(FText::FromString(Metadata->Date));
	MediumWidget->SetText(FText::FromString(Metadata->Medium));
	SourceWidget->SetText(FText::FromString(Metadata->Source));
	SourceUrlWidget->SetText(FText::FromString(Metadata->SourceUrl));
	LicenseWidget->SetText(FText::FromString(Metadata->License));
	CopyrightWidget->SetText(FText::FromString(Metadata->Copyright));
	CreditWidget->SetText(FText::FromString(Metadata->Credit));
	OriginWidget->SetText(FText::FromString(Metadata->Origin));
	LocationWidget->SetText(FText::FromString(Metadata->Location));
	PublicCheckBoxWidget->SetIsChecked(Metadata->bPublic);

	UploadFileMetadata = MakeShared<FApiUploadFileMetadata>();

	// if (!OnFileUploadedDelegateHandle.IsValid()) {
	// 	if (FVeApiModule* ApiModule = FVeApiModule::Get()) {
	// 		if (const TSharedPtr<FApiFileUploadSubsystem> FileUploadSubsystem = ApiModule->GetFileUploadSubsystem()) {
	// 			{
	// 				OnFileUploadedDelegateHandle = FileUploadSubsystem->GetOnUploaded().AddUObject(this, &UUIObjectFormWidget::OnFileUploaded);
	// 			}
	// 		} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Api), STRINGIFY(FileUpload)); }
	// 	} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Api)); };
	// }
}

void UUIObjectFormWidget::OnCloseButtonClicked() {
	if (!bIsDirty) {
		if (OnClosedDelegate.IsBound()) {
			OnClosedDelegate.Broadcast(Metadata);
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

	if (auto* HUD = GetHUD()) {
		HUD->ShowDialog(DialogData, OnDialogButtonClickedDelegate);
	}
}

void UUIObjectFormWidget::OnSubmitButtonClicked() {
	if (bIsRequestInProgress) {
		return;
	}

	bIsRequestInProgress = true;

	if (!Metadata.IsValid()) {
		return;
	}

	GET_MODULE_SUBSYSTEM(ObjectSubsystem, Api, Object);
	if (ObjectSubsystem) {
		if (!Metadata->Id.IsValid()) {
			if (SelectedFileInfo.Path.IsEmpty()) {
				return;
			}

			const auto CallbackPtr = MakeShared<FOnObjectRequestCompleted>();

			CallbackPtr->BindWeakLambda(this, [this](const FApiObjectMetadata& InMetadata, const bool bInSuccessful, const FString& InError) {
				if (!bInSuccessful) {
					LogErrorF("failed to request: %s", *InError);
				}

				// Finish processing.
				SetIsProcessing(false);

				OnObjectCreated(InMetadata, bInSuccessful, InError);
			});

			ObjectSubsystem->Create(FApiUpdateObjectMetadata(*Metadata), CallbackPtr);
		} else {
			const auto CallbackPtr = MakeShared<FOnObjectRequestCompleted>();

			CallbackPtr->BindWeakLambda(this, [this](const FApiObjectMetadata& InMetadata, const bool bInSuccessful, const FString& InError) {
				if (!bInSuccessful) {
					LogErrorF("failed to request: %s", *InError);
				}

				// Finish processing.
				SetIsProcessing(false);

				OnObjectUpdated(InMetadata, bInSuccessful, InError);
			});

			ObjectSubsystem->Update(FApiUpdateObjectMetadata(*Metadata), CallbackPtr);
		}

		SetIsProcessing(true);
	}
}

void UUIObjectFormWidget::OnCancelButtonClicked() {
	if (!bIsDirty) {
		if (OnClosedDelegate.IsBound()) {
			OnClosedDelegate.Broadcast(Metadata);
		}
		return;
	}

	FUIDialogData CloseConfirmationDialogData;
	CloseConfirmationDialogData.Type = EUIDialogType::OkCancel;
	CloseConfirmationDialogData.HeaderText = NSLOCTEXT("VeUI", "ConfirmCloseDialogHeader", "Confirm");
	CloseConfirmationDialogData.MessageText = NSLOCTEXT("VeUI", "ConfirmCloseDialogMessage", "You will lose your changes");
	CloseConfirmationDialogData.FirstButtonText = NSLOCTEXT("VeUI", "ConfirmCloseDialogButtonOk", "OK");
	CloseConfirmationDialogData.SecondButtonText = NSLOCTEXT("VeUI", "ConfirmCloseDialogButtonCancel", "Cancel");
	CloseConfirmationDialogData.bCloseOnButtonClick = true;

	FUIDialogButtonClicked OnCloseConfirmationDialogButtonClickedDelegate;
	OnCloseConfirmationDialogButtonClickedDelegate.BindUObject(this, &UUIObjectFormWidget::OnCloseConfirmationDialogButtonClicked);

	if (auto* HUD = GetHUD()) {
		HUD->ShowDialog(CloseConfirmationDialogData, OnCloseConfirmationDialogButtonClickedDelegate);
	}
}

void UUIObjectFormWidget::OnBrowseButtonClicked() {
	if (IsValid(FileBrowserWidget)) {
		FileBrowserWidget->UpdateFileList(FPaths::ProjectContentDir(), AllowedExtensions);
		ShowFileBrowserWidget();
	}
}

void UUIObjectFormWidget::OnObjectCreated(const FApiObjectMetadata& InMetadata, const bool bSuccessful, const FString& Error) {
	if (bSuccessful) {
		GET_MODULE_SUBSYSTEM(FileUploadSubsystem, Api, FileUpload);
		if (FileUploadSubsystem) {
			if (!SelectedFileInfo.Path.IsEmpty()) {
				const TSharedRef<FOnFileRequestCompleted> CallbackPtr = MakeShareable(new FOnFileRequestCompleted());

				CallbackPtr->BindWeakLambda(this, [this](const FApiFileMetadata& InMetadata, const bool bInSuccessful, const FString& InError) {
					if (!bInSuccessful) {
						LogErrorF("failed to request: %s", *InError);
					}

					// Finish processing.
					SetIsProcessing(false);

					OnFileUploaded(InMetadata, bInSuccessful, InError);
				});

				UploadFileMetadata->EntityId = InMetadata.Id;
				UploadFileMetadata->FileType = TEXT("image_full_initial");
				UploadFileMetadata->FilePath = SelectedFileInfo.Path;
				if (!FileUploadSubsystem->Upload(*UploadFileMetadata, CallbackPtr)) {
					LogErrorF("failed to request");
				} else {
					SetIsProcessing(true);
					return;
				}
			}

			GET_MODULE_SUBSYSTEM(NotificationSubsystem, UI, Notification);
			if (NotificationSubsystem) {
				FUINotificationData NotificationData;
				NotificationData.Type = EUINotificationType::Warning;
				NotificationData.Header = LOCTEXT("CreateObjectNoFileWarningNotificationHeader", "Warning");
				NotificationData.Message = LOCTEXT("CreateObjectNoFileWarningNotificationMessage", "You did not upload file for the object, so it won't be listed.");
				NotificationSubsystem->AddNotification(NotificationData);
			}
					
			LogWarningF("Failed to create an object: %s", *Error);
			return;
		}
		
		GET_MODULE_SUBSYSTEM(NotificationSubsystem, UI, Notification);
		if (NotificationSubsystem) {
			FUINotificationData NotificationData;
			NotificationData.Type = EUINotificationType::Warning;
			NotificationData.Header = LOCTEXT("CreateObjectNoFileWarningNotificationHeader", "Warning");
			NotificationData.Message = LOCTEXT("CreateObjectNoFileWarningNotificationMessage", "Failed to upload file for the object, so it won't be listed.");
			NotificationSubsystem->AddNotification(NotificationData);
		}
		
		LogWarningF("File upload subsystem is not available");
	} else {
		GET_MODULE_SUBSYSTEM(NotificationSubsystem, UI, Notification);
		if (NotificationSubsystem) {
			FUINotificationData NotificationData;
			NotificationData.Type = EUINotificationType::Failure;
			NotificationData.Header = LOCTEXT("CreateObjectErrorNotificationHeader", "Error");
			NotificationData.Message = LOCTEXT("CreateObjectErrorNotificationMessage", "Failed to create an object.");
			NotificationSubsystem->AddNotification(NotificationData);
		}
		
		LogWarningF("Failed to create an object: %s", *Error);
	}

	SetIsProcessing(false);
}

void UUIObjectFormWidget::OnObjectUpdated(const FApiObjectMetadata& /**InMetadata*/, const bool bSuccessful, const FString& Error) const {
	if (OnClosedDelegate.IsBound()) {
		OnClosedDelegate.Broadcast(Metadata);
	}

	GET_MODULE_SUBSYSTEM(NotificationSubsystem, UI, Notification);
	if (NotificationSubsystem) {
		FUINotificationData NotificationData;

		if (bSuccessful) {
			NotificationData.Type = EUINotificationType::Success;
			NotificationData.Header = LOCTEXT("UpdateObjectSuccessNotificationHeader", "Success");
			NotificationData.Message = LOCTEXT("UpdateObjectSuccessNotificationMessage", "Successfully updated an object.");
		} else {
			NotificationData.Type = EUINotificationType::Failure;
			NotificationData.Header = LOCTEXT("UpdateObjectErrorNotificationHeader", "Error");
			NotificationData.Message = LOCTEXT("UpdateObjectErrorNotificationMessage", "Failed to update an object.");
			LogWarningF("Failed to create a object: %s", *Error);
		}

		NotificationSubsystem->AddNotification(NotificationData);
	}
}

#pragma region  Check_state

void UUIObjectFormWidget::OnNameTextCommitted(const FText& InText, ETextCommit::Type InTextCommitType) {
	Metadata->Name = InText.ToString();
	bIsDirty = true;
}

void UUIObjectFormWidget::OnArtistTextCommitted(const FText& InText, ETextCommit::Type InTextCommitType) {
	Metadata->Artist = InText.ToString();
	bIsDirty = true;
}

void UUIObjectFormWidget::OnDateTextCommitted(const FText& InText, ETextCommit::Type InTextCommitType) {
	Metadata->Date = InText.ToString();
	bIsDirty = true;
}

void UUIObjectFormWidget::OnMediumTextCommitted(const FText& InText, ETextCommit::Type InTextCommitType) {
	Metadata->Medium = InText.ToString();
	bIsDirty = true;
}

void UUIObjectFormWidget::OnSourceTextCommitted(const FText& InText, ETextCommit::Type InTextCommitType) {
	Metadata->Source = InText.ToString();
	bIsDirty = true;
}

void UUIObjectFormWidget::OnSourceUrlTextCommitted(const FText& InText, ETextCommit::Type InTextCommitType) {
	Metadata->SourceUrl = InText.ToString();
	bIsDirty = true;
}

void UUIObjectFormWidget::OnLicenseTextCommitted(const FText& InText, ETextCommit::Type InTextCommitType) {
	Metadata->License = InText.ToString();
	bIsDirty = true;
}

void UUIObjectFormWidget::OnCopyrightTextCommitted(const FText& InText, ETextCommit::Type InTextCommitType) {
	Metadata->Copyright = InText.ToString();
	bIsDirty = true;
}

void UUIObjectFormWidget::OnCreditTextCommitted(const FText& InText, ETextCommit::Type InTextCommitType) {
	Metadata->Credit = InText.ToString();
	bIsDirty = true;
}

void UUIObjectFormWidget::OnOriginTextCommitted(const FText& InText, ETextCommit::Type InTextCommitType) {
	Metadata->Origin = InText.ToString();
	bIsDirty = true;
}

void UUIObjectFormWidget::OnLocationTextCommitted(const FText& InText, ETextCommit::Type InTextCommitType) {
	Metadata->Location = InText.ToString();
	bIsDirty = true;
}

void UUIObjectFormWidget::OnDescriptionTextCommitted(const FText& InText, ETextCommit::Type InTextCommitType) {
	Metadata->Description = InText.ToString();
	bIsDirty = true;
}

void UUIObjectFormWidget::OnCheckBoxCheckStateChanged(const bool bIsChecked) {
	Metadata->bPublic = bIsChecked;
	bIsDirty = true;
}

#pragma endregion

void UUIObjectFormWidget::ShowFileBrowserWidget(const bool bAnimate, const bool bPlaySound) {
	if (FVeUIModule* UIModule = FVeUIModule::Get()) {
		if (const TSharedPtr<FUIBreadcrumbSubsystem> BreadcrumbSubsystem = UIModule->GetBreadcrumbSubsystem()) {
			{
				BreadcrumbSubsystem->Push(LOCTEXT("BrowseFiles", "Browse Files"));
			}
		} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(Breadcrumb)); }
	} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); };

	if (!bFileBrowserWidgetVisible) {
		bFileBrowserWidgetVisible = true;
		if (IsValid(FileBrowserWidget)) {
			// FileBrowserWidget->ResetState();
			// FileBrowserWidget->ResetAnimationState();
			FileBrowserWidget->Show();
		}
	}
}

void UUIObjectFormWidget::HideFileBrowserWidget(const bool bAnimate, const bool bPlaySound) {
	if (FVeUIModule* UIModule = FVeUIModule::Get()) {
		if (const TSharedPtr<FUIBreadcrumbSubsystem> BreadcrumbSubsystem = UIModule->GetBreadcrumbSubsystem()) {
			{
				BreadcrumbSubsystem->Pop();
			}
		} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(Breadcrumb)); }
	} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); };

	if (bFileBrowserWidgetVisible) {
		bFileBrowserWidgetVisible = false;
		if (IsValid(FileBrowserWidget)) {
			FileBrowserWidget->Hide();
		}
	}
}

void UUIObjectFormWidget::OnCloseConfirmationDialogButtonClicked(UUIDialogWidget*, const uint8& InButtonIndex) {
	if (InButtonIndex == 0) {

		// if (OnFileUploadedDelegateHandle.IsValid()) {
		// 	if (FVeApiModule* ApiModule = FVeApiModule::Get()) {
		// 		if (const TSharedPtr<FApiFileUploadSubsystem> FileUploadSubsystem = ApiModule->GetFileUploadSubsystem()) {
		// 			{
		// 				FileUploadSubsystem->GetOnUploaded().Remove(OnFileUploadedDelegateHandle);
		// 				OnFileUploadedDelegateHandle.Reset();
		// 			}
		// 		} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Api), STRINGIFY(FileUpload)); }
		// 	} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Api)); };
		// }


		if (OnClosedDelegate.IsBound()) {
			OnClosedDelegate.Broadcast(Metadata);
		}
	}
}

void UUIObjectFormWidget::OnFileBrowserFileSelected(const FPlatformFileInfo& InFileInfo) {
	if (!InFileInfo.bIsDirectory) {
		if (AllowedExtensions.Contains(InFileInfo.Extension)) {
			SelectedFileInfo = InFileInfo;
		} else {
			GET_MODULE_SUBSYSTEM(NotificationSubsystem, UI, Notification);
			if (NotificationSubsystem) {
				FUINotificationData NotificationData;
				NotificationData.Type = EUINotificationType::Failure;
				NotificationData.Header = LOCTEXT("FileSelectedErrorNotificationHeader", "Error");
				NotificationData.Message = LOCTEXT("FileSelectedErrorNotificationMessage", "File type is not allowed.");
				NotificationSubsystem->AddNotification(NotificationData);
			}
			LogWarningF("File extension is not allowed.");
		}
	} else {
		GET_MODULE_SUBSYSTEM(NotificationSubsystem, UI, Notification);
		if (NotificationSubsystem) {
			FUINotificationData NotificationData;
			NotificationData.Type = EUINotificationType::Failure;
			NotificationData.Header = LOCTEXT("FileSelectedDirectoryErrorNotificationHeader", "Error");
			NotificationData.Message = LOCTEXT("FileSelectedDirectoryErrorNotificationMessage", "You can not upload a directory.");
			NotificationSubsystem->AddNotification(NotificationData);
		}
		LogWarningF("Directory upload is not allowed.");
	}
}

void UUIObjectFormWidget::OnFileUploaded(const FApiFileMetadata& InMetadata, const bool bSuccessful, const FString& Error) {
	SetIsProcessing(false);

	if (OnClosedDelegate.IsBound()) {
		OnClosedDelegate.Broadcast(Metadata);
	}

	GET_MODULE_SUBSYSTEM(NotificationSubsystem, UI, Notification);
	if (NotificationSubsystem) {
		FUINotificationData NotificationData;

		if (bSuccessful) {
			NotificationData.Type = EUINotificationType::Success;
			NotificationData.Header = LOCTEXT("CreateObjectSuccessNotificationHeader", "Success");
			NotificationData.Message = LOCTEXT("CreateObjectSuccessNotificationMessage", "Successfully created an object.");
		} else {
			NotificationData.Type = EUINotificationType::Failure;
			NotificationData.Header = LOCTEXT("CreateObjectErrorNotificationHeader", "Error");
			NotificationData.Message = LOCTEXT("CreateObjectErrorNotificationMessage", "Failed to create an object.");
			LogWarningF("Failed to create an object: %s", *Error);
		}

		NotificationSubsystem->AddNotification(NotificationData);
	}
}

#undef LOCTEXT_NAMESPACE
