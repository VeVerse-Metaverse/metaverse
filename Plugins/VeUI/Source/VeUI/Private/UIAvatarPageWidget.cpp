// Authors: Khusan T.Yadgarov, Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UIAvatarPageWidget.h"

#include "VeApi.h"
#include "VeApi2.h"
#include "Api2AuthSubsystem.h"
#include "Api2FileSubsystem.h"
#include "VeShared.h"
#include "VeUI.h"
#include "Components/UIButtonWidget.h"
#include "Components/UICommonPaginationWidget.h"
#include "UIAvatarFormPageWidget.h"
#include "Subsystems/UICharacterCustomizationSubsystem.h"
#include "Subsystems/UINotificationSubsystem.h"
#include "UIPageManagerWidget.h"
#include "VeImageFunctionLibrary.h"
#include "Components/TileView.h"
#include "Components/UILoadingWidget.h"

#define LOCTEXT_NAMESPACE "VeUI"


void UUIAvatarPageWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();

	if (PaginationWidget) {
		PaginationWidget->ItemsPerPage = ItemsPerPage;
	}

	RequestMetadata.Limit = ItemsPerPage;
	RequestMetadata.Offset = 0;
	RequestMetadata.Query = Api::Fields::RPM;

	SetIsProcessing(false);
}

void UUIAvatarPageWidget::RegisterCallbacks() {
	Super::RegisterCallbacks();

	if (!OnCreateButtonClickedDelegateHandle.IsValid()) {
		if (IsValid(CreateButtonWidget)) {
			OnCreateButtonClickedDelegateHandle = CreateButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [=] {
				if (IsValid(FormPageClass)) {
					GetPageManager()->OpenPage(FormPageClass);
				}
			});
		}
	}

	if (!OnObjectListItemClickedDelegateHandle.IsValid()) {
		if (IsValid(ContainerWidget)) {
			ContainerWidget->OnItemClicked().AddWeakLambda(this, [this](UObject* InObject) {
				if (UApiPersonaMetadataObject* Object = Cast<UApiPersonaMetadataObject>(InObject)) {
					OnPersonaListItemClicked(Object->Metadata->Id);
				} else {
					LogErrorF("Failed to load the Person");
				}
			});
		}
	}

	if (!OnPaginationPageChangeDelegateHandle.IsValid()) {
		if (IsValid(PaginationWidget)) {
			OnPaginationPageChangeDelegateHandle = PaginationWidget->OnPageChanged.AddWeakLambda(this, [this](const int32 InCurrentPage, const int32 InItemsPerPage) {
				RequestMetadata.Limit = InItemsPerPage;
				RequestMetadata.Offset = InCurrentPage * InItemsPerPage;
				Refresh();
			});
		}
	}

	GET_MODULE_SUBSYSTEM(CharacterCustomizationSubsystem, UI, CharacterCustomization);
	if (CharacterCustomizationSubsystem) {
		if (!OnNotifyPersonaCreatedDelegateHandle.IsValid()) {
			OnNotifyPersonaCreatedDelegateHandle = CharacterCustomizationSubsystem->GetNotifyPersonaCreated().AddWeakLambda(
				this, [=](const FApiFileMetadata& InMetadata) {
					Refresh();
					OnPersonaListItemClicked(InMetadata.EntityId);
				});
		}

		if (!OnNotifyPersonaLoadedDelegateHandle.IsValid()) {
			OnNotifyPersonaLoadedDelegateHandle = CharacterCustomizationSubsystem->GetNotifyPersonaLoaded().AddWeakLambda(this,
				[this, CharacterCustomizationSubsystem](const FGuid& PersonaId) {

					// Update PersonaFullPreview
					CharacterCustomizationSubsystem->UpdatePersonaFullPreview();

					// Check PersonaFacePreview and render if not exist
					if (!CheckAvatarImagePreview(PersonaId)) {
						CharacterCustomizationSubsystem->UpdatePersonaFacePreview();
						RenderAndSavePersonaPreview(PersonaId);
					}

					SetIsProcessing(false);
				});
		}

		if (!OnNotifyPersonaSelectedDelegateHandle.IsValid()) {
			OnNotifyPersonaSelectedDelegateHandle = CharacterCustomizationSubsystem->GetNotifyPersonaSelected().AddWeakLambda(this, [=](const FGuid& PersonaId) {
				SetIsProcessing(true);
			});
		}
	}
}

