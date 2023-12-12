// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.
#pragma once

#include "UIWidgetBase.h"
#include "UIInspectorModelPreviewWidget.generated.h"


/**
 * 
 */
UCLASS(HideDropdown)
class VEGAME_API UUIInspectorModelPreviewWidget : public UUIWidgetBase {
	GENERATED_BODY()

public:
#pragma region Widgets

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UImage* PreviewImageWidget;

#pragma endregion Widgets

};
