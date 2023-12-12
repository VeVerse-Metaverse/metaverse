// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once


#include "UIBaseRootWidget.h"
#include "TextChatRootWidget.generated.h"


/**
 * 
 */
UCLASS(HideDropdown)
class VEUI_API UTextChatRootWidget : public UUIBaseRootWidget {
	GENERATED_BODY()

	DECLARE_EVENT(UTextChatRootWidget, FOnTextChatRootEvent);

public:
#pragma region Widgets

	UPROPERTY(BlueprintReadOnly, Category="Widget", meta=(BindWidget))
	class UWidgetSwitcher* SwitcherWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget", meta=(BindWidget))
	class UTextChatInfoWidget* TextChatInfoWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget", meta=(BindWidget))
	class UTextChatWidget* TextChatWidget;

#pragma endregion Widgets

	/** Time to hide unfocused text chat. */
	UPROPERTY(EditAnywhere, Category="Widget")
	float UnfocusedHideTime = 3.f;

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeOnUpdateInputMode() override;

	void DoShowFullMode();
	void DoHideFullMode();

	UWidget* GetWidgetToFocus() const;

private:
	FTimerHandle HideTimerHandle;

	void TextChatSubsystem_OnShowFullModeCallback();
	void TextChatSubsystem_OnHideFullModeCallback(float Delay);
	void TextChatWidget_OnFocusReceivedCallback();
	void TextChatWidget_OnFocusLostCallback();
	void TextChatWidget_OnCloseCallback();

};
