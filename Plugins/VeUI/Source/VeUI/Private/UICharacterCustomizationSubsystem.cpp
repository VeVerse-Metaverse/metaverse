// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "Subsystems/UICharacterCustomizationSubsystem.h"

#include "VeShared.h"
#include "VeApi2.h"
#include "Api2AuthSubsystem.h"


const FName FUICharacterCustomizationSubsystem::Name = "UICharacterCustomizationSubsystem";

FGuid FUICharacterCustomizationSubsystem::GetCurrentPersonaId() {
	if (!CurrentPersonaId.IsValid()) {
		GET_MODULE_SUBSYSTEM(AuthSubsystem, Api2, Auth);
		if (AuthSubsystem) {
			CurrentPersonaId = AuthSubsystem->GetUserMetadata().DefaultPersonaMetadata.Id;
		}
	}
	
	return CurrentPersonaId;
}

void FUICharacterCustomizationSubsystem::Initialize() {
}

void FUICharacterCustomizationSubsystem::Shutdown() {
}

void FUICharacterCustomizationSubsystem::NotifyAvatarListItemClickedCustomizer(const TSharedPtr<FApiPersonaMetadata> InMetadata) {
	if (OnAvatarListItemClickedCustomizer.IsBound()) {
		OnAvatarListItemClickedCustomizer.Broadcast(InMetadata);
	}
}

void FUICharacterCustomizationSubsystem::NotifyLE7ELAvatarListItemClickedCustomizer(const TSharedPtr<FApiPersonaMetadata> InMetadata) {
	if (OnLe7elAvatarListItemClickedCustomizer.IsBound()) {
		OnLe7elAvatarListItemClickedCustomizer.Broadcast(InMetadata);
	}
}

void FUICharacterCustomizationSubsystem::NotifyCreatePersonaRequest(const FApiPersonaMetadata& InMetadata) {
	if (OnBroadcastCreatePersonaRequest.IsBound()) {
		OnBroadcastCreatePersonaRequest.Broadcast(InMetadata);
	}
}

void FUICharacterCustomizationSubsystem::NotifyCreateLE7ELPersonaRequest(const FApiPersonaMetadata& InMetadata) {
	if (OnLe7elCreatePersonaCustomizer.IsBound()) {
		OnLe7elCreatePersonaCustomizer.Broadcast(InMetadata);
	}
}

void FUICharacterCustomizationSubsystem::NotifyPersonaCreated(const FApiFileMetadata& InMetadata) {
	if (OnPersonaCreated.IsBound()) {
		OnPersonaCreated.Broadcast(InMetadata);
	}
}

void FUICharacterCustomizationSubsystem::NotifyPersonaLoaded(const FGuid& PersonaId) {
	if (OnNotifyPersonaLoaded.IsBound()) {
		OnNotifyPersonaLoaded.Broadcast(PersonaId);
	}
}

void FUICharacterCustomizationSubsystem::NotifyLe7elAvatarLoaded() {
	if (OnNotifyLe7elAvatarLoaded.IsBound()) {
		OnNotifyLe7elAvatarLoaded.Broadcast();
	}
}

void FUICharacterCustomizationSubsystem::NotifyCurrentPersonaIdChanged(const FGuid PersonaId) {
	CurrentPersonaId = PersonaId;
	if (OnCurrentPersonaIdChanged.IsBound()) {
		OnCurrentPersonaIdChanged.Broadcast(CurrentPersonaId);
	}
}

void FUICharacterCustomizationSubsystem::NotifyPersonaSelected(const FGuid& PersonaId) {
	if (OnPersonaSelected.IsBound()) {
		OnPersonaSelected.Broadcast(PersonaId);
	}
}

void FUICharacterCustomizationSubsystem::UpdatePersonaFullPreview() {
	if (OnUpdatePersonaFullPreview.IsBound()) {
		OnUpdatePersonaFullPreview.Broadcast();
	}
}

void FUICharacterCustomizationSubsystem::UpdatePersonaFacePreview() {
	if (OnUpdatePersonaFacePreview.IsBound()) {
		OnUpdatePersonaFacePreview.Broadcast();
	}
}

void FUICharacterCustomizationSubsystem::OpenCustomizer() const {
	if (OnCustomizerOpened.IsBound()) {
		OnCustomizerOpened.Broadcast();
	}
}

void FUICharacterCustomizationSubsystem::CloseCustomizer() const {
	if (OnCustomizerClosed.IsBound()) {
		OnCustomizerClosed.Broadcast();
	}
}

void FUICharacterCustomizationSubsystem::OpenCustomizerPreview() const {
	if (OnCustomizerPreviewOpened.IsBound()) {
		OnCustomizerPreviewOpened.Broadcast();
	}
}

void FUICharacterCustomizationSubsystem::CloseCustomizerPreview() const {
	if (OnCustomizerPreviewClosed.IsBound()) {
		OnCustomizerPreviewClosed.Broadcast();
	}
}
