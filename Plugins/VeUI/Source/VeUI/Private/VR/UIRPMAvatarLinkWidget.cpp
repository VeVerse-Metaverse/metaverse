// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "VR/UIRPMAvatarLinkWidget.h"

#include "ApiFileUploadSubsystem.h"
#include "UINotificationData.h"
#include "VeApi.h"
#include "Animation/UMGSequencePlayer.h"
#include "Subsystems/UICharacterCustomizationSubsystem.h"
#include "VeShared.h"
#include "VeUI.h"

#define LOCTEXT_NAMESPACE "VeUI"

void UUIRPMAvatarLinkWidget::RegisterCallbacks() {
	if (IsValid(CloseButtonWidget)) {
		if (!OnCloseButtonWidgetDelegateHandle.IsValid()) {
			OnCloseButtonWidgetDelegateHandle = CloseButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
				if (OnClosedDelegate.IsBound()) {
					OnClosedDelegate.Broadcast();
				}
			});
		}
	}

	if (CancelButtonWidget) {
		if (!OnCancelButtonClickedDelegateHandle.IsValid()) {
			OnCancelButtonClickedDelegateHandle = CancelButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
				if (OnClosedDelegate.IsBound()) {
					OnClosedDelegate.Broadcast();
				}
			});
		}
	}

	if (SubmitButtonWidget) {
		if (!OnSubmitButtonClickedDelegateHandle.IsValid()) {
			OnSubmitButtonClickedDelegateHandle = SubmitButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIRPMAvatarLinkWidget::UpdateAvatarFiles);
		}
	}

	if (CreateButtonWidget) {
		if (!OnCreateButtonClickedDelegateHandle.IsValid()) {
			OnCreateButtonClickedDelegateHandle = CreateButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIRPMAvatarLinkWidget::CreatePersona);
		}
	}

	if (IsValid(UrlWidget)) {
		if (!OnUrlPersonInputDelegateHandle.IsValid()) {
			if (!UrlWidget->OnTextCommitted.IsBound()) {
				OnUrlPersonInputDelegateHandle = UrlWidget->OnTextCommitted.AddWeakLambda(this, [this](const FText& InText, const ETextCommit::Type InCommitMethod) {
					OnUrlTextCommittedCallback(InText, InCommitMethod);
				});
			}
		}
	}
}


void UUIRPMAvatarLinkWidget::UnregisterCallbacks() {
	if (OnCloseButtonWidgetDelegateHandle.IsValid()) {
		if (CloseButtonWidget) {
			CloseButtonWidget->GetOnButtonClicked().Remove(OnCloseButtonWidgetDelegateHandle);
			OnCloseButtonWidgetDelegateHandle.Reset();
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

	if (OnCreateButtonClickedDelegateHandle.IsValid()) {
		if (CreateButtonWidget) {
			CreateButtonWidget->GetOnButtonClicked().Remove(OnCreateButtonClickedDelegateHandle);
			OnCreateButtonClickedDelegateHandle.Reset();
		}
	}

	if (OnUrlPersonInputDelegateHandle.IsValid()) {
		if (UrlWidget) {
			UrlWidget->OnTextCommitted.Remove(OnUrlPersonInputDelegateHandle);
			OnUrlPersonInputDelegateHandle.Reset();
		}
	}
	
}

void UUIRPMAvatarLinkWidget::NativeConstruct() {
	CreateButtonWidget->SetIsEnabled(false);
	
	Super::NativeConstruct();
}

void UUIRPMAvatarLinkWidget::CreatePersona() {
	if (OnCreateDelegate.IsBound()) {
		OnCreateDelegate.Broadcast();
	}

	if (FVeApiModule* ApiModule = FVeApiModule::Get()) {
		if (const TSharedPtr<FApiPersonaSubsystem> PersonaSubsystem = ApiModule->GetPersonaSubsystem()) {
			if (const TSharedPtr<FApiAuthSubsystem> AuthSubsystem = ApiModule->GetAuthSubsystem()) {
				if (AuthSubsystem) {
					//auto UserId = AuthSubsystem->GetUserId();
					FApiCreatePersonaMetadata RequestMetadata;
					//RequestMetadata.Id = UserId;
					RequestMetadata.Name = PersonaNameWidget->GetText().ToString();
					RequestMetadata.Configuration = TEXT("{}");
					RequestMetadata.Type = Api::Fields::RPM;
					TSharedPtr<FOnPersonaRequestCompleted> CallbackPtr = MakeShareable(new FOnPersonaRequestCompleted());
					CallbackPtr->BindWeakLambda(this, [this, CallbackPtr](const FApiPersonaMetadata& InMetadata, const bool bInSuccessful, const FString& InError) mutable {
						if (!bInSuccessful) {
							LogErrorF("failed to request: %s", *InError);
						}
						// Finish processing.
						// SetIsProcessing(false);
						SuccessCreatePersona(InMetadata, bInSuccessful, InError);
						CallbackPtr.Reset();
					});

					if (PersonaSubsystem->PostPersonas(RequestMetadata, CallbackPtr)) {
						LogF("request successful");
					}
				}
			} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Api), STRINGIFY(FileUpload)); }
		} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Api)); }
	}
}

