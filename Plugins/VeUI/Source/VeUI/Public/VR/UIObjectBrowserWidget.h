// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "UIBreadcrumbTrailWidget.h"
#include "UIObjectFormWidget.h"
#include "UIFileBrowserWidget.h"
#include "UIWidgetBase.h"

#include "UIObjectBrowserWidget.generated.h"

class UUIObjectListWidget;
class UUIObjectDetailWidget;

/**
 * 
 */
UCLASS()
class VEUI_API UUIObjectBrowserWidget final : public UUIWidgetBase
 {
	GENERATED_BODY()

	virtual void RegisterCallbacks() override;
	virtual void UnregisterCallbacks() override;

	FDelegateHandle OnObjectDetailWidgetClosedDelegateHandle;
	FDelegateHandle OnObjectSelectedDelegateHandle;
	FDelegateHandle OnObjectCreateButtonClickedDelegateHandle;
	FDelegateHandle OnObjectEditButtonClickedDelegateHandle;
	FDelegateHandle OnObjectDeleteButtonClickedDelegateHandle;
	FDelegateHandle OnObjectFormClosedDelegateHandle;
	FDelegateHandle OnObjectBreadcrumbNavigationDelegateHandle;
	FDelegateHandle OnCloseButtonClickedDelegateHandle;


public:
	
	virtual void OnNavigate() override;
	
	virtual void NativeConstruct() override;
	/** List of object. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIObjectListWidget* ObjectListWidget;

	/** Object details. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIObjectDetailWidget* ObjectDetailWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIObjectFormWidget* ObjectFormWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIBreadcrumbTrailWidget* BreadcrumbTrailWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* CloseButtonWidget;

	DECLARE_EVENT(UUIFileBrowserWidget, FOnClosed);
	FOnClosed OnClosedDelegate;

#pragma region Navigation
private:
	bool bObjectListWidgetVisible;
	bool bObjectDetailWidgetVisible;
	bool bObjectFormWidgetVisible;

	/** Fades in the reset password widget. */
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void ShowObjectListWidget(bool bAnimate = true, bool bPlaySound = true);

	/** Fades in the reset password widget. */
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void ShowObjectDetailWidget(bool bAnimate = true, bool bPlaySound = true);

	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void ShowObjectFormWidget(bool bAnimate = true, bool bPlaySound = true);

	/** Fades out the login widget. */
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void HideObjectListWidget(bool bAnimate = true, bool bPlaySound = true);

	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void HideObjectFormWidget(bool bAnimate = true, bool bPlaySound = true);

	/** Fades out the login widget. */
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void HideObjectDetailWidget(bool bAnimate = true, bool bPlaySound = true);

	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void NavigateToObjectList(bool bAnimate = true, bool bPlaySound = true);

	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void NavigateToObjectDetail(bool bAnimate = true, bool bPlaySound = true);

	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void NavigateToObjectForm(bool bAnimate = true, bool bPlaySound = true);
#pragma endregion
};
