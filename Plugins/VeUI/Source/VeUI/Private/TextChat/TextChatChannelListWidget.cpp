// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "TextChat/TextChatChannelListWidget.h"

#include "RpcWebSocketsTextChatSubsystem.h"
#include "TextChat/TextChatChannelItemWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/ScrollBox.h"
#include "Components/ScrollBoxSlot.h"
#include "VeUI.h"
#include "VeRpc.h"


void UTextChatChannelListWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();

	if (IsValid(ScrollBoxWidget)) {
		ScrollBoxWidget->ClearChildren();
	}
}

void UTextChatChannelListWidget::NativeConstruct() {
	Super::NativeConstruct();

	GET_MODULE_SUBSYSTEM(TextChatSubsystem, Rpc, WebSocketsTextChat);
	if (TextChatSubsystem) {
		TextChatSubsystem->OnChannelListAdded.AddUObject(this, &UTextChatChannelListWidget::TextChatSubsystem_OnChannelListAddedCallback);
		TextChatSubsystem->OnChannelListChanged.AddUObject(this, &UTextChatChannelListWidget::UpdateChannelList);
		TextChatSubsystem->OnChannelSelectChanged.AddUObject(this, &UTextChatChannelListWidget::TextChatSubsystem_OnChannelSelectChangedCallback);
		UpdateChannelList();
	}
}

void UTextChatChannelListWidget::NativeDestruct() {
	Super::NativeDestruct();

	GET_MODULE_SUBSYSTEM(TextChatSubsystem, Rpc, WebSocketsTextChat);
	if (TextChatSubsystem) {
		TextChatSubsystem->OnChannelListAdded.RemoveAll(this);
		TextChatSubsystem->OnChannelListChanged.RemoveAll(this);
		TextChatSubsystem->OnChannelSelectChanged.RemoveAll(this);
	}
}

void UTextChatChannelListWidget::UpdateChannelList() {
	GET_MODULE_SUBSYSTEM(TextChatSubsystem, Rpc, WebSocketsTextChat);
	if (!TextChatSubsystem) {
		return;
	}

	if (IsValid(ScrollBoxWidget)) {
		ScrollBoxWidget->ClearChildren();
	}

	const TArray<TSharedRef<FRpcWebSocketsTextChatChannel>>& Channels = FilterSelected ? TextChatSubsystem->SelectedChannels : TextChatSubsystem->Channels;
	for (const auto& Channel : Channels) {
		if (FilterSelected) {
			UpdateChannelVisible(Channel, true);
		} else {
			auto* ChannelWidget = CreateChannelWidget(Channel);
			const bool Selected = TextChatSubsystem->IsSelectedChannel(Channel);
			UpdateChannelVisible(Channel, Selected, ChannelWidget);
		}
	}
}

UTextChatChannelItemWidget* UTextChatChannelListWidget::CreateChannelWidget(const TSharedRef<FRpcWebSocketsTextChatChannel>& Channel) {
	if (!IsValid(ChannelItemClass) || !IsValid(ScrollBoxWidget)) {
		return nullptr;		
	}

	auto* ChannelItemWidget = WidgetTree->ConstructWidget<UTextChatChannelItemWidget>(ChannelItemClass);
	if (!ChannelItemWidget) {
		return nullptr;
	}

	ChannelItemWidget->GetOnSelectClicked().AddUObject(this, &UTextChatChannelListWidget::ChannelItemWidget_OnSelectClickedCallback);
	ChannelItemWidget->GetOnDeselectClicked().AddUObject(this, &UTextChatChannelListWidget::ChannelItemWidget_OnDeselectClickedCallback);
	ChannelItemWidget->SetChannel(Channel);

	auto* PanelSlot = Cast<UScrollBoxSlot>(ScrollBoxWidget->AddChild(ChannelItemWidget));
	
	if (PanelSlot) {
		// PanelSlot->SetPadding({0.f, 4.f, 0.f, 4.f});
		PanelSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Left); 
		PanelSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
	}

	return ChannelItemWidget;
}

void UTextChatChannelListWidget::RemoveChannelWidget(const TSharedPtr<FRpcWebSocketsTextChatChannel>& Channel) {
	if (UTextChatChannelItemWidget* ChannelItemWidget = FindChannelItemWidget(Channel)) {
		ChannelItemWidget->RemoveFromParent();
	}
}

void UTextChatChannelListWidget::UpdateChannelVisible(const TSharedPtr<FRpcWebSocketsTextChatChannel>& Channel, bool Selected, UTextChatChannelItemWidget* ChannelItemWidget) {

	// Selected chanels panel (Create/Remove)
	if (FilterSelected) {
		if (Selected) {
			if (Channel) {
				CreateChannelWidget(Channel.ToSharedRef());
			}
		} else {
			RemoveChannelWidget(Channel);
		}
	}
	
	// Deselected channels panel (Show/Hide)
	else {
		if (!ChannelItemWidget) {
			ChannelItemWidget = FindChannelItemWidget(Channel);
		}	
		if (ChannelItemWidget) {
			if (Selected) {
				ChannelItemWidget->SetVisibility(ESlateVisibility::Collapsed);
			} else {
				ChannelItemWidget->SetVisibility(ESlateVisibility::Visible);
			}
		}
	}
}

UTextChatChannelItemWidget* UTextChatChannelListWidget::FindChannelItemWidget(const TSharedPtr<FRpcWebSocketsTextChatChannel>& Channel) {
	if (IsValid(ScrollBoxWidget)) {
		for (auto* Widget : ScrollBoxWidget->GetAllChildren()) {
			auto* ChannelItemWidget = Cast<UTextChatChannelItemWidget>(Widget);
			if (ChannelItemWidget && ChannelItemWidget->GetChannel() == Channel) {
				return ChannelItemWidget;
			}
		}
	}
	return nullptr;
}

void UTextChatChannelListWidget::TextChatSubsystem_OnChannelListAddedCallback(const TSharedPtr<FRpcWebSocketsTextChatChannel>& Channel) {
	if (!FilterSelected) {
		if (Channel) {
			auto* ChannelWidget = CreateChannelWidget(Channel.ToSharedRef());
			UpdateChannelVisible(Channel, false, ChannelWidget);
		}
	}
}

void UTextChatChannelListWidget::TextChatSubsystem_OnChannelSelectChangedCallback(const TSharedPtr<FRpcWebSocketsTextChatChannel>& Channel, bool Selected, bool CanDeselect) {
	UpdateChannelVisible(Channel, Selected);
}

void UTextChatChannelListWidget::ChannelItemWidget_OnSelectClickedCallback(UTextChatChannelItemWidget* ChannelItemWidget) {
	OnChannelSelectClicked.Broadcast(ChannelItemWidget);
}

void UTextChatChannelListWidget::ChannelItemWidget_OnDeselectClickedCallback(UTextChatChannelItemWidget* ChannelItemWidget) {
	OnChannelDeselectClicked.Broadcast(ChannelItemWidget);
}