void UUIRPMAvatarLinkWidget::UpdateFilePersona(const FApiPersonaMetadata& InMetadata) {

	if (FVeApiModule* ApiModule = FVeApiModule::Get()) {
		if (const TSharedPtr<FApiFileUploadSubsystem> FileUploadSubsystem = ApiModule->GetFileUploadSubsystem()) {
			FApiEntityFileLinkMetadata EntityFileLinkMetadata;
			EntityFileLinkMetadata.EntityId = InMetadata.Id;
			EntityFileLinkMetadata.File = UrlWidget->GetText().ToString();
			EntityFileLinkMetadata.FileType = Api::FileTypes::MeshAvatar;
			EntityFileLinkMetadata.MimeType = TEXT("model/gltf-binary");
			TSharedPtr<FOnFileRequestCompleted> CallbackPtr = MakeShareable(new FOnFileRequestCompleted());
			CallbackPtr->BindWeakLambda(this, [this, CallbackPtr](const FApiFileMetadata& InMetadata, const bool bInSuccessful, const FString& InError) mutable {
				if (!bInSuccessful) {
					LogErrorF("failed to request: %s", *InError);
				}
				// Finish processing.
				// SetIsProcessing(false);
				SuccessUpdateAvatarFiles(InMetadata, bInSuccessful, InError);
				CallbackPtr.Reset();
			});

			if (FileUploadSubsystem->Replace(EntityFileLinkMetadata, CallbackPtr)) {
				LogF("request successful");
			}
		} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Api), STRINGIFY(FileUpload)); }
	} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Api)); }
}
//
// void UUIRPMAvatarLinkWidget::SetDefaultPersona(const FApiPersonaMetadata& InMetadata) {
// 	
// 	if (FVeApiModule* ApiModule = FVeApiModule::Get()) {
// 		if (const TSharedPtr<FApiPersonaSubsystem> PersonaSubsystem = ApiModule->GetPersonaSubsystem()) {
// 			if (const TSharedPtr<FApiAuthSubsystem> AuthSubsystem = ApiModule->GetAuthSubsystem()) {
// 				auto PersonaId = InMetadata.Id;
// 				TSharedPtr<FOnPersonaRequestCompleted> CallbackPtr = MakeShareable(new FOnPersonaRequestCompleted());
// 				CallbackPtr->BindWeakLambda(this, [this, CallbackPtr](const FApiPersonaMetadata& InMetadata, const bool bInSuccessful, const FString& InError) mutable {
// 					if (!bInSuccessful) {
// 						LogErrorF("failed to request: %s", *InError);
// 					}
// 					// Finish processing.
// 					// SetIsProcessing(false);
// 					SuccessPostDefaultAvatarFiles(bInSuccessful, InError);
// 					CallbackPtr.Reset();
// 				});
//
// 				if (PersonaSubsystem->PostDefaultPersonas(PersonaId, CallbackPtr)) {
// 					LogF("request successful");
// 				}
// 			} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Api), STRINGIFY(FileUpload)); }
// 		} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Api)); }
// 	}
// }

