// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "UIMainMenuArtheonWidget.h"
#include "UIObjectBrowserWidget.h"
#include "UIOpenSeaAssetBrowserWidget.h"
#include "UIWidget.h"

#include "UIMainMenuArtheonRootWidget.generated.h"

class UUISpaceBrowserWidget;

/**
 * 
 */
UCLASS()
class VEUI_API UUIMainMenuArtheonRootWidget final : public UUIWidget {
	GENERATED_BODY()

	virtual void RegisterCallbacks() override;
	virtual void UnregisterCallbacks() override;

	FDelegateHandle OnMainMenuModsButtonClickedDelegateHandle;
	FDelegateHandle OnMainMenuSpacesButtonClickedDelegateHandle;
	FDelegateHandle OnMainMenuUsersButtonClickedDelegateHandle;
	FDelegateHandle OnMainMenuSettingsButtonClickedDelegateHandle;
	FDelegateHandle OnMainMenuServersButtonClickedDelegateHandle;
	FDelegateHandle OnMainMenuAboutButtonClickedDelegateHandle;
	FDelegateHandle OnMainMenuAboutButtonCloseDelegateHandle;
	FDelegateHandle OnMainMenuButtonCloseDelegateHandle;
	FDelegateHandle OnMainMenuObjectsButtonClickedDelegateHandle;
	FDelegateHandle OnOpenSeaNFTsButtonClickedDelegateHandle;
	FDelegateHandle OnOpenSeaNFTsCloseButtonClickedDelegateHandle;
	FDelegateHandle OnObjectBrowserClosedDelegateHandle;
	FDelegateHandle OnSpaceBrowserClosedDelegateHandle;
	FDelegateHandle OnUserBrowserClosedDelegateHandle;
	FDelegateHandle OnModBrowserClosedDelegateHandle;
	FDelegateHandle OnServerBrowserClosedDelegateHandle;

public:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIMainMenuArtheonWidget* MainMenuWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUISpaceBrowserWidget* SpaceBrowserWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIObjectBrowserWidget* ObjectBrowserWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIOpenSeaAssetBrowserWidget* OpenSeaAssetBrowserWidget;


#pragma region Widget Control
	/** Fades in the login widget. */
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void ShowSpaceBrowserWidget(bool bAnimate = true, bool bPlaySound = true);

	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void ShowObjectBrowserWidget(bool bAnimate, bool bPlaySound);

	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void ShowMainMenuWidget(bool bAnimate, bool bPlaySound);

	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void ShowOpenSeaNFTsBrowserWidget(bool bAnimate, bool bPlaySound);

	/** Fades out the login widget. */
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void HideSpaceBrowserWidget(bool bAnimate = true, bool bPlaySound = true);

	/** Fades out the ObjectBrowser widget. */
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void HideObjectBrowserWidget(bool bAnimate, bool bPlaySound);

	/** Fades out the reset password widget. */
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void HideMainMenuWidget(bool bAnimate, bool bPlaySound);

	/** Fades out the reset password widget. */
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void HideOpenSeaNFTsBrowserWidget(bool bAnimate, bool bPlaySound);

#pragma endregion

private:
#pragma region Navigation
	bool bSpaceBrowserWidgetVisible;
	bool bMainWidgetVisible;
	bool bMainMenuWidgetVisible;
	bool bOpenSeaNFTsBrowserWidgetVisible;
	bool bObjectBrowserWidgetVisible;


public:
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void NavigateToSpaceBrowserScreen(bool bAnimate = true, bool bPlaySound = true);

	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void NavigateToMainScreen(bool bAnimate = true, bool bPlaySound = true);

	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void NavigateToObjectBrowserScreen(bool bAnimate = true, bool bPlaySound = true);

	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void NavigateToOpenSeaNFTsBrowserScreen(bool bAnimate = true, bool bPlaySound = true);

	// UFUNCTION(BlueprintCallable, Category="Widget|Content")
	// void NavigateToMainMenuScreen(bool bAnimate = true, bool bPlaySound = true);

#pragma endregion
};
