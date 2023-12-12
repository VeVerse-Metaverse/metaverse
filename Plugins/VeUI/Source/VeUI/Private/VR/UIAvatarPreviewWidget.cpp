// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "VR/UIAvatarPreviewWidget.h"

#include "ApiPersonaMetadataObject.h"
#include "ApiPersonaSubsystem.h"
#include "UINotificationData.h"
#include "VeApi.h"
#include "VeApi2.h"
#include "Subsystems/UIBreadcrumbSubsystem.h"
#include "Subsystems/UICharacterCustomizationSubsystem.h"
#include "VeShared.h"
#include "VeUI.h"
#include "AnimatedTexture2D.h"

#define LOCTEXT_NAMESPACE "VeUI"

void UUIAvatarPreviewWidget::RegisterCallbacks() {

	if (!OnObjectListItemClickedDelegateHandle.IsValid()) {
		OnObjectListItemClickedDelegateHandle = OnPersonaListItemClicked.AddWeakLambda(this, [this](UApiPersonaMetadataObject* InObject) {
			if (InObject) {
				if (OnObjectSelected.IsBound()) {
					OnObjectSelected.Broadcast(InObject->Metadata);
				}
				if (FVeUIModule* UIModule = FVeUIModule::Get()) {
					if (const TSharedPtr<FUICharacterCustomizationSubsystem> CharacterCustomizationSubsystem = UIModule->GetCharacterCustomizationSubsystem()) {
						CharacterCustomizationSubsystem->NotifyAvatarListItemClickedCustomizer(InObject->Metadata);
					} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(CharacterCustomization)); }
				} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); }
			} else {
				FUINotificationData NotificationData;
				NotificationData.Header = NSLOCTEXT("VeUI", "OpenObjectError", "Error");
				NotificationData.Lifetime = 3.f;
				NotificationData.Type = EUINotificationType::Failure;
				NotificationData.Message = FText::FromString("Failed to load the Person.");
				AddNotification(NotificationData);
			}
		});
	}

	if (!OnLe7elObjectListItemClickedDelegateHandle.IsValid()) {
		OnLe7elObjectListItemClickedDelegateHandle = OnLe7elPersonaListItemClicked.AddWeakLambda(this, [this](UApiPersonaMetadataObject* InObject) {
			if (InObject) {
				if (OnLe7elObjectSelected.IsBound()) {
					OnLe7elObjectSelected.Broadcast(InObject->Metadata);
				}
				if (FVeUIModule* UIModule = FVeUIModule::Get()) {
					if (const TSharedPtr<FUICharacterCustomizationSubsystem> CharacterCustomizationSubsystem = UIModule->GetCharacterCustomizationSubsystem()) {
						CharacterCustomizationSubsystem->NotifyLE7ELAvatarListItemClickedCustomizer(InObject->Metadata);
					} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(CharacterCustomization)); }
				} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); }
			} else {
				FUINotificationData NotificationData;
				NotificationData.Header = NSLOCTEXT("VeUI", "OpenObjectError", "Error");
				NotificationData.Lifetime = 3.f;
				NotificationData.Type = EUINotificationType::Failure;
				NotificationData.Message = FText::FromString("Failed to load the Person.");
				AddNotification(NotificationData);
			}
		});
	}

	if (IsValid(EditButtonWidget)) {
		if (!OnEditButtonClickedDelegateHandle.IsValid()) {
			OnEditButtonClickedDelegateHandle = EditButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [=] {
				if (FVeUIModule* UIModule = FVeUIModule::Get()) {
					if (const TSharedPtr<FUICharacterCustomizationSubsystem> CharacterCustomizationSubsystem = UIModule->GetCharacterCustomizationSubsystem()) {
						CharacterCustomizationSubsystem->OpenCustomizer();
					} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(CharacterCustomization)); }
				} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); }
			});
		}
	}

	if (IsValid(CloseButtonWidget)) {
		if (!OnCloseButtonWidgetDelegateHandle.IsValid()) {
			OnCloseButtonWidgetDelegateHandle = CloseButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
				if (FVeUIModule* UIModule = FVeUIModule::Get()) {
					if (const TSharedPtr<FUIBreadcrumbSubsystem> BreadcrumbSubsystem = UIModule->GetBreadcrumbSubsystem()) {
						{
							BreadcrumbSubsystem->Pop();
						}
					} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(Breadcrumb)); }
				} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); };

				if (FVeUIModule* UIModule = FVeUIModule::Get()) {
					if (const TSharedPtr<FUICharacterCustomizationSubsystem> CharacterCustomizationSubsystem = UIModule->GetCharacterCustomizationSubsystem()) {
						CharacterCustomizationSubsystem->CloseCustomizerPreview();
					} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(CharacterCustomization)); }
				} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); }

				if (OnClosedDelegate.IsBound()) {
					OnClosedDelegate.Broadcast();
				}
			});
		}
	}


	if (!OnPaginationPageChangeDelegateHandle.IsValid()) {
		if (PaginationWidget) {
			OnPaginationPageChangeDelegateHandle = PaginationWidget->OnPageChanged.AddWeakLambda(this, [this](const int32 InCurrentPage, const int32 InItemsPerPage) {
				if (IsValid(PaginationWidget)) {
					IApiBatchQueryRequestMetadata RequestMetadata;
					RequestMetadata.Limit = ItemsPerPage;
					RequestMetadata.Offset = InCurrentPage * InItemsPerPage;
					RequestMetadata.Query = Api::Fields::RPM;
					GetRPMPersonas(RequestMetadata);
				}
			});
		}
	}

	if (!OnLe7elPaginationPageChangeDelegateHandle.IsValid()) {
		if (PaginationWidget) {
			OnLe7elPaginationPageChangeDelegateHandle = Le7elPaginationWidget->OnPageChanged.AddWeakLambda(this, [this](const int32 InCurrentPage, const int32 InItemsPerPage) {
				if (IsValid(Le7elPaginationWidget)) {
					IApiBatchQueryRequestMetadata RequestMetadata;
					RequestMetadata.Limit = ItemsPerPage;
					RequestMetadata.Offset = InCurrentPage * InItemsPerPage;
					RequestMetadata.Query = Api::Fields::CharacterCustomizer;
					GetLe7elPersonas(RequestMetadata);
				}
			});
		}
	}

	if (FVeUIModule* UIModule = FVeUIModule::Get()) {
		if (const TSharedPtr<FUICharacterCustomizationSubsystem> CharacterCustomizationSubsystem = UIModule->GetCharacterCustomizationSubsystem()) {
			if (!OnPersonaCreateBoundDelegateHandle.IsValid()) {
				OnPersonaCreateBoundDelegateHandle = CharacterCustomizationSubsystem->GetNotifyPersonaCreated().AddWeakLambda(
					this, [=](const FApiFileMetadata& InMetadata) {
						Refresh();
						SetIsProcessing(true);
					});
			}

			if (!OnLe7elPersonaCreateBoundDelegateHandle.IsValid()) {
				OnLe7elPersonaCreateBoundDelegateHandle = CharacterCustomizationSubsystem->GetNotifyLe7elCreatePersonaCustomizer().AddWeakLambda(
					this, [=](const FApiPersonaMetadata& InMetadata) {
						Refresh();
					});
			}

			if (!OnNotifyAvatarDelegateHandle.IsValid()) {
				OnNotifyAvatarDelegateHandle = CharacterCustomizationSubsystem->GetNotifyPersonaLoaded().AddWeakLambda(this, [=](const FGuid& PersonaId) {
					SetIsProcessing(false);
				});
			}

			if (!OnNotifyLe7elAvatarDelegateHandle.IsValid()) {
				OnNotifyLe7elAvatarDelegateHandle = CharacterCustomizationSubsystem->GetNotifyLe7elAvatarLoaded().AddWeakLambda(this, [=] {
					SetIsProcessing(false);
				});
			}

			if (!OnPersonaClickBoundDelegateHandle.IsValid()) {
				OnPersonaClickBoundDelegateHandle = CharacterCustomizationSubsystem->GetNotifyAvatarListItemClickedCustomizer().AddWeakLambda(
					this, [=](const TSharedPtr<FApiPersonaMetadata> InMetadata) {
						SetIsProcessing(true);
					});
			}

			if (!OnLe7elPersonaClickBoundDelegateHandle.IsValid()) {
				OnLe7elPersonaClickBoundDelegateHandle = CharacterCustomizationSubsystem->GetNotifyLe7elAvatarListItemClickedCustomizer().AddWeakLambda(
					this, [=](const TSharedPtr<FApiPersonaMetadata> InMetadata) {
						SetIsProcessing(true);
					});
			}

			// if (!OnPersonaImageUpdatedDelegateHandle.IsValid()) {
			// 	OnPersonaImageUpdatedDelegateHandle = CharacterCustomizationSubsystem->GetNotifyPersonaImageUpdated().AddWeakLambda(
			// 		this, [=](const FGuid& PersonaId, const bool bInSuccessful, const FString& InError) {
			// 			SuccessPersonaImageUpdated(PersonaId, bInSuccessful, InError);
			// 			SetIsProcessing(false);
			// 		});
			// }
		}
	}
}

