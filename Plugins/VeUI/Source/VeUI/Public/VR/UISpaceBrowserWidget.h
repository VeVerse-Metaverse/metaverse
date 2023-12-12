// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "UISpaceFormWidget.h"
#include "UIWidget.h"
#include "UISpaceBrowserWidget.generated.h"

class UUISpaceListWidget;
//class UUISpaceFilterWidget;
class UUISpaceDetailWidget;


/**
 * 
 */
UCLASS()
class VEUI_API UUISpaceBrowserWidget final : public UUIWidget {
	GENERATED_BODY()

	virtual void RegisterCallbacks() override;
	virtual void UnregisterCallbacks() override;
	
	FDelegateHandle OnSpaceDetailWidgetClosedDelegateHandle;
	FDelegateHandle OnSpaceSelectedDelegateHandle;
	FDelegateHandle OnSpaceCreateButtonClickedDelegateHandle;
	FDelegateHandle OnSpaceEditButtonClickedDelegateHandle;
	FDelegateHandle OnSpaceFormClosedDelegateHandle;
	FDelegateHandle OnCloseButtonClickedDelegateHandle;

public:
	virtual void NativeConstruct() override;

	virtual void OnNavigate() override;

	/** List of space. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUISpaceListWidget* SpaceListWidget;

	/** Space details. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUISpaceDetailWidget* SpaceDetailWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUISpaceFormWidget* SpaceFormWidget;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* CloseButtonWidget;
	
	DECLARE_EVENT(UUIFileBrowserWidget, FOnClosed);
	FOnClosed OnClosedDelegate;
	
#pragma region Navigation
private:
	bool bSpaceListWidgetVisible;
	bool bSpaceDetailWidgetVisible;
	bool bSpaceFormWidgetVisible;

	/** Fades in the reset password widget. */
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void ShowSpaceListWidget(bool bAnimate = true, bool bPlaySound = true);

	/** Fades in the reset password widget. */
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void ShowSpaceDetailWidget(bool bAnimate = true, bool bPlaySound = true);

	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void ShowSpaceFormWidget(bool bAnimate = true, bool bPlaySound = true);

	/** Fades out the login widget. */
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void HideSpaceListWidget(bool bAnimate = true, bool bPlaySound = true);

	/** Fades out the login widget. */
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void HideSpaceDetailWidget(bool bAnimate = true, bool bPlaySound = true);

	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void HideSpaceFormWidget(bool bAnimate = true, bool bPlaySound = true);

	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void NavigateToSpaceList(bool bAnimate = true, bool bPlaySound = true);

	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void NavigateToSpaceDetail(bool bAnimate = true, bool bPlaySound = true);

	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void NavigateToSpaceForm(bool bAnimate = true, bool bPlaySound = true);
#pragma endregion
};