void UUIRPMAvatarLinkWidget::UpdateAvatarFiles() {
	//
	// if (OnSubmitDelegate.IsBound()) {
	// 	OnSubmitDelegate.Broadcast();
	// }
	//
	// if (FVeApiModule* ApiModule = FVeApiModule::Get()) {
	// 	if (const TSharedPtr<FApiFileUploadSubsystem> FileUploadSubsystem = ApiModule->GetFileUploadSubsystem()) {
	// 		FApiEntityFileLinkMetadata EntityFileLinkMetadata;
	// 		EntityFileLinkMetadata.Id = Metadata.Id;
	// 		EntityFileLinkMetadata.File = UrlWidget->GetText().ToString();
	// 		EntityFileLinkMetadata.FileType = Api::FileTypes::MeshAvatar;
	// 		EntityFileLinkMetadata.MimeType = TEXT("model/gltf-binary");
	// 		TSharedPtr<FOnFileRequestCompleted> CallbackPtr = MakeShareable(new FOnFileRequestCompleted());
	// 		CallbackPtr->BindWeakLambda(this, [this, CallbackPtr](const FApiFileMetadata& InMetadata, const bool bInSuccessful, const FString& InError) mutable {
	// 			if (!bInSuccessful) {
	// 				LogErrorF("failed to request: %s", *InError);
	// 			}
	// 			// Finish processing.
	// 			// SetIsProcessing(false);
	// 			SuccessUpdateAvatarFiles(InMetadata, bInSuccessful, InError);
	// 			CallbackPtr.Reset();
	// 		});
	//
	// 		if (FileUploadSubsystem->Replace(EntityFileLinkMetadata, CallbackPtr)) {
	// 			LogF("request successful");
	// 		}
	// 	} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Api), STRINGIFY(FileUpload)); }
	// } else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Api)); }
}

void UUIRPMAvatarLinkWidget::SetMetadata(const FApiUserMetadata& InMetadata) {
	Metadata = InMetadata;
}

void UUIRPMAvatarLinkWidget::OnUrlTextCommittedCallback(const FText& InText, ETextCommit::Type InCommitMethod) {
	//UrlWidget->ValidationRegexPattern = "^" + InText.ToString() + "$";
	if (!InText.ToString().IsEmpty()) {
	//	UrlWidget->Validate();
		if (!UrlWidget->Validate()) {
			CreateButtonWidget->SetIsEnabled(false);
		} else {
			CreateButtonWidget->SetIsEnabled(true);
		}
	}
}

void UUIRPMAvatarLinkWidget::ShowMessage() {
	if (MessageFadeInSound) {
		PlaySound(MessageFadeInSound);
	}

	if (MessageFadeInAnimation) {
		UUMGSequencePlayer* SequencePlayer = PlayAnimationForward(MessageFadeInAnimation);
		if (SequencePlayer) {
			SequencePlayer->OnSequenceFinishedPlaying().AddWeakLambda(this, [this](UUMGSequencePlayer& /*InSequencePlayer*/) {
				MessageWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				MessageWidget->SetText(MessageText);
			});
			return;
		}
	}

	MessageWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	MessageWidget->SetText(MessageText);
}

void UUIRPMAvatarLinkWidget::HideMessage() {
	if (MessageFadeOutSound) {
		PlaySound(MessageFadeOutSound);
	}

	if (MessageFadeOutAnimation) {
		UUMGSequencePlayer* SequencePlayer = PlayAnimationForward(MessageFadeOutAnimation);
		if (SequencePlayer) {
			SequencePlayer->OnSequenceFinishedPlaying().AddWeakLambda(this, [this](UUMGSequencePlayer& /*InSequencePlayer*/) {
				MessageWidget->SetVisibility(ESlateVisibility::Collapsed);
				MessageWidget->SetText(MessageText);
			});
			return;
		}
	} else {
		UUMGSequencePlayer* SequencePlayer = PlayAnimationReverse(MessageFadeInAnimation);
		if (SequencePlayer) {
			SequencePlayer->OnSequenceFinishedPlaying().AddWeakLambda(this, [this](UUMGSequencePlayer& /*InSequencePlayer*/) {
				MessageWidget->SetVisibility(ESlateVisibility::Collapsed);
				MessageWidget->SetText(MessageText);
			});
			return;
		}
	}

	MessageWidget->SetVisibility(ESlateVisibility::Collapsed);
	MessageWidget->SetText(MessageText);
}

