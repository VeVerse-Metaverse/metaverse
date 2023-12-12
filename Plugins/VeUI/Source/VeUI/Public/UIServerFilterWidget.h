// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "Components/UIInputWidget.h"
#include "UIWidgetBase.h"

#include "Components/ComboBox.h"
#include "Components/ComboBoxString.h"

#include "UIServerFilterWidget.generated.h"

/**
 * 
 */
UCLASS()
class VEUI_API UUIServerFilterWidget final : public UUIWidgetBase
 {
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

	/** Filter by the space name. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIInputWidget* SpaceNameFilterWidget;

	/** Filter by the space tag divided by the comma. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIInputWidget* TagFilterWidget;

	/** Drop-down select to select whether we want to show all, only public or only private (but accessible servers). */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UComboBoxString* AccessFilterWidget;

	/** Drop-down select to select whether we want to show all, only public or only private (but accessible servers). */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UComboBoxString* GameModeFilterWidget;

	/** Submits filters. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* ButtonSearchWidget;
};
