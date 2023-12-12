// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include <Components/ScrollBoxSlot.h>

#include "Blueprint/UserWidget.h"
#include "RpcWebSocketsTextChatTypes.h"
#include "TextChatMessagesWidget.generated.h"

class FRpcWebSocketsTextChatChannel;

/**
 * 
 */
UCLASS()
class VEUI_API UTextChatMessagesWidget : public UUserWidget {
	GENERATED_BODY()

public:
#pragma region Widgets

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UScrollBox* ScrollBoxWidget;
	
#pragma endregion Widgets

#pragma region Buttons
#pragma endregion Buttons

	UTextChatMessagesWidget(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(EditAnywhere)
	FMargin MessagePadding;
	
	UPROPERTY(EditAnywhere)
	FSlateFontInfo MessageFont;

	void SetChannel(const TSharedPtr<FRpcWebSocketsTextChatChannel>& InChannel);
	void SetAutoScroll(bool Value);
	
protected:
	void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	TSharedPtr<FRpcWebSocketsTextChatChannel> Channel;

public:
	void AddMessage(const TSharedPtr<FRpcWebSocketsTextChatMessage>& Message);
	
private:
	FTimerHandle TimerScrolling;

	bool bAutoScroll = true;
	
	void Channel_OnMessageReceivedCallback(const TSharedPtr<FRpcWebSocketsTextChatChannel>& InChannel, const TSharedPtr<FRpcWebSocketsTextChatMessage>& InMessage);

	UFUNCTION()
	void ScrollBox_OnUserScrolledCallback(float CurrentOffset);
	
};
