// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "UIDialogWidget.h"
#include "GameFramework/HUD.h"
#include "VeWorldMetadata.h"
#include "VeHUD.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class VEUI_API AVeHUD final : public AHUD {
	GENERATED_BODY()

public:
	AVeHUD();

private:
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditDefaultsOnly, Category="Game")
	float ExitDelay = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category="Widget")
	float DefaultNotificationLifetime = 5.0f;

	UPROPERTY(EditAnywhere, Category="Widget")
	TSubclassOf<class UUIRootWidget> RootWidgetClass;

	UPROPERTY(EditAnywhere, Category="Widget")
	TSubclassOf<class UUIRootWidget> MobileRootWidgetClass;

	UPROPERTY(EditAnywhere, Category="Widget")
	TSubclassOf<class UUserWidget> DefaultHUDWidgetClass;

public:
	void SetVoiceState(const bool bInVoiceActive) const;

	/** Show popup dialog widget. */
	UUIDialogWidget* ShowDialog(const FUIDialogData& DialogData, const FUIDialogButtonClicked& InOnButtonClicked) const;

	/** Add a notification to the notification area. */
	void AddNotification(const FUINotificationData& NotificationData) const;

	/** Request dialog widget to confirm exiting the application. */
	void RequestExitConfirmation();

	class UUIRootWidget* GetRootWidget() const { return RootWidget; }
	class UUIEditorRootWidget* GetEditorRootWidget() const;
	class UUIInspectorRootWidget* GetInspectorRootWidget() const;

	void ShowEditor();
	void HideEditor();

	void ShowInspector();
	void HideInspector();

	bool ShowMenu();
	bool HideMenu();

	UFUNCTION(BlueprintCallable, Category="VeGame")
	UUserWidget* ShowHUDWidget(TSubclassOf<UUserWidget> WidgetClass);

protected:
	/** Root widget [2] for the UI. */
	UPROPERTY(BlueprintReadOnly, Category="Widget")
	TObjectPtr<UUIRootWidget> RootWidget;

	/** HUD widget. */
	UPROPERTY()
	TWeakObjectPtr<UUserWidget> CurrentHUDWidget;

private:
	void RootWidget_OnHideMenu();
	
};
