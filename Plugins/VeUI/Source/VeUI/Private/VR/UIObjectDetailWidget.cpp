// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.


#include "VR/UIObjectDetailWidget.h"
#include "ApiObjectSubsystem.h"
#include "VeHUD.h"
#include "UINotificationData.h"
#include "VeApi.h"
#include "VeUI.h"
#include "Subsystems/UINotificationSubsystem.h"
#include "AnimatedTexture2D.h"


#define LOCTEXT_NAMESPACE "VeUI"


void UUIObjectDetailWidget::NativeConstruct() {
	bZoomImageWidgetVisible = true;

	HideZoomImageWidget(false, false);

	Super::NativeConstruct();
}

void UUIObjectDetailWidget::RegisterCallbacks() {
	if (CloseButtonWidget) {
		if (!OnCloseButtonWidgetDelegateHandle.IsValid()) {
			OnCloseButtonWidgetDelegateHandle = CloseButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
				if (OnClosed.IsBound()) {
					OnClosed.Broadcast();
				}
			});
		}
	}

	if (PreviewImageButtonWidget) {
		if (!OnPreviewButtonClickedDelegateHandle.IsValid()) {
			OnPreviewButtonClickedDelegateHandle = PreviewImageButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
				if (ZoomImageWidget) {
					if (Metadata) {
						FApiFileMetadata* PreviewImage = Metadata->Files.FindByPredicate([=](const FApiFileMetadata& File) {
							return File.Type == EApiFileType::ImageFull;
						});
						if (PreviewImage) {
							ZoomImageWidget->SetMetadata(*PreviewImage);
						}
					} else {
						LogErrorF(" Metadata is Nullprt");
					}
				}
				ShowZoomImageWidget();
			});
		}
	}

	if (ZoomImageWidget) {
		if (!OnZoomImageWidgetClosedDelegateHandle.IsValid())
			OnZoomImageWidgetClosedDelegateHandle = ZoomImageWidget->OnClosedDelegate.AddWeakLambda(this, [this]() {
				HideZoomImageWidget();
			});
	}

	if (!OnObjectEditButtonClickedDelegateHandle.IsValid()) {
		if (EditButtonWidget) {
			OnObjectEditButtonClickedDelegateHandle = EditButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIObjectDetailWidget::OnObjectFormEditButtonClicked);
		}
	}

	if (!OnObjectDeleteButtonClickedDelegateHandle.IsValid()) {
		if (DeleteButtonWidget) {
			OnObjectDeleteButtonClickedDelegateHandle = DeleteButtonWidget->GetOnButtonClicked().AddUObject(this, &UUIObjectDetailWidget::OnObjectDeleteButtonClicked);
		}
	}

	// if (!OnObjectDeletedDelegateHandle.IsValid()) {
	// 	if (FVeApiModule* ApiModule = FVeApiModule::Get()) {
	// 		if (const TSharedPtr<FApiObjectSubsystem> ObjectSubsystem = ApiModule->GetObjectSubsystem()) {
	// 			{
	// 				OnObjectDeletedDelegateHandle = ObjectSubsystem->GetOnDeleted().AddUObject(this, &UUIObjectDetailWidget::OnObjectDeleted);
	// 			}
	// 		} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Api), STRINGIFY(Object)); }
	// 	} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Api)); };
	// }
}

