// Authors: Khusan T.Yadgarov, Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UIAvatarFormPageWidget.h"

#include "Subsystems/UINotificationSubsystem.h"
#include "ApiFileUploadSubsystem.h"
#include "VeHUD.h"
#include "UINotificationData.h"
#include "UIPageManagerWidget.h"
#include "VeApi.h"
#include "Animation/UMGSequencePlayer.h"
#include "Subsystems/UICharacterCustomizationSubsystem.h"
#include "VeShared.h"
#include "VeUI.h"

#define LOCTEXT_NAMESPACE "VeUI"


UUIAvatarFormPageWidget::UUIAvatarFormPageWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {

	bNoHistory = true;
}

void UUIAvatarFormPageWidget::NativeConstruct() {
	Super::NativeConstruct();
	// SubmitButtonWidget->SetIsEnabled(false);
}

void UUIAvatarFormPageWidget::RegisterCallbacks() {
	Super::RegisterCallbacks();

	if (!OnCancelButtonClickedDelegateHandle.IsValid()) {
		if (CancelButtonWidget) {
			OnCancelButtonClickedDelegateHandle = CancelButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
				if (IsValid(GetPageManager())) GetPageManager()->ShowPrevPage();
			});
		}
	}

	if (!OnSubmitButtonClickedDelegateHandle.IsValid()) {
		if (SubmitButtonWidget) {
			OnSubmitButtonClickedDelegateHandle = SubmitButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIAvatarFormPageWidget::CreatePersonaRequest);
		}
	}

	if (!OnUrlPersonInputDelegateHandle.IsValid()) {
		if (IsValid(UrlInputWidget)) {
			// if (!UrlInputWidget->OnTextCommitted.IsBound()) {
			OnUrlPersonInputDelegateHandle = UrlInputWidget->OnTextCommitted.AddWeakLambda(this, [this](const FText& InText, const ETextCommit::Type InCommitMethod) {
				OnUrlTextCommittedCallback(InText, InCommitMethod);
			});
			// }
		}
	}
}

void UUIAvatarFormPageWidget::UnregisterCallbacks() {
	Super::UnregisterCallbacks();

	if (OnCancelButtonClickedDelegateHandle.IsValid()) {
		if (CancelButtonWidget) {
			CancelButtonWidget->GetOnButtonClicked().Remove(OnCancelButtonClickedDelegateHandle);
		}
		OnCancelButtonClickedDelegateHandle.Reset();
	}

	if (OnSubmitButtonClickedDelegateHandle.IsValid()) {
		if (SubmitButtonWidget) {
			SubmitButtonWidget->GetOnButtonClicked().Remove(OnSubmitButtonClickedDelegateHandle);
		}
		OnSubmitButtonClickedDelegateHandle.Reset();
	}

	if (OnUrlPersonInputDelegateHandle.IsValid()) {
		if (UrlInputWidget) {
			UrlInputWidget->OnTextCommitted.Remove(OnUrlPersonInputDelegateHandle);
		}
		OnUrlPersonInputDelegateHandle.Reset();
	}
}

void UUIAvatarFormPageWidget::SetMetadata(const FApiPersonaMetadata& InMetadata) {
	Metadata = InMetadata;

	NameInputWidget->SetText(FText::FromString(InMetadata.Name));

	const auto File = InMetadata.Files.FindByPredicate([=](const FApiFileMetadata& File) {
		return File.Type == EApiFileType::MeshAvatar;
	});

	if (File) {
		UrlInputWidget->SetText(FText::FromString(File->Url));
	} else {
		UrlInputWidget->SetText(FText());
	}
}

