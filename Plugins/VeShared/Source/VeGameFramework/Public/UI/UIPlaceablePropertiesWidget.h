// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


#pragma once

#include "Blueprint/UserWidget.h"
#include "UIPlaceablePropertiesWidget.generated.h"

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUIPropertyWidgetEventBP);
/**
 * Base class for properties of placeable actors. 
 */
UCLASS(HideDropdown)
class VEGAMEFRAMEWORK_API UUIPlaceablePropertiesWidget : public UUserWidget {
	GENERATED_BODY()

	DECLARE_DELEGATE(FUIPropertyWidgetEvent);
	DECLARE_DELEGATE_OneParam(FUIPropertyWidgetEnabledEvent, bool bInIsEnabled);

public:
	virtual void Save();
	virtual void Reset();

	bool GetSaveEnabled() const { return bSaveEnabled; }
	bool GetResetEnabled() const { return bResetEnabled; }

	FUIPropertyWidgetEvent& GetOnSave() { return OnSave; }
	FUIPropertyWidgetEvent& GetOnReset() { return OnReset; }
	FUIPropertyWidgetEvent& GetOnChanged() { return OnChanged; }
	FUIPropertyWidgetEnabledEvent& GetOnSaveEnableChanged() { return OnSaveEnableChanged; }
	FUIPropertyWidgetEnabledEvent& GetOnResetEnableChanged() { return OnResetEnableChanged; }
	
	//* Save button widget clicked**/
	UPROPERTY(BlueprintAssignable)
	FUIPropertyWidgetEventBP OnSaveBP;

	UPROPERTY(BlueprintAssignable)
	FUIPropertyWidgetEventBP OnResetBP;

	UPROPERTY(BlueprintAssignable)
	FUIPropertyWidgetEventBP OnChangeBP;
	
protected:
	virtual void NativeOnSave();
	virtual void NativeOnReset();
	virtual void NativeOnChanged();

	void SetSaveEnabled(bool bInIsEnabled);
	void SetResetEnabled(bool bInIsEnabled);
	void SetError(const FString& Error);

private:
	bool bSaveEnabled = true;
	bool bResetEnabled = false;
	void NativeOnSaveEnabled();
	void NativeOnResetEnabled();

	FUIPropertyWidgetEvent OnSave;
	FUIPropertyWidgetEvent OnReset;
	FUIPropertyWidgetEvent OnChanged;
	FUIPropertyWidgetEnabledEvent OnSaveEnableChanged;
	FUIPropertyWidgetEnabledEvent OnResetEnableChanged;
};
