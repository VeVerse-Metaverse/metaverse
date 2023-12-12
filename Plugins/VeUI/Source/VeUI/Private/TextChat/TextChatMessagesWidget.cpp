// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "TextChat/TextChatMessagesWidget.h"

#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"
#include "Components/ScrollBox.h"
#include "Blueprint/WidgetTree.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/ScrollBoxSlot.h"
#include "Components/VerticalBoxSlot.h"
#include "RpcWebSocketsTextChatChannel.h"


UTextChatMessagesWidget::UTextChatMessagesWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {

	MessagePadding = {0.f, 4.f, 0.f, 4.f};
}

void UTextChatMessagesWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();

	if (IsValid(ScrollBoxWidget)) {
		ScrollBoxWidget->OnUserScrolled.AddDynamic(this, &UTextChatMessagesWidget::ScrollBox_OnUserScrolledCallback);
	}
}

void UTextChatMessagesWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime) {
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bAutoScroll && ScrollBoxWidget->GetScrollOffset() != ScrollBoxWidget->GetScrollOffsetOfEnd()) {
		ScrollBoxWidget->ScrollToEnd();
	}
}

void UTextChatMessagesWidget::SetChannel(const TSharedPtr<FRpcWebSocketsTextChatChannel>& InChannel) {
	if (!IsValid(ScrollBoxWidget)) {
		return;
	}

	if (Channel) {
		Channel->OnMessageReceived.RemoveAll(this);
	}

	if (!InChannel) {
		Channel = nullptr;
		ScrollBoxWidget->ClearChildren();
		return;
	}
	
	Channel = InChannel;

	Channel->OnMessageReceived.AddUObject(this, &UTextChatMessagesWidget::Channel_OnMessageReceivedCallback);

	ScrollBoxWidget->ClearChildren();
	for (auto& Messsage : Channel->Messages) {
		AddMessage(Messsage);
	}
	
	SetAutoScroll(true);
}

void UTextChatMessagesWidget::SetAutoScroll(bool Value) {
	bAutoScroll = Value;
}

void UTextChatMessagesWidget::AddMessage(const TSharedPtr<FRpcWebSocketsTextChatMessage>& Message) {
	if (!Message) {
		return;
	}

	auto* MessageWidget = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	if (!MessageWidget) {
		return;
	}

	MessageWidget->SetFont(MessageFont);
	MessageWidget->SetAutoWrapText(true);
	MessageWidget->SetText(FText::FromString(Message->ToString()));
	MessageWidget->SetColorAndOpacity(FSlateColor(Message->GetFontColor()));

	if (auto* PanelSlot = Cast<UScrollBoxSlot>(ScrollBoxWidget->AddChild(MessageWidget))) {
		PanelSlot->SetPadding(MessagePadding);
		PanelSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill); 
		PanelSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill); 
	}
}

void UTextChatMessagesWidget::Channel_OnMessageReceivedCallback(const TSharedPtr<FRpcWebSocketsTextChatChannel>& InChannel, const TSharedPtr<FRpcWebSocketsTextChatMessage>& InMessage) {
	AddMessage(InMessage);
}

void UTextChatMessagesWidget::ScrollBox_OnUserScrolledCallback(float CurrentOffset) {
	bAutoScroll = CurrentOffset == ScrollBoxWidget->GetScrollOffsetOfEnd();
}