void UUIRPMAvatarLinkWidget::SetMessageText(const FText& InText) {
	MessageText = InText;

	if (MessageWidget) {
		if (MessageText.IsEmpty()) {
			HideMessage();
		} else {
			ShowMessage();
		}
	}
}

UUMGSequencePlayer* UUIRPMAvatarLinkWidget::Hide(bool bAnimate, bool bPlaySound) {
	SetMessageText(FText());

	return Super::Hide(bAnimate, bPlaySound);
}

void UUIRPMAvatarLinkWidget::SuccessUpdateAvatarFiles(const FApiFileMetadata& InMetadata, const bool bInSuccessful, const FString& InError) const {

	
	if (FVeUIModule* UIModule = FVeUIModule::Get()) {
		if (const TSharedPtr<FUICharacterCustomizationSubsystem> CharacterCustomizationSubsystem = UIModule->GetCharacterCustomizationSubsystem()) {
			CharacterCustomizationSubsystem->NotifyPersonaCreated(InMetadata);
		} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(CharacterCustomization)); }
	} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); }

	
	FUINotificationData NotificationData;
	NotificationData.Lifetime = 3.f;
	if (bInSuccessful) {
		NotificationData.Header = LOCTEXT("AvatarUploadSuccessNotificationHeader", "Success");
		NotificationData.Type = EUINotificationType::Success;
		NotificationData.Message = LOCTEXT("AvatarUploadSuccessNotificationMessage", "Avatar file uploaded successfully");
	} else {
		NotificationData.Header = LOCTEXT("AvatarUploadErrorNotificationHeader", "Error");
		NotificationData.Type = EUINotificationType::Failure;
		NotificationData.Message = LOCTEXT("AvatarUploadErrorNotificationMessage", "Failed to Avatar file.");
		LogWarningF("Failed to create a space: %s", *InError);
	}

	AddNotification(NotificationData);
}

void UUIRPMAvatarLinkWidget::SuccessCreatePersona(const FApiPersonaMetadata& InMetadata, const bool bInSuccessful, const FString& InError) {
	if (bInSuccessful) {
		UpdateFilePersona(InMetadata);
		//SetDefaultPersona(InMetadata);
	}

	if (FVeUIModule* UIModule = FVeUIModule::Get()) {
		if (const TSharedPtr<FUICharacterCustomizationSubsystem> CharacterCustomizationSubsystem = UIModule->GetCharacterCustomizationSubsystem()) {
			CharacterCustomizationSubsystem->NotifyCreatePersonaRequest(InMetadata);
		} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(CharacterCustomization)); }
	} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); }
	
	FUINotificationData NotificationData;
	NotificationData.Lifetime = 3.f;
	if (bInSuccessful) {
		NotificationData.Header = LOCTEXT("PersonaSuccessNotificationHeader", "Success");
		NotificationData.Type = EUINotificationType::Success;
		NotificationData.Message = LOCTEXT("PersonaSuccessNotificationMessage", "The persona is successfully created.");
	} else {
		NotificationData.Header = LOCTEXT("PersonaErrorNotificationHeader", "Error");
		NotificationData.Type = EUINotificationType::Failure;
		NotificationData.Message = LOCTEXT("PersonaErrorNotificationMessage", "Failed to create Persona.");
		LogWarningF("Failed to create a space: %s", *InError);
	}

	AddNotification(NotificationData);

	if (UrlWidget) {
		UrlWidget->SetText(FText());
		PersonaNameWidget->SetText(FText());
		CreateButtonWidget->SetIsEnabled(false);
	}

}


#undef LOCTEXT_NAMESPACE
