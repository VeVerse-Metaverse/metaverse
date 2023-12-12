// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#pragma once

#include "UIWidget.h"
#include "UINavigatableWidget.generated.h"

UCLASS()
class VEUI_API UUINavigatableWidget : public UUIWidget
 {
	GENERATED_BODY()

public:
	/** Register widget with the navigation system at construction. */
	void Register();

	virtual ~UUINavigatableWidget() = default;

	/** Get base path used to identify the widget. */
	virtual FString GetPath();;

	/** Navigate to this widget opening child widget hierarchy as required. */
	virtual void Navigate(const FString& Options = TEXT(""));;
};