void UUIAvatarPageWidget::UnregisterCallbacks() {
	Super::UnregisterCallbacks();

	if (OnCreateButtonClickedDelegateHandle.IsValid()) {
		if (IsValid(CreateButtonWidget)) {
			CreateButtonWidget->GetOnButtonClicked().Remove(OnCreateButtonClickedDelegateHandle);
			OnCreateButtonClickedDelegateHandle.Reset();
		}
	}

	if (OnObjectListItemClickedDelegateHandle.IsValid()) {
		if (IsValid(ContainerWidget)) {
			ContainerWidget->OnItemClicked().Remove(OnObjectListItemClickedDelegateHandle);
		}
		OnObjectListItemClickedDelegateHandle.Reset();
	}

	if (OnPaginationPageChangeDelegateHandle.IsValid()) {
		if (IsValid(PaginationWidget)) {
			PaginationWidget->OnPageChanged.Remove(OnPaginationPageChangeDelegateHandle);
		}
		OnPaginationPageChangeDelegateHandle.Reset();
	}

	GET_MODULE_SUBSYSTEM(CharacterCustomizationSubsystem, UI, CharacterCustomization);
	if (CharacterCustomizationSubsystem) {
		if (OnNotifyPersonaCreatedDelegateHandle.IsValid()) {
			CharacterCustomizationSubsystem->GetNotifyPersonaCreated().Remove(OnNotifyPersonaCreatedDelegateHandle);
			OnNotifyPersonaCreatedDelegateHandle.Reset();
		}

		if (OnNotifyPersonaLoadedDelegateHandle.IsValid()) {
			CharacterCustomizationSubsystem->GetNotifyPersonaLoaded().Remove(OnNotifyPersonaLoadedDelegateHandle);
			OnNotifyPersonaLoadedDelegateHandle.Reset();
		}

		if (OnNotifyPersonaSelectedDelegateHandle.IsValid()) {
			CharacterCustomizationSubsystem->GetNotifyAvatarListItemClickedCustomizer().Remove(OnNotifyPersonaSelectedDelegateHandle);
			OnNotifyPersonaSelectedDelegateHandle.Reset();
		}
	}
}

void UUIAvatarPageWidget::NativeOnRefresh() {
	Super::NativeOnRefresh();

	GET_MODULE_SUBSYSTEM(AuthSubsystem, Api2, Auth);
	if (AuthSubsystem) {
		CurrentUserId = AuthSubsystem->GetUserId();
	} else {
		CurrentUserId = FGuid();
	}

	GetRPMPersonasRequest(RequestMetadata);

	GET_MODULE_SUBSYSTEM(CharacterCustomizationSubsystem, UI, CharacterCustomization);
	if (CharacterCustomizationSubsystem) {
		CharacterCustomizationSubsystem->UpdatePersonaFullPreview();
	}
}

void UUIAvatarPageWidget::GetRPMPersonasRequest(const IApiBatchQueryRequestMetadata& InRequestMetadata) {
	GET_MODULE_SUBSYSTEM(PersonaSubsystem, Api, Persona);
	if (PersonaSubsystem) {
		const TSharedRef<FOnPersonaBatchRequestCompleted> CallbackPtr = MakeShareable(new FOnPersonaBatchRequestCompleted());

		CallbackPtr->BindWeakLambda(this, [this](const FApiPersonaBatchMetadata& InMetadata, const bool bInSuccessful, const FString& InError) {
			if (!bInSuccessful) {
				LogErrorF("failed to request: %s", *InError);
			}

			// SetIsProcessing(false);
			OnGetRPMPersonasResponse(InMetadata, bInSuccessful, InError);
		});

		if (PersonaSubsystem->GetPersonas(CurrentUserId, InRequestMetadata, CallbackPtr)) {
			// SetIsProcessing(true);
		} else {
			LogErrorF("failed to request");
		}
	}
}

void UUIAvatarPageWidget::OnGetRPMPersonasResponse(const FApiPersonaBatchMetadata& InMetadata, const bool bSuccessful, const FString& Error) {
	if (bSuccessful) {
		if (IsValid(PaginationWidget)) {
			PaginationWidget->SetPaginationOptions(InMetadata.Total, ItemsPerPage);
		}
		if (IsValid(ContainerWidget)) {
			ContainerWidget->ScrollToTop();
		}
		SetRPMPersonas(InMetadata);
	} else {
		GET_MODULE_SUBSYSTEM(NotificationSubsystem, UI, Notification);
		if (NotificationSubsystem) {
			FUINotificationData NotificationData;
			NotificationData.Type = EUINotificationType::Failure;
			NotificationData.Header = LOCTEXT("LoadObjectBatchError", "Error");
			NotificationData.Message = FText::FromString("Failed to load a object batch.");
			NotificationSubsystem->AddNotification(NotificationData);
		}
	}
}

void UUIAvatarPageWidget::SetRPMPersonas(const FApiPersonaBatchMetadata& BatchMetadata) {
	if (IsValid(ContainerWidget)) {
		ContainerWidget->ClearListItems();
		ContainerWidget->SetListItems(UApiPersonaMetadataObject::BatchToUObjectArray(BatchMetadata.Entities, this));
	}

	if (IsValid(PaginationWidget)) {
		PaginationWidget->SetPaginationOptions(BatchMetadata.Total, ItemsPerPage);
	}
}

