// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "TextChat/TextChatNotificationItemWidget.h"

#include "Components/TextBlock.h"
#include "RpcWebSocketsTextChatTypes.h"

void UTextChatNotificationItemWidget::SetMessage(const TSharedPtr<FRpcWebSocketsTextChatMessage>& Message) {
	if (!IsValid(MessageTextWidget)) {
		return;
	}
	if (!Message) {
		return;
	}

	FString MessageStr = Message->ToString();

	if (MessageTextLimit && MessageStr.Len() > MessageTextLimit) {
		MessageTextWidget->SetText(FText::FromString(MessageStr.Left(MessageTextLimit) + TEXT("…")));
	} else {
		MessageTextWidget->SetText(FText::FromString(MessageStr));
	}

	MessageTextWidget->SetColorAndOpacity(FSlateColor(Message->GetFontColor()));
}
