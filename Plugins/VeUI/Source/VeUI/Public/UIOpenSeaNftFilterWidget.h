// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "Components/UIInputWidget.h"
#include "UIWidgetBase.h"

#include "UIOpenSeaNftFilterWidget.generated.h"

/**
 * 
 */
UCLASS()
class VEUI_API UUIOpenSeaNftFilterWidget : public UUIWidgetBase
 {
	GENERATED_BODY()

protected:
	virtual void RegisterCallbacks() override;
	virtual void UnregisterCallbacks() override;

	FDelegateHandle OnQueryTextCommittedDelegateHandle;
	FDelegateHandle OnSearchButtonClickedDelegateHandle;

	void OnQueryTextCommittedCallback(const FText& InText, const ETextCommit::Type InTextCommit);
	void OnSearchButtonClickedCallback();

public:
	DECLARE_EVENT_OneParam(UUIOpenSeaNftFilterWidget, FOnFilterChanged, const FString&);
	FOnFilterChanged OnFilterChanged;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIInputWidget* QueryInputWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* SearchButtonWidget;
};
