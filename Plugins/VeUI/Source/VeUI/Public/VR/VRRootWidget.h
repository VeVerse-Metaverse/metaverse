// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once


#include "Blueprint/UserWidget.h"
#include "UIDialogRootWidget.h"
#include "GameFramework/PlayerInput.h"
#include "VRRootWidget.generated.h"


/**
 * 
 */
UCLASS(HideDropdown)
class VEUI_API UVRRootWidget : public UUserWidget {
	GENERATED_BODY()

public:
#pragma	region Widgets
	
	/** Background widget. */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Widget", meta=(BindWidgetOptional))
	class UUIWidget* BackgroundWidget;

	/** Base widget for notifications. */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Widget", meta=(BindWidgetOptional))
	class UUINotificationRootWidget* NotificationRootWidget;

	/** Base widget for dialogs. */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Widget", meta=(BindWidgetOptional))
	class UUIDialogRootWidget* DialogRootWidget;

	/** Base widget for login, register and reset password menus. */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Widget", meta=(BindWidgetOptional))
	class UUIAuthMenuRootWidget* AuthRootWidget;

	/** Base widget for main menus. */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Widget", meta=(BindWidgetOptional))
	class UVRMainMenuRootWidget* MainMenuRootWidget;

#pragma	endregion Widgets

	UVRRootWidget(const FObjectInitializer& ObjectInitializer);
	
	UUIDialogWidget* ShowDialog(const FUIDialogData& InDialogData, const FUIDialogButtonClicked& InOnButtonClicked) const;

	void AddNotification(const FUINotificationData& InNotificationData);

	void ShowAuthorizationDialog();
	void HideAuthorizationDialog();
	void ToggleMenu();
	void ShowMenu();
	void HideMenu();
	
protected:
	virtual void NativeOnInitialized() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	
	/** InputAction name for escape. */
	UPROPERTY(EditAnywhere, Category = "Menu")
	FName EscapeInputAction;

	bool bAuthRootWidgetVisible;
	bool bMainMenuRootWidgetVisible;

	TArray<FInputActionKeyMapping> EscapeActionMappings;

	void SetInputModeUIOnly();
	void SetInputModeGame();
	void Escape();

};