void UUIAvatarPreviewWidget::UnregisterCallbacks() {
	if (OnObjectListItemClickedDelegateHandle.IsValid()) {
		OnPersonaListItemClicked.Remove(OnObjectListItemClickedDelegateHandle);
		OnObjectListItemClickedDelegateHandle.Reset();
	}

	if (OnLe7elObjectListItemClickedDelegateHandle.IsValid()) {
		OnLe7elPersonaListItemClicked.Remove(OnLe7elObjectListItemClickedDelegateHandle);
		OnLe7elObjectListItemClickedDelegateHandle.Reset();
	}

	if (OnCloseButtonWidgetDelegateHandle.IsValid()) {
		if (CloseButtonWidget) {
			CloseButtonWidget->GetOnButtonClicked().Remove(OnCloseButtonWidgetDelegateHandle);
			OnCloseButtonWidgetDelegateHandle.Reset();
		}
	}

	if (OnEditButtonClickedDelegateHandle.IsValid()) {
		if (IsValid(EditButtonWidget)) {
			EditButtonWidget->GetOnButtonClicked().Remove(OnEditButtonClickedDelegateHandle);
			OnEditButtonClickedDelegateHandle.Reset();
		}
	}

	if (OnPaginationPageChangeDelegateHandle.IsValid()) {
		if (PaginationWidget) {
			PaginationWidget->OnPageChanged.Remove(OnPaginationPageChangeDelegateHandle);
			OnPaginationPageChangeDelegateHandle.Reset();
		}
	}

	if (OnLe7elPaginationPageChangeDelegateHandle.IsValid()) {
		if (Le7elPaginationWidget) {
			Le7elPaginationWidget->OnPageChanged.Remove(OnLe7elPaginationPageChangeDelegateHandle);
			OnLe7elPaginationPageChangeDelegateHandle.Reset();
		}
	}

	if (!OnPersonaCreateBoundDelegateHandle.IsValid()) {
		if (FVeUIModule* UIModule = FVeUIModule::Get()) {
			if (const TSharedPtr<FUICharacterCustomizationSubsystem> CharacterCustomizationSubsystem = UIModule->GetCharacterCustomizationSubsystem()) {
				CharacterCustomizationSubsystem->GetNotifyPersonaCreated().Remove(OnPersonaCreateBoundDelegateHandle);
				OnPersonaCreateBoundDelegateHandle.Reset();
			} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(CharacterCustomization)); }
		} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); }
	}

	if (!OnLe7elPersonaCreateBoundDelegateHandle.IsValid()) {
		if (FVeUIModule* UIModule = FVeUIModule::Get()) {
			if (const TSharedPtr<FUICharacterCustomizationSubsystem> CharacterCustomizationSubsystem = UIModule->GetCharacterCustomizationSubsystem()) {
				CharacterCustomizationSubsystem->GetNotifyLe7elCreatePersonaCustomizer().Remove(OnLe7elPersonaCreateBoundDelegateHandle);
				OnLe7elPersonaCreateBoundDelegateHandle.Reset();
			} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(CharacterCustomization)); }
		} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); }
	}

	if (!OnNotifyAvatarDelegateHandle.IsValid()) {
		if (FVeUIModule* UIModule = FVeUIModule::Get()) {
			if (const TSharedPtr<FUICharacterCustomizationSubsystem> CharacterCustomizationSubsystem = UIModule->GetCharacterCustomizationSubsystem()) {
				CharacterCustomizationSubsystem->GetNotifyPersonaLoaded().Remove(OnNotifyAvatarDelegateHandle);
				OnNotifyAvatarDelegateHandle.Reset();
			} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(CharacterCustomization)); }
		} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); }
	}

	if (!OnNotifyLe7elAvatarDelegateHandle.IsValid()) {
		if (FVeUIModule* UIModule = FVeUIModule::Get()) {
			if (const TSharedPtr<FUICharacterCustomizationSubsystem> CharacterCustomizationSubsystem = UIModule->GetCharacterCustomizationSubsystem()) {
				CharacterCustomizationSubsystem->GetNotifyLe7elAvatarLoaded().Remove(OnNotifyLe7elAvatarDelegateHandle);
				OnNotifyLe7elAvatarDelegateHandle.Reset();
			} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(CharacterCustomization)); }
		} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); }
	}

	if (!OnPersonaClickBoundDelegateHandle.IsValid()) {
		if (FVeUIModule* UIModule = FVeUIModule::Get()) {
			if (const TSharedPtr<FUICharacterCustomizationSubsystem> CharacterCustomizationSubsystem = UIModule->GetCharacterCustomizationSubsystem()) {
				CharacterCustomizationSubsystem->GetNotifyAvatarListItemClickedCustomizer().Remove(OnPersonaClickBoundDelegateHandle);
				OnPersonaClickBoundDelegateHandle.Reset();
			} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(CharacterCustomization)); }
		} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); }
	}

	if (!OnLe7elPersonaClickBoundDelegateHandle.IsValid()) {
		if (FVeUIModule* UIModule = FVeUIModule::Get()) {
			if (const TSharedPtr<FUICharacterCustomizationSubsystem> CharacterCustomizationSubsystem = UIModule->GetCharacterCustomizationSubsystem()) {
				CharacterCustomizationSubsystem->GetNotifyLe7elAvatarListItemClickedCustomizer().Remove(OnLe7elPersonaClickBoundDelegateHandle);
				OnLe7elPersonaClickBoundDelegateHandle.Reset();
			} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(CharacterCustomization)); }
		} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); }
	}

	// if (!OnPersonaImageUpdatedDelegateHandle.IsValid()) {
	// 	if (FVeUIModule* UIModule = FVeUIModule::Get()) {
	// 		if (const TSharedPtr<FUICharacterCustomizationSubsystem> CharacterCustomizationSubsystem = UIModule->GetCharacterCustomizationSubsystem()) {
	// 			CharacterCustomizationSubsystem->GetNotifyCurrentPersonaIdChanged().Remove(OnPersonaImageUpdatedDelegateHandle);
	// 			OnPersonaImageUpdatedDelegateHandle.Reset();
	// 		} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(CharacterCustomization)); }
	// 	} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); }
	// }

}