void UUIAvatarPageWidget::RenderAndSavePersonaPreview(const FGuid& PersonaId) {
	if (!PersonaId.IsValid()) {
		return;
	}

	if (PersonaPreviewRenderTarget) {
		TArray<uint8> OutJpegBytes;
		if (UVeImageFunctionLibrary::ExportRenderTarget2DAsJPEGRaw(PersonaPreviewRenderTarget, OutJpegBytes)) {
			GET_MODULE_SUBSYSTEM(FileUploadSubsystem, Api2, File);
			if (FileUploadSubsystem) {
				FApi2FileUploadMemoryRequestMetadata UploadMetadata;
				UploadMetadata.Id = PersonaId;
				UploadMetadata.FileBytes = OutJpegBytes;
				UploadMetadata.FileName = FString::Printf(TEXT("persona_preview_%s"), *PersonaId.ToString(EGuidFormats::Digits));
				UploadMetadata.FileType = Api::FileTypes::ImagePreview;
				UploadMetadata.MimeType = Api::ContentTypes::ImageJpeg;

				const auto InCallback = MakeShared<FOnFileRequestCompleted>();
				InCallback->BindWeakLambda(
					this, [this, PersonaId, InCallback](const FApiFileMetadata& InMetadata, const bool bInSuccessful, const FString& InError) {
						if (!bInSuccessful) {
							LogErrorF("failed to upload persona preview: %s", *InError);
							return;
						}

						OnNotifyPersonaImageUpdated(PersonaId, bInSuccessful, InError);
					});

				FileUploadSubsystem->UploadMemory(UploadMetadata, InCallback);
			}
		}
	}
}

void UUIAvatarPageWidget::SetIsProcessing(bool bInIsLoading) const {
	if (IsValid(LoadingWidget)) {
		if (bInIsLoading) {
			LoadingWidget->Show();
		} else {
			LoadingWidget->Hide();
		}
	}
}

bool UUIAvatarPageWidget::CheckAvatarImagePreview(const FGuid& PersonaId) {
	if (!IsValid(ContainerWidget)) {
		return false;
	}

	for (UObject* Object : ContainerWidget->GetListItems()) {
		UApiPersonaMetadataObject* PersonaMetadataObject = Cast<UApiPersonaMetadataObject>(Object);
		if (PersonaMetadataObject) {
			if (PersonaMetadataObject->Metadata->Id == PersonaId) {
				const auto PreviewImage = PersonaMetadataObject->Metadata->Files.FindByPredicate([=](const FApiFileMetadata& File) {
					return File.Type == EApiFileType::ImagePreview;
				});

				return PreviewImage != nullptr;
			}
		}
	}

	return false;
}

void UUIAvatarPageWidget::OnPersonaListItemClicked(const FGuid& PersonaId) {
	if (SelectedPersonaId == PersonaId) {
		return;
	}

	SelectedPersonaId = PersonaId;

	// Set default persona
	GET_MODULE_SUBSYSTEM(PersonaSubsystem, Api, Persona);
	if (PersonaSubsystem) {
		TSharedPtr<FOnPersonaRequestCompleted> CallbackPtr = MakeShareable(new FOnPersonaRequestCompleted());

		CallbackPtr->BindWeakLambda(
			this, [this, PersonaId, CallbackPtr](const FApiPersonaMetadata& /*InMetadata*/, const bool bInSuccessful, const FString& InError) mutable {
				if (!bInSuccessful) {
					LogErrorF("failed to request: %s", *InError);
				}

				if (bInSuccessful) {
					GET_MODULE_SUBSYSTEM(CharacterCustomizationSubsystem, UI, CharacterCustomization);
					if (CharacterCustomizationSubsystem) {
						CharacterCustomizationSubsystem->NotifyCurrentPersonaIdChanged(PersonaId);
					}
				}

				CallbackPtr.Reset();
			});

		if (PersonaSubsystem->PostDefaultPersonas(PersonaId, CallbackPtr)) {
			LogF("request successful");
		}
	}

	// Set character persona id (AVeMainCharacter::Owner_SetCurrentPersonaId)
	GET_MODULE_SUBSYSTEM(CharacterCustomizationSubsystem, UI, CharacterCustomization);
	if (CharacterCustomizationSubsystem) {
		CharacterCustomizationSubsystem->NotifyPersonaSelected(PersonaId);
	}
}

void UUIAvatarPageWidget::OnNotifyPersonaImageUpdated(const FGuid& PersonaId, const bool bInSuccessful, const FString& InError) {
	if (bInSuccessful) {
		Refresh();
	}

	GET_MODULE_SUBSYSTEM(NotificationSubsystem, UI, Notification);
	if (NotificationSubsystem) {
		FUINotificationData NotificationData;
		if (bInSuccessful) {
			NotificationData.Type = EUINotificationType::Success;
			NotificationData.Header = LOCTEXT("PersonaImageUpdatedSuccessNotificationHeader", "Success");
			NotificationData.Message = LOCTEXT("PersonaImageUpdatedSuccessNotificationMessage", "Persona Image successfully uploaded");
		} else {
			NotificationData.Type = EUINotificationType::Failure;
			NotificationData.Header = LOCTEXT("PersonaImageUpdatedErrorNotificationHeader", "Error");
			NotificationData.Message = LOCTEXT("PersonaImageUpdatedErrorNotificationMessage", "Persona Image was not loaded");
			LogWarningF("Failed to create a space: %s", *InError);
		}
		// NotificationSubsystem->AddNotification(NotificationData);
	}
}
#undef LOCTEXT_NAMESPACE
