// 

#pragma once

#include "UIWidgetBase.h"
#include "UIBaseRootWidget.generated.h"

/**
 * 
 */
UCLASS()
class VEUI_API UUIBaseRootWidget : public UUserWidget {
	GENERATED_BODY()

	friend class UUIRootWidget;

	DECLARE_EVENT_OneParam(UUIBaseRootWidget, FUIBaseRootWidgetEvent, UUIBaseRootWidget* RootWidget);

public:
	FUIBaseRootWidgetEvent OnClose;

protected:
	virtual void NativeOnInitialized() override;

	void Show();
	void Hide();
	void UpdateInputMode();

	virtual void NativeOnShow();
	virtual void NativeOnHide();
	virtual void NativeOnClose();
	virtual void NativeOnUpdateInputMode();

	virtual void SetInputModeUiOnly(UWidget* InWidgetToFocus = nullptr);
	virtual void SetInputModeGameAndUi();
	virtual void SetInputModeGame();

};
