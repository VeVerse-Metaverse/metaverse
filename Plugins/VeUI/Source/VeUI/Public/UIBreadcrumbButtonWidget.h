// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "Components/UIButtonWidget.h"
#include "UIWidgetBase.h"
#include "UIBreadcrumbButtonWidget.generated.h"


UCLASS(HideDropdown)
class UUIBreadcrumbButtonWidget final : public UUIWidgetBase
 {
	DECLARE_MULTICAST_DELEGATE(FOnBreadcrumbButtonClicked);
	
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* ButtonWidget;
	
	void SetText(const FText& InText) const;

	FOnBreadcrumbButtonClicked& GetOnButtonClicked() { return OnButtonClicked; }

protected:
	virtual void NativeOnInitializedShared() override;
	
private:
	/** Called when the button is clicked */
	FOnBreadcrumbButtonClicked OnButtonClicked;
	
 };
