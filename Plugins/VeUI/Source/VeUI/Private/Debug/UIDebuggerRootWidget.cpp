// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "Debug/UIDebuggerRootWidget.h"

#include "Components/EditableText.h"
#include "Components/UIButtonWidget.h"

void UUIDebuggerRootWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();

	Clear();

	if (CloseButtonWidget) {
		CloseButtonWidget->SetIsEnabled(false);
		CloseButtonWidget->GetOnButtonClicked().AddWeakLambda(this, [=] {
			OnClosedDelegate.Broadcast();
		});
	}
}

void UUIDebuggerRootWidget::NativeOnUpdateInputMode() {
	SetInputModeGameAndUi();
}

void UUIDebuggerRootWidget::Clear() {}