#pragma region Navigation

// void UUIAvatarPreviewWidget::ShowRPMAvatarLinkWidget(bool bAnimate, bool bPlaySound) {
// 	if (!bRPMAvatarLinkWidgetVisible) {
// 		bRPMAvatarLinkWidgetVisible = true;
// 		if (RPMAvatarLinkWidget) {
// 			RPMAvatarLinkWidget->ResetState();
// 			RPMAvatarLinkWidget->ResetAnimationState();
// 			RPMAvatarLinkWidget->Show(bAnimate, bPlaySound);
// 		}
// 	}
// }
//
// void UUIAvatarPreviewWidget::HideRPMAvatarLinkWidget(bool bAnimate, bool bPlaySound) {
// 	if (bRPMAvatarLinkWidgetVisible) {
// 		bRPMAvatarLinkWidgetVisible = false;
// 		if (RPMAvatarLinkWidget) {
// 			RPMAvatarLinkWidget->Hide(bAnimate, bPlaySound);
// 		}
// 	}
// }
//
// void UUIAvatarPreviewWidget::NavigateToRPMAvatarLinkWidget(bool bAnimate, bool bPlaySound) {
// 	HideRPMAvatarLinkWidget();
// }

void UUIAvatarPreviewWidget::NativeConstruct() {
	//bRPMAvatarLinkWidgetVisible = true;

	if (IsValid(ContainerWidget)) {
		ContainerWidget->OnItemClicked().AddWeakLambda(this, [this](UObject* InObject) {
			if (OnPersonaListItemClicked.IsBound()) {
				if (UApiPersonaMetadataObject* Object = Cast<UApiPersonaMetadataObject>(InObject)) {
					OnPersonaListItemClicked.Broadcast(Object);
				}
			}
		});
	}

	if (IsValid(Le7elContainerWidget)) {
		Le7elContainerWidget->OnItemClicked().AddWeakLambda(this, [this](UObject* InObject) {
			if (OnLe7elPersonaListItemClicked.IsBound()) {
				if (UApiPersonaMetadataObject* Object = Cast<UApiPersonaMetadataObject>(InObject)) {
					OnLe7elPersonaListItemClicked.Broadcast(Object);
				}
			}
		});
	}

	SetMetadata();

	SetIsProcessing(false);

	//HideRPMAvatarLinkWidget(false, false);

	Super::NativeConstruct();
}
#pragma endregion

