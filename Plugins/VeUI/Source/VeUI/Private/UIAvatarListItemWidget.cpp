// Authors: Khusan T.Yadgarov, Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UIAvatarListItemWidget.h"
#include "ApiObjectMetadataObject.h"
#include "ApiPersonaMetadataObject.h"
#include "VeApi.h"
#include "Subsystems/UICharacterCustomizationSubsystem.h"
#include "VeShared.h"
#include "VeUI.h"
#include "AnimatedTexture2D.h"
#include "Components/UIImageWidget.h"


void UUIAvatarListItemWidget::NativeConstruct() {
	SetIsLoading(false);

	if (FVeUIModule* UIModule = FVeUIModule::Get()) {
		if (const TSharedPtr<FUICharacterCustomizationSubsystem> CharacterCustomizationSubsystem = UIModule->GetCharacterCustomizationSubsystem()) {
			CurrentPersonaId = CharacterCustomizationSubsystem->GetCurrentPersonaId();
		}
	}

	Super::NativeConstruct();
}

void UUIAvatarListItemWidget::NativeOnEntryReleased() {
	if (PreviewImageWidget && !ImageUrl.IsEmpty()) {
		PreviewImageWidget->ShowImage(ImageUrl);
	}

	IUserObjectListEntry::NativeOnEntryReleased();
}

void UUIAvatarListItemWidget::RegisterCallbacks() {
	Super::RegisterCallbacks();

	GET_MODULE_SUBSYSTEM(CharacterCustomizationSubsystem, UI, CharacterCustomization);
	if (CharacterCustomizationSubsystem) {
		if (!OnGetCurrenPersonaMetadataDelegateHandle.IsValid()) {
			OnGetCurrenPersonaMetadataDelegateHandle = CharacterCustomizationSubsystem->GetNotifyCurrentPersonaIdChanged().AddWeakLambda(this, [=](const FGuid PersonaId) {
				SetCurrentPersonaId(PersonaId);
			});
		}

		// if (!OnPersonaImageUpdatedDelegateHandle.IsValid()) {
		// 	OnPersonaImageUpdatedDelegateHandle = CharacterCustomizationSubsystem->GetNotifyPersonaImageUpdated().AddWeakLambda(
		// 		this, [=](const FGuid& PersonaId, const bool bInSuccessful, const FString& InError) {
		// 			// SuccessPersonaImageUpdated(PersonaId, bInSuccessful, InError);
		// 			// SetIsProcessing(false);
		// 		});
		// }
	}
}

void UUIAvatarListItemWidget::UnregisterCallbacks() {
	Super::UnregisterCallbacks();

	GET_MODULE_SUBSYSTEM(CharacterCustomizationSubsystem, UI, CharacterCustomization);
	if (CharacterCustomizationSubsystem) {
		if (!OnGetCurrenPersonaMetadataDelegateHandle.IsValid()) {
			CharacterCustomizationSubsystem->GetNotifyCurrentPersonaIdChanged().Remove(OnGetCurrenPersonaMetadataDelegateHandle);
			OnGetCurrenPersonaMetadataDelegateHandle.Reset();
		}

		// if (!OnPersonaImageUpdatedDelegateHandle.IsValid()) {
		// 	CharacterCustomizationSubsystem->GetNotifyPersonaImageUpdated().Remove(OnPersonaImageUpdatedDelegateHandle);
		// 	OnPersonaImageUpdatedDelegateHandle.Reset();
		// }
	}
}

void UUIAvatarListItemWidget::SetCurrentPersonaId(const FGuid PersonaId) {
	if (PersonaId.IsValid()) {
		CurrentPersonaId = PersonaId;
		CheckSelected(CurrentPersonaId);
	}
}

void UUIAvatarListItemWidget::CheckSelected(const FGuid PersonaId) {
	if (PersonaId == Metadata->Id) {
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

void UUIAvatarListItemWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) {
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

void UUIAvatarListItemWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent) {
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

void UUIAvatarListItemWidget::NativeOnListItemObjectSet(UObject* ListItemObject) {
	
	if (const UApiPersonaMetadataObject* PersonaObject = Cast<UApiPersonaMetadataObject>(ListItemObject)) {
		Metadata = PersonaObject->Metadata;

		CheckSelected(CurrentPersonaId);

		if (NameTextWidget) {
			if (Metadata->Name.IsEmpty()) {
				NameTextWidget->SetText(NSLOCTEXT("VeUI", "PersonaNameUnknown", "Unnamed"));
			} else {
				NameTextWidget->SetText(FText::FromString(TruncateString(Metadata->Name)));
			}
		}
	}

	const auto PreviewImage = Metadata->Files.FindByPredicate([=](const FApiFileMetadata& File) {
		return File.Type == EApiFileType::ImagePreview;
	});

	if (PreviewImageWidget) {
		const TArray<EApiFileType> Types = {
			EApiFileType::ImagePreview,
			EApiFileType::ImageFull,
		};
	
		if (const auto* FileMetadata = FindFileMetadata(Metadata->Files, Types)) {
			PreviewImageWidget->ShowImage(FileMetadata->Url);
		}
	}

	const FString DefaultAvatarUrl = Metadata->GetDefaultAvatarUrl();
	if (PreviewImage && !PreviewImage->Url.IsEmpty()) {
		PreviewImageWidget->ShowImage(PreviewImage->Url);
		ImageUrl = PreviewImage->Url;
	} else if (!DefaultAvatarUrl.IsEmpty()) {
		PreviewImageWidget->ShowImage(DefaultAvatarUrl);
	} else {
		PreviewImageWidget->ShowDefaultImage();
	}
}

// void UUIAvatarListItemWidget::NativeOnTextureLoaded(const FTextureLoadResult& InResult) {
// 	if (!InResult.bSuccessful || !InResult.Texture.IsValid()) {
// 		SetIsLoading(false);
// 		if (DefaultTexture) {
// 			PreviewImageWidget->SetBrushFromTexture(DefaultTexture);
// 		}
// 		return;
// 	}
//
// 	if (UTexture2D* Texture2D = InResult.AsTexture2D()) {
// 		PreviewImageWidget->SetBrushFromTexture(Texture2D);
// 	} else if (UAnimatedTexture2D* AnimatedTexture2D = InResult.AsAnimatedTexture2D()) {
// 		PreviewImageWidget->SetBrushResourceObject(AnimatedTexture2D);
// 	}
//
// 	const FVector2D Size = {
// 		InResult.Texture->GetSurfaceWidth(),
// 		InResult.Texture->GetSurfaceHeight()
// 	};
//
// 	PreviewImageWidget->SetDesiredSizeOverride(Size);
//
// 	// todo: consider this as a hack, we need to figure out why this is called before NativeOnListItemObjectSet if the texture is cached.
// 	RUN_DELAYED(0.1f, this, [this](){
// 				SetIsLoading(false);
// 				});
// }

void UUIAvatarListItemWidget::SetIsLoading(const bool bInIsLoading) const {
	if (LoadingWidget) {
		if (bInIsLoading) {
			LoadingWidget->Show();
		} else {
			LoadingWidget->Hide();
		}
	}
}
