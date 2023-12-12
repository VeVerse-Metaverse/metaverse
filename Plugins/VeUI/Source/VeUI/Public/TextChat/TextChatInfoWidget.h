// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "Blueprint/UserWidget.h"
#include "TextChatInfoWidget.generated.h"


/**
 * 
 */
UCLASS()
class VEUI_API UTextChatInfoWidget : public UUserWidget {
	GENERATED_BODY()

	DECLARE_EVENT_OneParam(UTextChatInfoWidget, FOnTextChatMessageSent, const FText& Message);

public:
#pragma region Widgets

	UPROPERTY(BlueprintReadOnly, Category="Widget", meta=(BindWidget))
	class UTextBlock* AmountTextWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget", meta=(BindWidget))
	class UTextChatNotificationManagerWidget* NotificationManagerWidget;

#pragma endregion Widgets

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	FOnTextChatMessageSent OnTextChatMessageSent;

	void TextChatSubsystem_OnMessageReceivedCallback(const TSharedPtr<class FRpcWebSocketsTextChatMessage>& Message);
	void TextChatSubsystem_OnUnreadCountChangedCallback();

};
