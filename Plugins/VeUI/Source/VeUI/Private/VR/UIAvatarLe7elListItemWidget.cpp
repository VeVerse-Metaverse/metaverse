// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.


#include "VR/UIAvatarLe7elListItemWidget.h"
#include "ApiObjectMetadataObject.h"
#include "ApiPersonaMetadataObject.h"
#include "VeApi.h"
#include "Subsystems/UICharacterCustomizationSubsystem.h"
#include "VeShared.h"
#include "VeUI.h"
#include "AnimatedTexture2D.h"


void UUIAvatarLe7elListItemWidget::NativeConstruct() {
	SetIsLoading(false);

	if (FVeUIModule* UIModule = FVeUIModule::Get()) {
		if (const TSharedPtr<FUICharacterCustomizationSubsystem> CharacterCustomizationSubsystem = UIModule->GetCharacterCustomizationSubsystem()) {
			CurrentPersonaId = CharacterCustomizationSubsystem->GetCurrentPersonaId();
		}
	}

	Super::NativeConstruct();
}


void UUIAvatarLe7elListItemWidget::RegisterCallbacks() {

	if (FVeUIModule* UIModule = FVeUIModule::Get()) {
		if (const TSharedPtr<FUICharacterCustomizationSubsystem> CharacterCustomizationSubsystem = UIModule->GetCharacterCustomizationSubsystem()) {
			if (!OnGetCurrenPersonaMetadataDelegateHandle.IsValid()) {
				OnGetCurrenPersonaMetadataDelegateHandle = CharacterCustomizationSubsystem->GetNotifyCurrentPersonaIdChanged().AddWeakLambda(this, [=](const FGuid PersonaId) {
					SetCurrentPersonaId(PersonaId);
				});
			}
		}
	}

	Super::RegisterCallbacks();
}

void UUIAvatarLe7elListItemWidget::UnregisterCallbacks() {
	if (!OnGetCurrenPersonaMetadataDelegateHandle.IsValid()) {
		if (FVeUIModule* UIModule = FVeUIModule::Get()) {
			if (const TSharedPtr<FUICharacterCustomizationSubsystem> CharacterCustomizationSubsystem = UIModule->GetCharacterCustomizationSubsystem()) {
				CharacterCustomizationSubsystem->GetNotifyCurrentPersonaIdChanged().Remove(OnGetCurrenPersonaMetadataDelegateHandle);
				OnGetCurrenPersonaMetadataDelegateHandle.Reset();
			} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(UI), STRINGIFY(CharacterCustomization)); }
		} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(UI)); }
	}

	Super::UnregisterCallbacks();
}

void UUIAvatarLe7elListItemWidget::SetCurrentPersonaId(const FGuid PersonaId) {
	if (PersonaId.IsValid()) {
		CurrentPersonaId = PersonaId;
	}
}

void UUIAvatarLe7elListItemWidget::NativeOnEntryReleased() {
	PreviewImageWidget->SetBrushFromTexture(DefaultTexture);
	// ReleaseAllTextures();

	IUserObjectListEntry::NativeOnEntryReleased();
}

void UUIAvatarLe7elListItemWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) {
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	if (bIsActive) {
		if (ActiveHoverInAnimation) {
			PlayAnimationForward(ActiveHoverInAnimation);
		}
	} else if (HoverInAnimation) {
		if (HoverInAnimation) {
			PlayAnimationForward(HoverInAnimation);
		}
	}
}

void UUIAvatarLe7elListItemWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent) {
	Super::NativeOnMouseLeave(InMouseEvent);
	if (bIsActive) {
		if (ActiveHoverOutAnimation) {
			PlayAnimationForward(ActiveHoverOutAnimation);
		}
	} else if (HoverOutAnimation) {
		if (HoverOutAnimation) {
			PlayAnimationForward(HoverOutAnimation);
		}
	}
}

void UUIAvatarLe7elListItemWidget::NativeOnListItemObjectSet(UObject* ListItemObject) {
	PreviewImageWidget->SetBrushFromTexture(DefaultTexture);

	if (const UApiPersonaMetadataObject* PersonaObject = Cast<UApiPersonaMetadataObject>(ListItemObject)) {
		Persona = PersonaObject->Metadata;

		if (FVeApiModule* ApiModule = FVeApiModule::Get()) {
			if (const TSharedPtr<FApiAuthSubsystem> AuthSubsystem = ApiModule->GetAuthSubsystem()) {
				if (/**AuthSubsystem->GetUserMetadata().DefaultPersonaMetadata.Id == Persona->Id ||*/ CurrentPersonaId == Persona->Id) {
					bIsActive = true;
					if (PreviewImageBorderWidget) {
						PreviewImageBorderWidget->SetBrushColor(FLinearColor(1.0f, 0.0f, 1.0f));
					}
				} else {
					bIsActive = false;
					if (PreviewImageBorderWidget) {
						PreviewImageBorderWidget->SetBrushColor(FLinearColor(0.5f, 0.5f, 0.5f));
					}
				}
			}
		}

		if (NameTextWidget) {
			if (Persona->Name.IsEmpty()) {
				NameTextWidget->SetText(NSLOCTEXT("VeUI", "PersonaNameUnknown", "Unnamed"));
			} else {
				NameTextWidget->SetText(FText::FromString(TruncateString(Persona->Name)));
			}
		}
	}

	const auto PreviewImage = Persona->Files.FindByPredicate([=](const FApiFileMetadata& File) {
		return File.Type == EApiFileType::ImagePreview;
	});

	const FString DefaultAvatarUrl = Persona->GetDefaultAvatarUrl();
	if (PreviewImage && !PreviewImage->Url.IsEmpty()) {
		SetIsLoading(true);
		RequestTexture(this, PreviewImage->Url, TEXTUREGROUP_UI);
	} else if (!DefaultAvatarUrl.IsEmpty()) {
		SetIsLoading(true);
		RequestTexture(this, DefaultAvatarUrl, TEXTUREGROUP_UI);
	}
}

void UUIAvatarLe7elListItemWidget::NativeOnTextureLoaded(const FTextureLoadResult& InResult) {
	if (!InResult.bSuccessful || !InResult.Texture.IsValid()) {
		SetIsLoading(false);
		if (DefaultTexture) {
			PreviewImageWidget->SetBrushFromTexture(DefaultTexture);
		}
		return;
	}

	ASSIGN_WIDGET_TEXTURE(PreviewImageWidget, InResult);

	// todo: consider this as a hack, we need to figure out why this is called before NativeOnListItemObjectSet if the texture is cached.
	RUN_DELAYED(0.1f, this, [this](){
		SetIsLoading(false);
		});
}

void UUIAvatarLe7elListItemWidget::SetIsLoading(const bool bInIsLoading) const {
	if (LoadingWidget) {
		if (bInIsLoading) {
			LoadingWidget->Show();
		} else {
			LoadingWidget->Hide();
		}
	}
}
