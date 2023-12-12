// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "Blueprint/UserWidget.h"
#include "RpcWebSocketsTextChatChannel.h"
#include "TextChatChannelItemWidget.generated.h"


/**
 * 
 */
UCLASS(HideDropdown)
class VEUI_API UTextChatChannelItemWidget : public UUserWidget {
	GENERATED_BODY()

	DECLARE_EVENT_OneParam(UTextChatBookmarkItemWidget, FOnChannelItemWidgetEvent, UTextChatChannelItemWidget* ItemWidget);

	enum class ERpcTextChatChannelStatus : uint8 {
		Default,
		Active,
		Unread
	};

public:
#pragma region Widgets

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UBorder* RootWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UTextBlock* NameTextWidget;
	
#pragma endregion Widgets

#pragma region Buttons

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UButton* SelectButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UButton* DeselectButtonWidget;

#pragma endregion Buttons

	UPROPERTY(EditAnywhere)
	FLinearColor ColorDefault = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);

	UPROPERTY(EditAnywhere)
	FLinearColor ColorActive = FLinearColor(0.4f, 1.0f, 0.4f, 1.0f);

	UPROPERTY(EditAnywhere)
	FLinearColor ColorUnread = FLinearColor(1.0f, 1.0f, 0.4f, 1.0f);
	
	TSharedPtr<FRpcWebSocketsTextChatChannel> GetChannel() const { return Channel; }
	void SetChannel(const TSharedRef<FRpcWebSocketsTextChatChannel>& InChannel);

	FOnChannelItemWidgetEvent& GetOnSelectClicked() { return OnSelectClicked; }
	FOnChannelItemWidgetEvent& GetOnDeselectClicked() { return OnDeselectClicked; }

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	TSharedPtr<FRpcWebSocketsTextChatChannel> Channel;

	ERpcTextChatChannelStatus Status = ERpcTextChatChannelStatus::Default;
	void SetStatus(ERpcTextChatChannelStatus InStatus);

private:
	FOnChannelItemWidgetEvent OnSelectClicked;
	FOnChannelItemWidgetEvent OnDeselectClicked;

	void TextChatSubsystem_OnChannelSelectChangedCallback(const TSharedPtr<FRpcWebSocketsTextChatChannel>& InChannel, bool Selected, bool CanDeselect);
	void TextChatSubsystem_OnActiveChannelChangedCallback(const TSharedPtr<FRpcWebSocketsTextChatChannel>& InChannel);
	void Channel_OnUnreadNumChangedCallback(const TSharedPtr<FRpcWebSocketsTextChatChannel>& InChannel);
	
	UFUNCTION()
	void SelectButton_OnClickedCallback();

	UFUNCTION()
	void DeselectButton_OnClickedCallback();
	
};