void UUIObjectDetailWidget::UnregisterCallbacks() {
	if (OnCloseButtonWidgetDelegateHandle.IsValid()) {
		if (CloseButtonWidget) {
			CloseButtonWidget->GetOnButtonClicked().Remove(OnCloseButtonWidgetDelegateHandle);
			OnCloseButtonWidgetDelegateHandle.Reset();
		}
	}

	if (OnObjectEditButtonClickedDelegateHandle.IsValid()) {
		if (EditButtonWidget) {
			EditButtonWidget->GetOnButtonClicked().Remove(OnObjectEditButtonClickedDelegateHandle);
			OnObjectEditButtonClickedDelegateHandle.Reset();
		}
	}

	if (OnObjectDeleteButtonClickedDelegateHandle.IsValid()) {
		if (DeleteButtonWidget) {
			DeleteButtonWidget->GetOnButtonClicked().Remove(OnObjectDeleteButtonClickedDelegateHandle);
			OnObjectDeleteButtonClickedDelegateHandle.Reset();
		}
	}

	if (OnPreviewButtonClickedDelegateHandle.IsValid()) {
		if (PreviewImageButtonWidget) {
			PreviewImageButtonWidget->GetOnButtonClicked().Remove(OnPreviewButtonClickedDelegateHandle);
			OnPreviewButtonClickedDelegateHandle.Reset();
		}
	}

	if (OnZoomImageWidgetClosedDelegateHandle.IsValid()) {
		if (ZoomImageWidget) {
			ZoomImageWidget->OnClosedDelegate.Remove(OnZoomImageWidgetClosedDelegateHandle);
			OnZoomImageWidgetClosedDelegateHandle.Reset();
		}
	}
}

void UUIObjectDetailWidget::OnObjectFormEditButtonClicked() const {
	if (Metadata) {
		if (OnEditButtonClicked.IsBound()) {
			OnEditButtonClicked.Broadcast(Metadata);
		}
	}
}

void UUIObjectDetailWidget::OnObjectDeleteButtonClicked() {
	if (OnDeleteButtonClicked.IsBound()) {
		OnDeleteButtonClicked.Broadcast();
	}

	RequestObjectDeleteConfirmation();
}

void UUIObjectDetailWidget::OnObjectDeleted(const bool bSuccessful, const FString& Error) const {
	GET_MODULE_SUBSYSTEM(NotificationSubsystem, UI, Notification);
	if (NotificationSubsystem) {
		FUINotificationData NotificationData;

		if (bSuccessful) {
			NotificationData.Type = EUINotificationType::Success;
			NotificationData.Header = LOCTEXT("DeleteObjectSuccessNotificationHeader", "Success");
			NotificationData.Message = LOCTEXT("DeleteObjectSuccessNotificationMessage", "Successfully deleted an object.");
		} else {
			NotificationData.Type = EUINotificationType::Failure;
			NotificationData.Header = LOCTEXT("DeleteObjectErrorNotificationHeader", "Error");
			NotificationData.Message = FText::Format(LOCTEXT("DeleteObjectErrorNotificationMessage", "Failed to delete an object: {0}"), FText::FromString(Error));
		}
		
		NotificationSubsystem->AddNotification(NotificationData);
	}
}

void UUIObjectDetailWidget::NativeOnTextureLoaded(const FTextureLoadResult& InResult) {
	// LogScreenLogF("Texture Loaded at ObjectDetailWidget %s", *InResult.Url);

	ASSIGN_WIDGET_TEXTURE(PreviewImageWidget, InResult);
}

