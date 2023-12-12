// Copyright (c) 2022 LE7EL AS. All right reserved.

#pragma once

#include "UIBaseRootWidget.h"
#include "UIDebuggerRootWidget.generated.h"

/**  */
UCLASS(HideDropdown)
class VEUI_API UUIDebuggerRootWidget final : public UUIBaseRootWidget {
	GENERATED_BODY()

public:
#pragma region Widgets

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIButtonWidget* CloseButtonWidget;

	/** Object preview container. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UPanelWidget* ContainerWidget;

#pragma endregion

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeOnUpdateInputMode() override;

	TWeakObjectPtr<UUserWidget> DebugWidget;

	/** Resets widget state. */
	void Clear();
public:
	DECLARE_EVENT(UUIInspectorWidget, FOnClosed);

	FOnClosed OnClosedDelegate;
};
