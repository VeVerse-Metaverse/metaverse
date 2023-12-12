// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.


#include "UIDialogRootWidget.h"

UUIDialogRootWidget::UUIDialogRootWidget(): Super() {
	DialogWidgetClass = UUIDialogWidget::StaticClass();
}

UUIDialogWidget* UUIDialogRootWidget::ShowDialog(const FUIDialogData& DialogData, const FUIDialogButtonClicked& InOnButtonClicked) {
	APlayerController* PlayerController = GetOwningPlayer();
	UUIDialogWidget* DialogWidget = CreateWidget<UUIDialogWidget>(DialogWidgetClass);

	if (DialogWidget) {
		RootWidget->AddChild(DialogWidget);

		DialogWidget->Open(DialogData, InOnButtonClicked);
	}

	return DialogWidget;
}
