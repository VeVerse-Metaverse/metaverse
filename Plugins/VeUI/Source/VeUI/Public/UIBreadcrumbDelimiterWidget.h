// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "UIWidgetBase.h"

#include "Components/Image.h"
#include "UIBreadcrumbDelimiterWidget.generated.h"

UCLASS(HideDropdown)
class UUIBreadcrumbDelimiterWidget final : public UUIWidgetBase
 {
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UImage* ImageContent;
};
