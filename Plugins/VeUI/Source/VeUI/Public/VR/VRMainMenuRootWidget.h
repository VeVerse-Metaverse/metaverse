// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "UIAboutWidget.h"
#include "UIAvatarPreviewWidget.h"
#include "UIMainMenuArtheonRootWidget.h"
#include "UIModBrowserWidget.h"
#include "UIObjectBrowserWidget.h"
#include "UIServerBrowserWidget.h"
#include "UISettingsWidget.h"
#include "UIUserBrowserWidget.h"
#include "UIWidget.h"

#include "VRMainMenuRootWidget.generated.h"

class UUISpaceBrowserWidget;
class UUISocialPanelWidget;
class UUIMainMenuWidget;

/**
 * 
 */
UCLASS()
class VEUI_API UVRMainMenuRootWidget final : public UUIWidget {
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
	FDelegateHandle OnAvatarBrowserButtonClickedDelegateHandle;
	FDelegateHandle OnBackMainMenuButtonClickedDelegateHandle;
	FDelegateHandle OnObjectBrowserClosedDelegateHandle;
	FDelegateHandle OnSpaceBrowserClosedDelegateHandle;
	FDelegateHandle OnUserBrowserClosedDelegateHandle;
	FDelegateHandle OnModBrowserClosedDelegateHandle;
	FDelegateHandle OnServerBrowserClosedDelegateHandle;
	FDelegateHandle OnAvatarBrowserClosedDelegateHandle;
	FDelegateHandle OnSettingsClosedDelegateHandle;
	FDelegateHandle OnSettingsSubmitDelegateHandle;

public:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIButtonWidget* PinButtonWidget = nullptr;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIMainMenuWidget* MainMenuWidget = nullptr;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIServerBrowserWidget* ServerBrowserWidget = nullptr;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIAboutWidget* AboutWidget = nullptr;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUISpaceBrowserWidget* SpaceBrowserWidget = nullptr;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIUserBrowserWidget* UserBrowserWidget = nullptr;

	// UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	// UUIObjectBrowserWidget* ObjectBrowserWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIMainMenuArtheonRootWidget* MainMenuArtheonWidget = nullptr;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUISettingsWidget* SettingsWidget = nullptr;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIModBrowserWidget* ModBrowserWidget = nullptr;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUISocialPanelWidget* SocialPanelWidget = nullptr;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIAvatarPreviewWidget* AvatarBrowserWidget = nullptr;

#pragma region Widget Control
	/** Fades in the login widget. */
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void ShowSpaceBrowserWidget(bool bAnimate = true, bool bPlaySound = true);

	/** Fades in the register widget. */
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void ShowUserBrowserWidget(bool bAnimate = true, bool bPlaySound = true);

	/** Fades in the reset password widget. */
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void ShowModBrowserWidget(bool bAnimate = true, bool bPlaySound = true);

	/** Fades in the reset password widget. */
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void ShowSettingsWidget(bool bAnimate = true, bool bPlaySound = true);

	/** Fades in the reset password widget. */
	// UFUNCTION(BlueprintCallable, Category="Widget|Content")
	// void ShowServersWidget(bool bAnimate = true, bool bPlaySound = true);

	/** Fades in the reset password widget. */
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void ShowAboutWidget(bool bAnimate = true, bool bPlaySound = true);

	// UFUNCTION(BlueprintCallable, Category="Widget|Content")
	// void ShowObjectBrowserWidget(bool bAnimate, bool bPlaySound);

	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void ShowMainMenuArtheonWidget(bool bAnimate, bool bPlaySound);

	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void ShowMainMenuWidget(bool bAnimate, bool bPlaySound);

	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void ShowAvatarBrowserWidget(bool bAnimate, bool bPlaySound);

	/** Fades out the login widget. */
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void HideSpaceBrowserWidget(bool bAnimate = true, bool bPlaySound = true);

	/** Fades out the register widget. */
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void HideUserBrowserWidget(bool bAnimate = true, bool bPlaySound = true);

	/** Fades out the ModBrowser widget. */
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void HideModBrowserWidget(bool bAnimate = true, bool bPlaySound = true);

	/** Fades out the Settings widget. */
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void HideSettingsWidget(bool bAnimate = true, bool bPlaySound = true);

	/** Fades out the Servers widget. */
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void HideServersWidget(bool bAnimate = true, bool bPlaySound = true);

	/** Fades out the About widget. */
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void HideAboutWidget(bool bAnimate = true, bool bPlaySound = true);

	// /** Fades out the ObjectBrowser widget. */
	// UFUNCTION(BlueprintCallable, Category="Widget|Content")
	// void HideObjectBrowserWidget(bool bAnimate, bool bPlaySound);

	/** Fades out the ObjectBrowser widget. */
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void HideMainMenuArtheonWidget(bool bAnimate, bool bPlaySound);

	/** Fades out the reset password widget. */
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void HideMainMenuWidget(bool bAnimate, bool bPlaySound);

	/** Fades out the reset password widget. */
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void HideAvatarBrowserWidget(bool bAnimate, bool bPlaySound);

#pragma endregion

private:
#pragma region Navigation
	bool bSpaceBrowserWidgetVisible;
	bool bUserBrowserWidgetVisible;
	bool bModBrowserWidgetVisible;
	bool bSettingsWidgetVisible;
	bool bServerBrowserWidgetVisible;
	bool bAboutWidgetVisible;
	bool bMainWidgetVisible;
	bool bMainMenuWidgetVisible;
	//bool bObjectBrowserWidgetVisible;
	bool bMainMenuArtheonWidgetVisible;
	bool bAvatarBrowserWidgetVisible;

public:
	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void NavigateToSpaceBrowserScreen(bool bAnimate = true, bool bPlaySound = true);

	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void NavigateToUserBrowserScreen(bool bAnimate = true, bool bPlaySound = true);

	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void NavigateToModBrowserScreen(bool bAnimate = true, bool bPlaySound = true);

	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void NavigateToSettingsScreen(bool bAnimate = true, bool bPlaySound = true);

	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void NavigateToServerBrowserScreen(bool bAnimate = true, bool bPlaySound = true);

	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void NavigateToMainScreen(bool bAnimate = true, bool bPlaySound = true);

	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void NavigateToAboutScreen(bool bAnimate = true, bool bPlaySound = true);

	// UFUNCTION(BlueprintCallable, Category="Widget|Content")
	// void NavigateToObjectBrowserScreen(bool bAnimate = true, bool bPlaySound = true);

	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void NavigateToMainMenuArtheonWidget(bool bAnimate = true, bool bPlaySound = true);

	UFUNCTION(BlueprintCallable, Category="Widget|Content")
	void NavigateToAvatarBrowserWidget(bool bAnimate = true, bool bPlaySound = true);

	// UFUNCTION(BlueprintCallable, Category="Widget|Content")
	// void NavigateToMainMenuScreen(bool bAnimate = true, bool bPlaySound = true);

#pragma endregion
};
