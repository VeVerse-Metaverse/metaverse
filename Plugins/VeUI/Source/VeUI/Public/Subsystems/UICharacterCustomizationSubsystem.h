// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#pragma once

#include "VR/VRRootWidget.h"
#include "VeShared.h"

class FApiPersonaMetadata;
class FApiFileMetadata;

class VEUI_API FUICharacterCustomizationSubsystem final : public IModuleSubsystem {
public:
	const static FName Name;

	FGuid CurrentPersonaId;
	FGuid GetCurrentPersonaId();
	
	virtual void Initialize() override;
	virtual void Shutdown() override;

	DECLARE_EVENT(FUICharacterCustomizationSubsystem, FOnCustomizerOpened);

	DECLARE_EVENT(FUICharacterCustomizationSubsystem, FOnCustomizerClosed);

	DECLARE_EVENT_OneParam(FUICharacterCustomizationSubsystem, FNotifyAvatarLoaded, const FGuid& PersonaId);
	
	DECLARE_EVENT(FUICharacterCustomizationSubsystem, FNotifyLe7elAvatarLoaded);

	DECLARE_EVENT_ThreeParams(FUICharacterCustomizationSubsystem, FNotifyPersonaImageUpdated, const FGuid& PersonaId, const bool bInSuccessful, const FString& InError);

	DECLARE_EVENT_OneParam(FUICharacterCustomizationSubsystem, FOnCustomizerClicked, TSharedPtr<FApiPersonaMetadata>);

	DECLARE_EVENT_OneParam(FUICharacterCustomizationSubsystem, FOnLe7elCustomizerClicked, TSharedPtr<FApiPersonaMetadata>);

	DECLARE_EVENT_OneParam(FUICharacterCustomizationSubsystem, FOnBroadcastCreatePersonaRequest, FApiPersonaMetadata InMetadata);
	
	DECLARE_EVENT_OneParam(FUICharacterCustomizationSubsystem, FOnCurrentPersonaIdChanged, const FGuid& PersonaId);

	DECLARE_EVENT_OneParam(FUICharacterCustomizationSubsystem, FOnCreatePersonaClicked, FApiFileMetadata InMetadata);
	
	DECLARE_EVENT_OneParam(FUICharacterCustomizationSubsystem, FOnLe7elCreatePersonClicked, FApiPersonaMetadata InMetadata);

	
	DECLARE_EVENT(FUICharacterCustomizationSubsystem, FCharacterCustomizationSubsystemEvent);
	DECLARE_EVENT_OneParam(FUICharacterCustomizationSubsystem, FCharacterCustomizationPersonaId, const FGuid& PersonaId);
	
private:
	FOnCustomizerOpened OnCustomizerOpened;
	FOnCustomizerClosed OnCustomizerClosed;
	FOnCustomizerOpened OnCustomizerPreviewOpened;
	FOnCustomizerClosed OnCustomizerPreviewClosed;

	FNotifyLe7elAvatarLoaded OnNotifyLe7elAvatarLoaded;
	FOnCustomizerClicked OnAvatarListItemClickedCustomizer;
	FOnLe7elCustomizerClicked OnLe7elAvatarListItemClickedCustomizer;
	FOnCreatePersonaClicked OnPersonaCreated;
	FOnLe7elCreatePersonClicked OnLe7elCreatePersonaCustomizer;
	FOnBroadcastCreatePersonaRequest OnBroadcastCreatePersonaRequest;
	FOnCurrentPersonaIdChanged OnCurrentPersonaIdChanged;

	FCharacterCustomizationPersonaId OnPersonaSelected;
	FCharacterCustomizationSubsystemEvent OnUpdatePersonaFullPreview;
	FCharacterCustomizationSubsystemEvent OnUpdatePersonaFacePreview;
	FNotifyAvatarLoaded OnNotifyPersonaLoaded;

public:
	FOnCustomizerOpened& GetOnCustomizerOpened() { return OnCustomizerOpened; }
	FOnCustomizerClosed& GetOnCustomizerClosed() { return OnCustomizerClosed; }
	FOnCustomizerOpened& GetOnCustomizerPreviewOpened() { return OnCustomizerPreviewOpened; }
	FOnCustomizerClosed& GetOnCustomizerPreviewClosed() { return OnCustomizerPreviewClosed; }

	FNotifyLe7elAvatarLoaded& GetNotifyLe7elAvatarLoaded() { return OnNotifyLe7elAvatarLoaded; }
	FOnCustomizerClicked& GetNotifyAvatarListItemClickedCustomizer() { return OnAvatarListItemClickedCustomizer; }
	FOnLe7elCustomizerClicked& GetNotifyLe7elAvatarListItemClickedCustomizer() { return OnLe7elAvatarListItemClickedCustomizer; }
	FOnLe7elCreatePersonClicked& GetNotifyLe7elCreatePersonaCustomizer() { return OnLe7elCreatePersonaCustomizer; }
	FOnBroadcastCreatePersonaRequest& GetNotifyCreatePersonaRequest() { return OnBroadcastCreatePersonaRequest; }
	
	/** Call from UIAvatarPage on ListItemClickedResponse event */
	FOnCurrentPersonaIdChanged& GetNotifyCurrentPersonaIdChanged() { return OnCurrentPersonaIdChanged; }

	/** Call from UIAvatarFormPage */
	FOnCreatePersonaClicked& GetNotifyPersonaCreated() { return OnPersonaCreated; }

	/** Call from UIAvatarPage on ListItemClicked event (change of Default Persona) */
	FCharacterCustomizationPersonaId& GetNotifyPersonaSelected() { return OnPersonaSelected; }

	/** Call from Character on AvatarLoaded event */
	FNotifyAvatarLoaded& GetNotifyPersonaLoaded() { return OnNotifyPersonaLoaded; }

	/** Call from UIAvatarPage for PersonaFullPreview capture */
	FCharacterCustomizationSubsystemEvent& GetNotifyUpdatePersonaFullPreview() { return OnUpdatePersonaFullPreview; }

	/** Call from UIAvatarPage for PersonaFacePreview capture */
	FCharacterCustomizationSubsystemEvent& GetNotifyUpdatePersonaFacePreview() { return OnUpdatePersonaFacePreview; }

	void NotifyAvatarListItemClickedCustomizer(const TSharedPtr<FApiPersonaMetadata> InMetadata);
	void NotifyLE7ELAvatarListItemClickedCustomizer(const TSharedPtr<FApiPersonaMetadata> InMetadata);
	void NotifyCreatePersonaRequest(const FApiPersonaMetadata& InMetadata);
	void NotifyCreateLE7ELPersonaRequest(const FApiPersonaMetadata& InMetadata);
	void NotifyLe7elAvatarLoaded();
	void NotifyCurrentPersonaIdChanged(const FGuid PersonaId);

	/** replace for: NotifyLe7elAvatarListItemClickedCustomizer */
	void NotifyPersonaSelected(const FGuid& PersonaId);
	void NotifyPersonaCreated(const FApiFileMetadata& InMetadata);
	void NotifyPersonaLoaded(const FGuid& PersonaId);
	void UpdatePersonaFullPreview();
	void UpdatePersonaFacePreview();

	void OpenCustomizer() const;
	void CloseCustomizer() const;
	void OpenCustomizerPreview() const;
	void CloseCustomizerPreview() const;
};

typedef TSharedPtr<FUICharacterCustomizationSubsystem> FUICharacterCustomizationSubsystemPtr;