void UUIAvatarPreviewWidget::NativeOnTextureLoaded(const FTextureLoadResult& InResult) {
	if (!InResult.bSuccessful || !InResult.Texture.IsValid()) {
		//	SetIsLoading(false);
		return;
	}

	ASSIGN_WIDGET_TEXTURE(PreviewImageWidget, InResult);

	// // todo: consider this as a hack, we need to figure out why this is called before NativeOnListItemObjectSet if the texture is cached.
	// RUN_DELAYED(0.1f, this, [this](){
	// 			SetIsLoading(false);
	// 			});
}

void UUIAvatarPreviewWidget::SetMetadata() {

	// Deprecated
#if 0				
	if (const auto* ApiModule = FVeApi2Module::Get()) {
		const auto AuthSubsystem = ApiModule->GetAuthSubsystem();
		if (AuthSubsystem.IsValid()) {
			if (!AuthSubsystem->GetUserMetadata().Id.IsValid()) {
				AuthSubsystem->GetOnUserLogin().AddWeakLambda(this, [=](const FApiUserMetadata& InMetadata, const bool bSuccessful, const FString& Error) {
					UserMetadata = InMetadata;

					if (NameTextWidget) {
						if (UserMetadata.Name.IsEmpty()) {
							NameTextWidget->SetText(NSLOCTEXT("VeUI", "UserNameUnknown", "Unnamed"));
						} else {
							NameTextWidget->SetText(FText::FromString(UserMetadata.Name));
						}
					}
				});
			} else {
				UserMetadata = AuthSubsystem->GetUserMetadata();

				if (NameTextWidget) {
					if (UserMetadata.Name.IsEmpty()) {
						NameTextWidget->SetText(NSLOCTEXT("VeUI", "UserNameUnknown", "Unnamed"));
					} else {
						NameTextWidget->SetText(FText::FromString(UserMetadata.Name));
					}
				}
			}
		} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Api), STRINGIFY(Auth)); }
	} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Api)); };
