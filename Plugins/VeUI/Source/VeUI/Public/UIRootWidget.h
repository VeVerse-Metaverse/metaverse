// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once


#include "VePlayerControllerBase.h"
#include "Blueprint/UserWidget.h"
#include "UIDialogRootWidget.h"
#include "GameFramework/PlayerInput.h"
#include "VeWorldMetadata.h"
#include "UIRootWidget.generated.h"


/**
 * 
 */
UCLASS(HideDropdown, Blueprintable, BlueprintType)
class VEUI_API UUIRootWidget : public UUserWidget {
	GENERATED_BODY()

	DECLARE_DELEGATE(FOnHideMenuDelegate);
	
public:
#pragma	region Widgets

	/** Background widget. */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Widget", meta=(BindWidgetOptional))
	class UUIWidgetBase* BackgroundWidget;

	/** Base widget for notifications. */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Widget", meta=(BindWidgetOptional))
	class UUINotificationManagerWidget* NotificationRootWidget;

	/** Base widget for dialogs. */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Widget", meta=(BindWidgetOptional))
	class UUIDialogRootWidget* DialogRootWidget;

	/** Base widget for main menus. */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Widget", meta=(BindWidgetOptional))
	class UUIMainMenuRootWidget* MainMenuRootWidget;

	/** Base widget for main menus. */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Widget", meta=(BindWidgetOptional))
	class UTextChatRootWidget* TextChatRootWidget;

	/** Base widget for login, register and reset password menus. */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Widget", meta=(BindWidgetOptional))
	class UUIAuthRootWidget* AuthRootWidget;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Widget", meta=(BindWidgetOptional))
	class UUIEditorRootWidget* EditorRootWidget;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Widget", meta=(BindWidgetOptional))
	class UUIInspectorRootWidget* InspectorRootWidget;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Widget", meta=(BindWidgetOptional))
	class UUIDebuggerRootWidget* DebuggerRootWidget;

#pragma	endregion Widgets

	UUIRootWidget(const FObjectInitializer& ObjectInitializer);

	UUIDialogWidget* ShowDialog(const FUIDialogData& InDialogData, const FUIDialogButtonClicked& InOnButtonClicked) const;

	void ShowAuthentication();
	void HideAuthentication();

	UFUNCTION(BlueprintNativeEvent, Category="Metaverse")
	void ShowMenu();
	UFUNCTION(BlueprintNativeEvent, Category="Metaverse")
	void HideMenu();

	void ShowMenuWorldDetail(const FGuid& InWorldId);

	class UUIEditorRootWidget* GetEditorRootWidget() const { return EditorRootWidget; }
	void ShowEditor();
	void HideEditor();

	class UUIInspectorRootWidget* GetInspectorRootWidget() const { return InspectorRootWidget; }
	void ShowInspector();
	void HideInspector();

	class UUIDebuggerRootWidget* GetDebugRootWidget() const { return DebuggerRootWidget; }
	void ShowDebugger();
	void HideDebugger();

	void ShowTextChat();
	void HideTextChat();
	void ToggleTextChatFullMode();
	void ShowTextChatFullMode();
	void HideTextChatFullMode();

	void ShowBackground();
	void HideBackground();
	
	FOnHideMenuDelegate OnHideMenu;

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	/** InputAction name for escape. */
	UPROPERTY(EditAnywhere, Category = "Menu")
	FName EscapeInputAction;

	bool bAuthenticationVisible = false;
	bool bMenuVisible = false;
	bool bEditorVisible = false;
	bool bInspectorVisible = false;
	bool bDebuggerVisible = false;
	bool bTextChatVisible = true;
	bool bTextChatFullMode = false;

	TArray<FInputActionKeyMapping> EscapeActionMappings;

	void UpdateInputMode();
	void SetInputModeGame();
	void Escape();
	void SetPlayerInputMode(EAppMode InInputMode);

	void OnClose(class UUIBaseRootWidget* RootWidget);

private:
	void TextChatSubsystem_OnShowFullMode();
	void TextChatSubsystem_OnHideFullMode(float Delay);

};
