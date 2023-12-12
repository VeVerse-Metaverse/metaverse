// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "Components/UIButtonWidget.h"
#include "UIWidgetBase.h"
#include "UIBreadcrumbItemWidget.generated.h"

UCLASS(HideDropdown)
class UUIBreadcrumbItemWidget final : public UUIWidgetBase
 {
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* ButtonWidget;

	void SetText(const FText& InText) const;
};