#endif

	const auto PreviewImage = UserMetadata.Files.FindByPredicate([=](const FApiFileMetadata& File) {
		return File.Type == EApiFileType::ImageAvatar;
	});

	const FString DefaultAvatarUrl = UserMetadata.GetDefaultAvatarUrl();
	if (PreviewImage && !PreviewImage->Url.IsEmpty()) {
		RequestTexture(this, PreviewImage->Url, TEXTUREGROUP_UI);
	} else if (!DefaultAvatarUrl.IsEmpty()) {
		RequestTexture(this, DefaultAvatarUrl, TEXTUREGROUP_UI);
	}
}

void UUIAvatarPreviewWidget::SetIsProcessing(bool bInIsLoading) const {
	if (IsValid(LoadingWidget)) {
		if (bInIsLoading) {
			LoadingWidget->Show();
		} else {
			LoadingWidget->Hide();
		}
	}
}

void UUIAvatarPreviewWidget::Refresh() {
	if (IsValid(PaginationWidget)) {
		IApiBatchQueryRequestMetadata RequestMetadata;
		RequestMetadata.Limit = ItemsPerPage;
		RequestMetadata.Offset = PaginationWidget->CurrentPage * PaginationWidget->ItemsPerPage;
		RequestMetadata.Query = Api::Fields::RPM;
		GetRPMPersonas(RequestMetadata);
	}

	if (IsValid(Le7elPaginationWidget)) {
		IApiBatchQueryRequestMetadata RequestMetadata;
		RequestMetadata.Limit = ItemsPerPage;
		RequestMetadata.Offset = Le7elPaginationWidget->CurrentPage * Le7elPaginationWidget->ItemsPerPage;
		RequestMetadata.Query = Api::Fields::CharacterCustomizer;
		GetLe7elPersonas(RequestMetadata);
	}
}

