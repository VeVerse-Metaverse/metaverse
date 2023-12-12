// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#pragma once
#include "VeShared.h"

class UUIWidgetBase;
class UUIKeyboardWidget;

class VEUI_API FUIFocusSubsystem final : public IModuleSubsystem {
public:
	const static FName Name;

	virtual void Initialize() override;
	virtual void Shutdown() override;

	UUIWidgetBase* GetFocusedWidget() const;

	void RestoreFocus() const;

	/** Register a new keyboard widget to check focus. */
	void RegisterKeyboard(UUIKeyboardWidget* InKeyboardWidget);

	/** Unregister a keyboard widget. */
	void UnregisterKeyboard(const UUIKeyboardWidget* InKeyboardWidget);

	/** Check if any of registered keyboards is in focus path. */
	bool CheckIfKeyboardIsFocused(uint32 InSlateUserIndex);
	
	/** Update focused widget. */
	void SetFocusedWidget(UUIWidgetBase* InFocusedWidget);

	DECLARE_EVENT_TwoParams(FUIFocusSubsystem, FOnFocusedWidgetChanged, UUIWidgetBase* /*PreviouslyFocusedWidget*/, UUIWidgetBase* /*FocusedWidget*/);

	FOnFocusedWidgetChanged& GetOnFocusedWidgetChanged() { return OnFocusedWidgetChanged; }

private:
	/** List of registered keyboard widgets. */
	TSet<UUIKeyboardWidget*> RegisteredKeyboardWidgets;

	FOnFocusedWidgetChanged OnFocusedWidgetChanged;

	/** Currently focused widget to use. */
	UUIWidgetBase* FocusedWidget = nullptr;

	/** Previously focused widget to restore. */
	UUIWidgetBase* LastFocusedWidget = nullptr;
};

typedef TSharedPtr<FUIFocusSubsystem> FUIFocusSubsystemPtr;
