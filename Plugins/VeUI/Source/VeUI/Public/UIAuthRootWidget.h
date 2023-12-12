// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "UIBaseRootWidget.h"
#include "UIAuthRootWidget.generated.h"

/**
 * 
 */
UCLASS()
class VEUI_API UUIAuthRootWidget final : public UUIBaseRootWidget {
	GENERATED_BODY()

public:
#pragma region Widgets

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Widget", meta=(BindWidget))
	class UUIPageManagerWidget* PageManagerWidget;

#pragma endregion Widgets

};
