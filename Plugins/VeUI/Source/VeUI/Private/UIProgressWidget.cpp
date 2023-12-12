// Authors: Khusan T.Yadgarov, Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UIProgressWidget.h"

#include "Materials/MaterialInstanceDynamic.h"

void UUIProgressWidget::NativeConstruct() {
	Super::NativeConstruct();

	if (ProgressBarWidget && !ProgressMID) {
		ProgressMID = UMaterialInstanceDynamic::Create(ProgressMaterial, this);
		if (ProgressMID) {
			ProgressBarWidget->SetBrushFromMaterial(ProgressMID);
		}
	}
}

void UUIProgressWidget::SetAlpha(const float InAlpha) {
	if (ProgressMID) {
		ProgressMID->SetScalarParameterValue(TEXT("Alpha"), InAlpha);
	}
}

void UUIProgressWidget::SetProgressRatio(const float InRatio) {
	if (ProgressMID) {
		ProgressMID->SetScalarParameterValue(TEXT("Ratio"), InRatio);
	}
}
