// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "VeShared.h"


class UEditorComponent;
class UPlaceableComponent;


class VEGAMEFRAMEWORK_API FGameFrameworkEditorSubsystem final : public IModuleSubsystem {
	DECLARE_EVENT(FGameFrameworkEditorSubsystem, FOnGameFrameworkEditorSubsystemEvent);

public:
	static const FName Name;

	virtual void Initialize() override;
	virtual void Shutdown() override;

	/** Registers the active editor component with the subsystem. */
	void SetEditorComponent(UEditorComponent* InEditorComponent);

	UEditorComponent* GetEditorComponent() const { return EditorComponent.Get(); }

	/** Unregisters the active editor component. */
	void ResetEditorComponent();

	DECLARE_EVENT_TwoParams(FGameFrameworkEditorSubsystem, FOnEditorComponentChanged, UEditorComponent* NewEditorComponent, UEditorComponent* OldEditorComponent);

	/** Called when an editor component is being registered or unregistered. */
	FOnEditorComponentChanged& GetOnEditorComponentChanged() { return OnEditorComponentChanged; }

	bool IsEditorModeEnabled() const { return bIsEditorModeEnabled; }
	void SetIsEditorModeEnabled(bool Value);	
	FOnGameFrameworkEditorSubsystemEvent& GetOnEditorModeEnabled() { return OnEditorModeEnabled; }
	FOnGameFrameworkEditorSubsystemEvent& GetOnEditorModeDisabled() { return OnEditorModeDisabled; }
	
private:
	/**
	 * Currently active local editor component.
	 * @note We assume that only one player controller having editor component present in the world at any time.
	 */
	TWeakObjectPtr<UEditorComponent> EditorComponent = nullptr;

	FOnEditorComponentChanged OnEditorComponentChanged;

	bool bIsEditorModeEnabled = false;
	FOnGameFrameworkEditorSubsystemEvent OnEditorModeEnabled;
	FOnGameFrameworkEditorSubsystemEvent OnEditorModeDisabled;

};
