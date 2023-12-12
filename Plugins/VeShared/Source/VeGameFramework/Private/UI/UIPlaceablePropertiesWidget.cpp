// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UI/UIPlaceablePropertiesWidget.h"

#include "VeShared.h"

void UUIPlaceablePropertiesWidget::Save() {
	NativeOnSave();
}

void UUIPlaceablePropertiesWidget::Reset() {
	NativeOnReset();
}

void UUIPlaceablePropertiesWidget::NativeOnSave() {
	OnSave.ExecuteIfBound();
	OnSaveBP.Broadcast();
}

void UUIPlaceablePropertiesWidget::NativeOnReset() {
	OnReset.ExecuteIfBound();
	OnResetBP.Broadcast();
}

void UUIPlaceablePropertiesWidget::NativeOnChanged() {
	OnChanged.ExecuteIfBound();
	OnChangeBP.Broadcast();
}

void UUIPlaceablePropertiesWidget::SetSaveEnabled(bool bInIsEnabled) {
	bSaveEnabled = bInIsEnabled;
	NativeOnSaveEnabled();
}

void UUIPlaceablePropertiesWidget::SetResetEnabled(bool bInIsEnabled) {
	bResetEnabled = bInIsEnabled;
	NativeOnResetEnabled();
}

void UUIPlaceablePropertiesWidget::SetError(const FString& Error) {}

void UUIPlaceablePropertiesWidget::NativeOnSaveEnabled() {
	OnSaveEnableChanged.ExecuteIfBound(bSaveEnabled);
}

void UUIPlaceablePropertiesWidget::NativeOnResetEnabled() {
	OnResetEnableChanged.ExecuteIfBound(bResetEnabled);
}
