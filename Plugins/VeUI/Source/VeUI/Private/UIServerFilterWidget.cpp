// Authors: Khusan T.Yadgarov, Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UIServerFilterWidget.h"

void UUIServerFilterWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();
}

void UUIServerFilterWidget::NativeConstruct() {
	if (ButtonSearchWidget) {
		if (!ButtonSearchWidget->GetOnButtonClicked().IsBoundToObject(this)) {
			ButtonSearchWidget->GetOnButtonClicked().AddWeakLambda(this, [this]() {
				// if (UApiSubsystem* Api = GetApiSubsystem()) {
				// 	Api->GetServers();
				// }
			});
		}
	}

	Super::NativeConstruct();
}
