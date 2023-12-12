// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "VR/UIAvatarCreateLe7elWidget.h"

#include "UINotificationData.h"
#include "VeApi.h"
#include "Subsystems/UICharacterCustomizationSubsystem.h"
#include "VeShared.h"
#include "VeUI.h"

#define LOCTEXT_NAMESPACE "VeUI"

void UUIAvatarCreateLe7elWidget::RegisterCallbacks() {
	if (IsValid(CloseButtonWidget)) {
		if (!OnCloseButtonWidgetDelegateHandle.IsValid()) {
			OnCloseButtonWidgetDelegateHandle = CloseButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
				if (OnClosedDelegate.IsBound()) {
					OnClosedDelegate.Broadcast();
				}
			});
		}
	}

	if (CreateButtonWidget) {
		if (!OnCreateButtonClickedDelegateHandle.IsValid()) {
			OnCreateButtonClickedDelegateHandle = CreateButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIAvatarCreateLe7elWidget::CreatePersona);
		}
	}
}


void UUIAvatarCreateLe7elWidget::UnregisterCallbacks() {
	if (OnCloseButtonWidgetDelegateHandle.IsValid()) {
		if (CloseButtonWidget) {
			CloseButtonWidget->GetOnButtonClicked().Remove(OnCloseButtonWidgetDelegateHandle);
			OnCloseButtonWidgetDelegateHandle.Reset();
		}
	}

	if (OnCreateButtonClickedDelegateHandle.IsValid()) {
		if (CreateButtonWidget) {
			CreateButtonWidget->GetOnButtonClicked().Remove(OnCreateButtonClickedDelegateHandle);
			OnCreateButtonClickedDelegateHandle.Reset();
		}
	}
}

void UUIAvatarCreateLe7elWidget::NativeConstruct() {
	Super::NativeConstruct();
}

void UUIAvatarCreateLe7elWidget::CreatePersona() {
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
					RequestMetadata.Type = Api::Fields::CharacterCustomizer;
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

void UUIAvatarCreateLe7elWidget::UpdateFilePersona(const FApiPersonaMetadata& InMetadata) {
	//
	// if (FVeApiModule* ApiModule = FVeApiModule::Get()) {
	// 	if (const TSharedPtr<FApiFileUploadSubsystem> FileUploadSubsystem = ApiModule->GetFileUploadSubsystem()) {
	// 		FApiEntityFileLinkMetadata EntityFileLinkMetadata;
	// 		EntityFileLinkMetadata.Id = InMetadata.Id;
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
	// 			SuccessUpdateAvatarFiles(bInSuccessful, InError);
	// 			CallbackPtr.Reset();
	// 		});
	//
	// 		if (FileUploadSubsystem->Replace(EntityFileLinkMetadata, CallbackPtr)) {
	// 			LogF("request successful");
	// 		}
	// 	} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Api), STRINGIFY(FileUpload)); }
	// } else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Api)); }
}
//
// void UUIAvatarCreateLe7elWidget::SetDefaultPersona(const FApiPersonaMetadata& InMetadata) {
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

void UUIAvatarCreateLe7elWidget::UpdateAvatarFiles() {

	if (OnSubmitDelegate.IsBound()) {
		OnSubmitDelegate.Broadcast();
	}

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
	// 			SuccessUpdateAvatarFiles(bInSuccessful, InError);
	// 			CallbackPtr.Reset();
	// 		});
	//
	// 		if (FileUploadSubsystem->Replace(EntityFileLinkMetadata, CallbackPtr)) {
	// 			LogF("request successful");
	// 		}
	// 	} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Api), STRINGIFY(FileUpload)); }
	// } else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Api)); }
}

void UUIAvatarCreateLe7elWidget::SetMetadata(const FApiUserMetadata& InMetadata) {
	Metadata = InMetadata;
}

void UUIAvatarCreateLe7elWidget::SuccessUpdateAvatarFiles(const bool bInSuccessful, const FString& InError) const {

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
//
// void UUIAvatarCreateLe7elWidget::SuccessPostDefaultAvatarFiles(const bool bInSuccessful, const FString& InError) const {
//
// 	FUINotificationData NotificationData;
// 	NotificationData.Lifetime = 3.f;
// 	if (bInSuccessful) {
// 		NotificationData.Header = LOCTEXT("AvatarDefaultSuccessNotificationHeader", "Success");
// 		NotificationData.Type = EUINotificationType::Success;
// 		NotificationData.Message = LOCTEXT("AvatarDefaultSuccessNotificationMessage", "Default avatar preset is successful.");
// 	} else {
// 		NotificationData.Header = LOCTEXT("AvatarDefaultErrorNotificationHeader", "Error");
// 		NotificationData.Type = EUINotificationType::Failure;
// 		NotificationData.Message = LOCTEXT("AvatarDefaultErrorNotificationMessage", "Failed to uDefault avatar preset.");
// 		LogWarningF("Failed to create a space: %s", *InError);
// 	}
//
// 	AddNotification(NotificationData);
// }

void UUIAvatarCreateLe7elWidget::SuccessCreatePersona(const FApiPersonaMetadata& InMetadata, const bool bInSuccessful, const FString& InError) {
	if (bInSuccessful) {
		UpdateFilePersona(InMetadata);
		//SetDefaultPersona(InMetadata);
	}

	if (FVeUIModule* UIModule = FVeUIModule::Get()) {
		if (const TSharedPtr<FUICharacterCustomizationSubsystem> CharacterCustomizationSubsystem = UIModule->GetCharacterCustomizationSubsystem()) {
			CharacterCustomizationSubsystem->NotifyCreateLE7ELPersonaRequest(InMetadata);
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
}


#undef LOCTEXT_NAMESPACE
