// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "Components/UIInputWidget.h"
#include "UIWidget.h"
#include "UISpaceFilterWidget.generated.h"

/**
 * 
 */
UCLASS()
class VEUI_API UUISpaceFilterWidget : public UUIWidget {
	GENERATED_BODY()

protected:
	virtual void RegisterCallbacks() override;
	virtual void UnregisterCallbacks() override;

	FDelegateHandle OnQueryTextCommittedDelegateHandle;
	FDelegateHandle OnSearchButtonClickedDelegateHandle;

	void OnQueryTextCommittedCallback(const FText& InText, const ETextCommit::Type InTextCommit);
	void OnSearchButtonClickedCallback();

public:
	DECLARE_EVENT_OneParam(UUISpaceFilterWidget, FOnFilterChanged, const FString&);
	FOnFilterChanged OnFilterChanged;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIInputWidget* QueryInputWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* SearchButtonWidget;
};