void UUIAvatarPreviewWidget::GetRPMPersonas(const IApiBatchQueryRequestMetadata& RequestMetadata) {
	if (FVeApiModule* ApiModule = FVeApiModule::Get()) {
		if (const TSharedPtr<FApiPersonaSubsystem> PersonaSubsystem = ApiModule->GetPersonaSubsystem()) {
			if (const TSharedPtr<FApiAuthSubsystem> AuthSubsystem = ApiModule->GetAuthSubsystem()) {
				if (AuthSubsystem) {
					auto UserId = AuthSubsystem->GetUserId();

					const TSharedRef<FOnPersonaBatchRequestCompleted> CallbackPtr = MakeShareable(new FOnPersonaBatchRequestCompleted());

					CallbackPtr->BindWeakLambda(this, [this](const FApiPersonaBatchMetadata& InMetadata, const bool bInSuccessful, const FString& InError) {
						if (!bInSuccessful) {
							LogErrorF("failed to request: %s", *InError);
						}

						// Finish processing.
						//SetIsProcessing(false);

						OnRPMPersonasLoaded(InMetadata, bInSuccessful, InError);
					});

					if (!PersonaSubsystem->GetPersonas(UserId, RequestMetadata, CallbackPtr)) {
						LogErrorF("failed to request");
					}
				}
			}
		} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Api), STRINGIFY(Object)); }
	} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Api)); };
}

void UUIAvatarPreviewWidget::GetLe7elPersonas(const IApiBatchQueryRequestMetadata& RequestMetadata) {
	if (FVeApiModule* ApiModule = FVeApiModule::Get()) {
		if (const TSharedPtr<FApiPersonaSubsystem> PersonaSubsystem = ApiModule->GetPersonaSubsystem()) {
			if (const TSharedPtr<FApiAuthSubsystem> AuthSubsystem = ApiModule->GetAuthSubsystem()) {
				if (AuthSubsystem) {
					auto UserId = AuthSubsystem->GetUserId();
					const TSharedRef<FOnPersonaBatchRequestCompleted> CallbackPtr = MakeShareable(new FOnPersonaBatchRequestCompleted());

					CallbackPtr->BindWeakLambda(this, [this](const FApiPersonaBatchMetadata& InMetadata, const bool bInSuccessful, const FString& InError) {
						if (!bInSuccessful) {
							LogErrorF("failed to request: %s", *InError);
						}

						// Finish processing.
						//SetIsProcessing(false);

						OnLe7elPersonasLoaded(InMetadata, bInSuccessful, InError);
					});

					if (!PersonaSubsystem->GetPersonas(UserId, RequestMetadata, CallbackPtr)) {
						LogErrorF("failed to request");
					}
				}
			}
		} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Api), STRINGIFY(Object)); }
	} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Api)); };
}

