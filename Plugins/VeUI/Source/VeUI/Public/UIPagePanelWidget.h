// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once


#include "UIWidgetBase.h"
#include "UIPagePanelWidget.generated.h"

class UUIButtonWidget;
class UUIPageContentWidget;
class UUIPageManagerWidget;
class UUIBreadcrumbItemWidget;
class UUIBreadcrumbDelimiterWidget;

/**
 * 
 */
UCLASS(Abstract)
class VEUI_API UUIPagePanelWidget : public UUIWidgetBase {
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* NextButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* PrevButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UHorizontalBox* PathBoxWidget;

	UPROPERTY(EditAnywhere, Category="Widget")
	TSubclassOf<UUIBreadcrumbItemWidget> BreadcrumbItemClass;

	UPROPERTY(EditAnywhere, Category="Widget")
	TSubclassOf<UUIBreadcrumbDelimiterWidget> BreadcrumbDelimiterClass;

	UPROPERTY(EditAnywhere, Category="Widget")
	TSubclassOf<class UUIBreadcrumbButtonWidget> BreadcrumbButtonClass;

	
	UFUNCTION(BlueprintCallable)
	void SetPageManager(UUIPageManagerWidget* InRootWidget);

	UFUNCTION(BlueprintPure)
	UUIPageManagerWidget* GetPageManager() const { return PageManager.Get(); }

protected:
	virtual void NativeOnInitializedShared() override;
	
	void OnHistoryChangedCallback(UUIPageManagerWidget* InPageManager);
	void OnPageInfoChangedCallback(UUIPageContentWidget* Page);

	void RefreshButtons();
	
private:
	FDelegateHandle OnHistoryChangedDelegateHandle;

	TWeakObjectPtr<UUIPageManagerWidget> PageManager;

};