void UUIObjectDetailWidget::SetMetadata(const TSharedPtr<FApiObjectMetadata> InMetadata) {
	Metadata = InMetadata;

	const auto PreviewImage = Metadata->Files.FindByPredicate([=](const FApiFileMetadata& File) {
		return File.Type == EApiFileType::ImagePreview;
	});

	if (PreviewImage) {
		if (!PreviewImage->Url.IsEmpty()) {
			RequestTexture(this, PreviewImage->Url, TEXTUREGROUP_UI);
		}
	}

	if (NameTextWidget) {
		if (Metadata->Name.IsEmpty()) {
			NameTextWidget->SetText(NSLOCTEXT("VeUI", "ObjectNameUnknown", ""));
		} else {
			NameTextWidget->SetText(FText::FromString(Metadata->Name));
		}
	}

	if (DescriptionTextWidget) {
		if (Metadata->Description.IsEmpty()) {
			DescriptionTextWidget->SetVisibility(ESlateVisibility::Collapsed);
		} else {
			DescriptionTextWidget->SetText(FText::FromString(Metadata->Description));
		}
	}

	if (ViewCountTextWidget) {
		ViewCountTextWidget->SetText(FText::Format(NSLOCTEXT("VeUI", "ObjectViewCountFormat", "{0}"), FText::AsNumber(Metadata->Views)));
	}

	if (TypeTextWidget) {
		if (Metadata->Type.IsEmpty()) {
			TypeTextWidget->SetText(NSLOCTEXT("VeUI", "ObjectTypeUnknown", ""));
		} else {
			TypeTextWidget->SetText(FText::FromString(Metadata->Type));
		}
	}

	if (ArtistTextWidget) {
		if (Metadata->Artist.IsEmpty()) {
			ArtistTextWidget->SetText(NSLOCTEXT("VeUI", "ObjectArtistUnknown", ""));
		} else {
			ArtistTextWidget->SetText(FText::FromString(Metadata->Artist));
		}
	}

	if (DateTextWidget) {
		if (Metadata->Date.IsEmpty()) {
			DateTextWidget->SetText(NSLOCTEXT("VeUI", "ObjectDateUnknown", ""));
		} else {
			DateTextWidget->SetText(FText::FromString(Metadata->Date));
		}
	}

	if (MediumTextWidget) {
		if (Metadata->Medium.IsEmpty()) {
			MediumTextWidget->SetText(NSLOCTEXT("VeUI", "ObjectMediumUnknown", ""));
		} else {
			MediumTextWidget->SetText(FText::FromString(Metadata->Medium));
		}
	}

	if (SourceTextWidget) {
		if (Metadata->Source.IsEmpty()) {
			SourceTextWidget->SetText(NSLOCTEXT("VeUI", "ObjectSourceUnknown", ""));
		} else {
			SourceTextWidget->SetText(FText::FromString(Metadata->Source));
		}
	}

	if (SourceUrlTextWidget) {
		if (Metadata->SourceUrl.IsEmpty()) {
			SourceUrlTextWidget->SetText(NSLOCTEXT("VeUI", "ObjectSourceUrlUnknown", ""));
		} else {
			SourceUrlTextWidget->SetText(FText::FromString(Metadata->SourceUrl));
		}
	}

	if (LicenseTextWidget) {
		if (Metadata->License.IsEmpty()) {
			LicenseTextWidget->SetText(NSLOCTEXT("VeUI", "ObjectLicenseUnknown", ""));
		} else {
			LicenseTextWidget->SetText(FText::FromString(Metadata->License));
		}
	}

	if (CopyrightTextWidget) {
		if (Metadata->Copyright.IsEmpty()) {
			CopyrightTextWidget->SetText(NSLOCTEXT("VeUI", "ObjectCopyrightUnknown", ""));
		} else {
			CopyrightTextWidget->SetText(FText::FromString(Metadata->Copyright));
		}
	}

	if (CreditTextWidget) {
		if (Metadata->Credit.IsEmpty()) {
			CreditTextWidget->SetText(NSLOCTEXT("VeUI", "ObjectCreditUnknown", ""));
		} else {
			CreditTextWidget->SetText(FText::FromString(Metadata->Credit));
		}
	}

	if (OriginTextWidget) {
		if (Metadata->Origin.IsEmpty()) {
			OriginTextWidget->SetText(NSLOCTEXT("VeUI", "ObjectOriginUnknown", ""));
		} else {
			OriginTextWidget->SetText(FText::FromString(Metadata->Origin));
		}
	}

	if (LocationTextWidget) {
		if (Metadata->Location.IsEmpty()) {
			LocationTextWidget->SetText(NSLOCTEXT("VeUI", "ObjectLocationUnknown", ""));
		} else {
			LocationTextWidget->SetText(FText::FromString(Metadata->Location));
		}
	}

	if (WidthTextWidget) {
		WidthTextWidget->SetText(FText::Format(NSLOCTEXT("VeUI", "ObjectWidthFormat", "{0}"), FText::AsNumber(Metadata->Width)));
	}

	if (HeightTextWidget) {
		HeightTextWidget->SetText(FText::Format(NSLOCTEXT("VeUI", "ObjectHeightFormat", "{0}"), FText::AsNumber(Metadata->Height)));
	}
}