void UUIAvatarPreviewWidget::SetRPMPersonas(const FApiPersonaBatchMetadata& BatchMetadata) {
	if (IsValid(ContainerWidget)) {
		ContainerWidget->ClearListItems();
		ContainerWidget->SetListItems(UApiPersonaMetadataObject::BatchToUObjectArray(BatchMetadata.Entities, this));
	}

	if (IsValid(PaginationWidget)) {
		PaginationWidget->SetPaginationOptions(BatchMetadata.Total, ItemsPerPage);
	}
}

void UUIAvatarPreviewWidget::SetLe7elPersonas(const FApiPersonaBatchMetadata& BatchMetadata) {
	if (IsValid(Le7elContainerWidget)) {
		Le7elContainerWidget->ClearListItems();
		Le7elContainerWidget->SetListItems(UApiPersonaMetadataObject::BatchToUObjectArray(BatchMetadata.Entities, this));
	}

	if (IsValid(Le7elPaginationWidget)) {
		Le7elPaginationWidget->SetPaginationOptions(BatchMetadata.Total, ItemsPerPage);
	}
}

void UUIAvatarPreviewWidget::OnRPMPersonasLoaded(const FApiPersonaBatchMetadata& InMetadata, const bool bSuccessful, const FString& Error) {
	if (bSuccessful) {
		if (IsValid(PaginationWidget)) { PaginationWidget->SetPaginationOptions(InMetadata.Total, ItemsPerPage); }
		if (IsValid(ContainerWidget)) { ContainerWidget->ScrollToTop(); }
		SetRPMPersonas(InMetadata);
	} else {
		FUINotificationData NotificationData;
		NotificationData.Header = LOCTEXT("LoadObjectBatchError", "Error");
		NotificationData.Lifetime = 3.f;
		NotificationData.Type = EUINotificationType::Failure;
		NotificationData.Message = FText::FromString("Failed to load a object batch.");
		AddNotification(NotificationData);
	}
}

void UUIAvatarPreviewWidget::OnLe7elPersonasLoaded(const FApiPersonaBatchMetadata& InMetadata, bool bSuccessful, const FString& Error) {
	if (bSuccessful) {
		if (IsValid(Le7elPaginationWidget)) { Le7elPaginationWidget->SetPaginationOptions(InMetadata.Total, ItemsPerPage); }
		if (IsValid(Le7elContainerWidget)) { Le7elContainerWidget->ScrollToTop(); }
		SetLe7elPersonas(InMetadata);
	} else {
		FUINotificationData NotificationData;
		NotificationData.Header = LOCTEXT("LoadObjectBatchError", "Error");
		NotificationData.Lifetime = 3.f;
		NotificationData.Type = EUINotificationType::Failure;
		NotificationData.Message = FText::FromString("Failed to load a object batch.");
		AddNotification(NotificationData);
	}
}

void UUIAvatarPreviewWidget::SuccessPersonaImageUpdated(const FGuid& PersonaId, const bool bInSuccessful, const FString& InError) {
	if (bInSuccessful) {
		Refresh();
	}

	FUINotificationData NotificationData;
	NotificationData.Lifetime = 3.f;
	if (bInSuccessful) {
		NotificationData.Header = LOCTEXT("PersonaImageUpdatedSuccessNotificationHeader", "Success");
		NotificationData.Type = EUINotificationType::Success;
		NotificationData.Message = LOCTEXT("PersonaImageUpdatedSuccessNotificationMessage", "Persona Image successfully uploaded");
	} else {
		NotificationData.Header = LOCTEXT("PersonaImageUpdatedErrorNotificationHeader", "Error");
		NotificationData.Type = EUINotificationType::Failure;
		NotificationData.Message = LOCTEXT("PersonaImageUpdatedErrorNotificationMessage", "Persona Image was not loaded");
		LogWarningF("Failed to create a space: %s", *InError);
	}
	//AddNotification(NotificationData);
}
#undef LOCTEXT_NAMESPACE