void UUIAvatarFormPageWidget::OnUrlTextCommittedCallback(const FText& InText, ETextCommit::Type InCommitMethod) {
	if (UrlInputWidget) {
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
}

void UUIAvatarFormPageWidget::CreatePersonaRequest() {
	GET_MODULE_SUBSYSTEM(PersonaSubsystem, Api, Persona);
	if (PersonaSubsystem) {
		FApiCreatePersonaMetadata RequestMetadata;
		RequestMetadata.Name = NameInputWidget->GetText().ToString();
		RequestMetadata.Configuration = TEXT("{}");
		RequestMetadata.Type = Api::Fields::RPM;

		TSharedPtr<FOnPersonaRequestCompleted> CallbackPtr = MakeShareable(new FOnPersonaRequestCompleted());
		CallbackPtr->BindWeakLambda(this, [this, CallbackPtr](const FApiPersonaMetadata& InMetadata, const bool bInSuccessful, const FString& InError) mutable {
			if (!bInSuccessful) {
				LogErrorF("failed to request: %s", *InError);
			}

			// SetIsProcessing(false);
			OnCreatePersonaResponse(InMetadata, bInSuccessful, InError);
			CallbackPtr.Reset();
		});

		if (PersonaSubsystem->PostPersonas(RequestMetadata, CallbackPtr)) {
			// SetIsProcessing(true);
			LogF("request successful");
		} else {
			LogErrorF("failed to request");
		}
	}
}

void UUIAvatarFormPageWidget::OnCreatePersonaResponse(const FApiPersonaMetadata& InMetadata, const bool bInSuccessful, const FString& InError) {
	if (bInSuccessful) {
		SetFilePersonaRequest(InMetadata);
		//SetDefaultPersona(InMetadata);
	}

	// GET_MODULE_SUBSYSTEM(CharacterCustomizationSubsystem, UI, CharacterCustomization);
	// if (CharacterCustomizationSubsystem) {
	// 	CharacterCustomizationSubsystem->NotifyCreatePersonaRequest(InMetadata);
	// }

	GET_MODULE_SUBSYSTEM(NotificationSubsystem, UI, Notification);
	if (NotificationSubsystem) {
		FUINotificationData NotificationData;
		if (bInSuccessful) {
			NotificationData.Type = EUINotificationType::Success;
			NotificationData.Header = LOCTEXT("PersonaSuccessNotificationHeader", "Success");
			NotificationData.Message = LOCTEXT("PersonaSuccessNotificationMessage", "The persona is successfully created.");
		} else {
			NotificationData.Type = EUINotificationType::Failure;
			NotificationData.Header = LOCTEXT("PersonaErrorNotificationHeader", "Error");
			NotificationData.Message = LOCTEXT("PersonaErrorNotificationMessage", "Failed to create Persona.");
			LogWarningF("Failed to create a space: %s", *InError);
		}
		NotificationSubsystem->AddNotification(NotificationData);
	}

	if (bInSuccessful) {
		GetPageManager()->ShowPrevPage(true);
	}

	// if (UrlInputWidget) {
	// 	UrlInputWidget->SetText(FText());
	// 	NameInputWidget->SetText(FText());
	// 	SubmitButtonWidget->SetIsEnabled(false);
	// }
}

void UUIAvatarFormPageWidget::SetFilePersonaRequest(const FApiPersonaMetadata& InPersonaMetadata) {

	GET_MODULE_SUBSYSTEM(FileUploadSubsystem, Api, FileUpload);
	if (FileUploadSubsystem) {
		FApiEntityFileLinkMetadata EntityFileLinkMetadata;
		EntityFileLinkMetadata.EntityId = InPersonaMetadata.Id;
		EntityFileLinkMetadata.File = UrlInputWidget->GetText().ToString();
		EntityFileLinkMetadata.FileType = Api::FileTypes::MeshAvatar;
		EntityFileLinkMetadata.MimeType = TEXT("model/gltf-binary");

		TSharedPtr<FOnFileRequestCompleted> CallbackPtr = MakeShareable(new FOnFileRequestCompleted());
		CallbackPtr->BindWeakLambda(this, [this, InPersonaMetadata, CallbackPtr](const FApiFileMetadata& InFileMetadata, const bool bInSuccessful, const FString& InError) mutable {
			if (!bInSuccessful) {
				LogErrorF("failed to request: %s", *InError);
			}

			// SetIsProcessing(false);
			OnSetFilePersonaResponse(InPersonaMetadata, InFileMetadata, bInSuccessful, InError);
			CallbackPtr.Reset();
		});

		if (FileUploadSubsystem->Replace(EntityFileLinkMetadata, CallbackPtr)) {
			// SetIsProcessing(true);
			LogF("request successful");
			return;
		} else {
			LogErrorF("failed to request");
		}
	}
}

void UUIAvatarFormPageWidget::OnSetFilePersonaResponse(const FApiPersonaMetadata& InPersonaMetadata, const FApiFileMetadata& InFileMetadata, const bool bInSuccessful,
													   const FString& InError) const {
	GET_MODULE_SUBSYSTEM(CharacterCustomizationSubsystem, UI, CharacterCustomization);
	if (CharacterCustomizationSubsystem) {
		CharacterCustomizationSubsystem->NotifyPersonaCreated(InFileMetadata);
	}

	GET_MODULE_SUBSYSTEM(NotificationSubsystem, UI, Notification);
	if (NotificationSubsystem) {
		FUINotificationData NotificationData;
		if (bInSuccessful) {
			NotificationData.Type = EUINotificationType::Success;
			NotificationData.Header = LOCTEXT("AvatarUploadSuccessNotificationHeader", "Success");
			NotificationData.Message = LOCTEXT("AvatarUploadSuccessNotificationMessage", "Avatar file uploaded successfully");
		} else {
			NotificationData.Type = EUINotificationType::Failure;
			NotificationData.Header = LOCTEXT("AvatarUploadErrorNotificationHeader", "Error");
			NotificationData.Message = LOCTEXT("AvatarUploadErrorNotificationMessage", "Failed to upload an avatar.");
			LogWarningF("Failed to create a space: %s", *InError);
		}
		NotificationSubsystem->AddNotification(NotificationData);
	}
}


#undef LOCTEXT_NAMESPACE
