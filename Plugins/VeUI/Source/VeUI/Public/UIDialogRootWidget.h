// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "UIDialogWidget.h"
#include "UIDialogRootWidget.generated.h"

/**
 * 
 */
UCLASS()
class VEUI_API UUIDialogRootWidget : public UUIWidgetBase {
	GENERATED_BODY()

public:
#pragma region Widgets

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UPanelWidget* RootWidget;

#pragma endregion Widgets

	UUIDialogRootWidget();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget")
	TSubclassOf<UUIDialogWidget> DialogWidgetClass;

	UUIDialogWidget* ShowDialog(const FUIDialogData& DialogData, const FUIDialogButtonClicked& InOnButtonClicked);
};