void UUIObjectDetailWidget::RequestObjectDeleteConfirmation() {
	FUIDialogData DialogData;
	DialogData.Type = EUIDialogType::OkCancel;
	DialogData.HeaderText = LOCTEXT("ConfirmObjectDeleteDialogHeader", "Confirm delete");
	DialogData.MessageText = LOCTEXT("ConfirmObjectDeleteDialogMessage", "Do you really want to delete this object?");
	DialogData.FirstButtonText = LOCTEXT("ConfirmObjectDeleteDialogButtonOk", "OK");
	DialogData.SecondButtonText = LOCTEXT("ConfirmObjectDeleteDialogButtonCancel", "Cancel");
	DialogData.bCloseOnButtonClick = true;

	FUIDialogButtonClicked OnDialogButtonClicked;
	OnDialogButtonClicked.BindWeakLambda(this, [this](UUIDialogWidget*, const uint8& InButtonIndex) {
		if (InButtonIndex == 0) {
			DeleteObject();
		}
	});

	if (auto* HUD = GetHUD()) {
		HUD->ShowDialog(DialogData, OnDialogButtonClicked);
	}
}

void UUIObjectDetailWidget::DeleteObject() {
	if (FVeApiModule* ApiModule = FVeApiModule::Get()) {
		if (const TSharedPtr<FApiObjectSubsystem> ObjectSubsystem = ApiModule->GetObjectSubsystem()) {
			{
				const TSharedRef<FOnGenericRequestCompleted> CallbackPtr = MakeShareable(new FOnGenericRequestCompleted());

				CallbackPtr->BindWeakLambda(this, [=](const bool bInSuccessful, const FString& InError) {
					if (!bInSuccessful) {
						LogErrorF("failed to request: %s", *InError);
					}

					// Finish processing.
					// SetIsProcessing(false);

					OnObjectDeleted(bInSuccessful, InError);
				});
			
				if (!ObjectSubsystem->Delete(Metadata->Id, CallbackPtr)) {
					LogErrorF("failed to request");
					GET_MODULE_SUBSYSTEM(NotificationSubsystem, UI, Notification);
					if (NotificationSubsystem) {
						FUINotificationData NotificationData;
						NotificationData.Type = EUINotificationType::Failure;
						NotificationData.Header = LOCTEXT("Error", "Error");
						NotificationData.Message = LOCTEXT("FailedToDeleteAnObject", "Failed to delete an object.");
						NotificationSubsystem->AddNotification(NotificationData);
					}
				} else {
					// SetIsProcessing(true);
				}
				return;
			}
		} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Api), STRINGIFY(Object)); }
	} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Api)); };

	GET_MODULE_SUBSYSTEM(NotificationSubsystem, UI, Notification);
	if (NotificationSubsystem) {
		FUINotificationData NotificationData;
		NotificationData.Type = EUINotificationType::Failure;
		NotificationData.Header = LOCTEXT("UpdateObjectErrorNotificationHeader", "Error");
		NotificationData.Message = LOCTEXT("UpdateObjectErrorNotificationMessage", "Failed to delete an object.");
		NotificationSubsystem->AddNotification(NotificationData);
	}
}

void UUIObjectDetailWidget::HideZoomImageWidget(bool bAnimate, bool bPlaySound) {
	if (bZoomImageWidgetVisible) {
		bZoomImageWidgetVisible = false;
		if (ZoomImageWidget) {
			ZoomImageWidget->Hide();
		}
	}
}

void UUIObjectDetailWidget::ShowZoomImageWidget(bool bAnimate, bool bPlaySound) {
	
	if (!bZoomImageWidgetVisible) {
		bZoomImageWidgetVisible = true;
		if (ZoomImageWidget) {
			// ZoomImageWidget->ResetState();
			// ZoomImageWidget->ResetAnimationState();
			ZoomImageWidget->Show();
		}
	}
}

#undef LOCTEXT_NAMESPACE
