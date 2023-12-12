// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "Blueprint/UserWidget.h"
#include "TextChatWidget.generated.h"

/**
 * 
 */
UCLASS()
class VEUI_API UTextChatWidget : public UUserWidget {
	GENERATED_BODY()

	DECLARE_EVENT(UTextChatWidget, FOnTextChatEvent);

public:
#pragma region Widgets

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UTextChatMessagesWidget* MessagesWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UTextChatInputWidget* InputWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UTextChatChannelListWidget* SelectedChannelsWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UTextChatChannelListWidget* DeselectedChannelsWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class USizeBox* DeselectedPanelWidget;

#pragma endregion Widgets

#pragma region Buttons

	UTextChatWidget(const FObjectInitializer& ObjectInitializer);

	/** Show/Hide deselected channels. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UButton* DeselectedButtonWidget;

	UWidget* GetWidgetToFocus() const;

#pragma endregion Buttons

	void Reset();

	FOnTextChatEvent& GetOnFocusReceived() { return OnFocusReceived; }
	FOnTextChatEvent& GetOnFocusLost() { return OnFocusLost; }
	FOnTextChatEvent& GetOnClose() { return OnClose; }

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
	virtual void NativeOnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath, const FFocusEvent& InFocusEvent);

	bool DeselectedPanelGetVisibility() const;
	void DeselectedPanelSetVisibility(bool Visible);

private:
	void InputWidget_OnMessageSentCallback(const FText& Message) const;

	void SelectedChannelsWidget_OnChannelSelectClickedCallback(class UTextChatChannelItemWidget* ChannelItemWidget);
	void SelectedChannelsWidget_OnChannelDeselectClickedCallback(class UTextChatChannelItemWidget* ChannelItemWidget);
	void DeselectedChannelsWidget_OnChannelSelectClickedCallback(class UTextChatChannelItemWidget* ChannelItemWidget);

	void TextChatSubsystem_OnActiveChannelChangedCallback(const TSharedPtr<class FRpcWebSocketsTextChatChannel>& Channel);
	void TextChatSubsystem_OnEnabledCallback();
	void TextChatSubsystem_OnDisabledCallback();

	UFUNCTION()
	void DeselectedButton_OnClickedCallback();

private:
	FOnTextChatEvent OnFocusReceived;
	FOnTextChatEvent OnFocusLost;
	FOnTextChatEvent OnClose;
	bool IsFocused = false;

};
