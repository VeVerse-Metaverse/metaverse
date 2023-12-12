// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "TextChat/TextChatRootWidget.h"

#include "Components/WidgetSwitcher.h"
#include "TextChat/TextChatInfoWidget.h"
#include "TextChat/TextChatWidget.h"
#include "VeShared.h"
#include "VeRpc.h"
#include "RpcWebSocketsTextChatSubsystem.h"


void UTextChatRootWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();

	NativeOnShow();

	if (TextChatWidget) {
		TextChatWidget->GetOnFocusReceived().AddUObject(this, &UTextChatRootWidget::TextChatWidget_OnFocusReceivedCallback);
		TextChatWidget->GetOnFocusLost().AddUObject(this, &UTextChatRootWidget::TextChatWidget_OnFocusLostCallback);
		TextChatWidget->GetOnClose().AddUObject(this, &UTextChatRootWidget::TextChatWidget_OnCloseCallback);
	}

	DoHideFullMode();
}

void UTextChatRootWidget::NativeConstruct() {
	Super::NativeConstruct();

	GET_MODULE_SUBSYSTEM(RpcTextChatSubsystem, Rpc, WebSocketsTextChat);
	if (RpcTextChatSubsystem) {
		RpcTextChatSubsystem->OnShowFullMode.AddUObject(this, &UTextChatRootWidget::TextChatSubsystem_OnShowFullModeCallback);
		RpcTextChatSubsystem->OnHideFullMode.AddUObject(this, &UTextChatRootWidget::TextChatSubsystem_OnHideFullModeCallback);
	}
}

void UTextChatRootWidget::NativeDestruct() {
	Super::NativeDestruct();

	GET_MODULE_SUBSYSTEM(RpcTextChatSubsystem, Rpc, WebSocketsTextChat);
	if (RpcTextChatSubsystem) {
		RpcTextChatSubsystem->OnShowFullMode.RemoveAll(this);
		RpcTextChatSubsystem->OnHideFullMode.RemoveAll(this);
	}
}

void UTextChatRootWidget::NativeOnUpdateInputMode() {
	SetInputModeUiOnly(GetWidgetToFocus());
}

void UTextChatRootWidget::DoShowFullMode() {
	if (TextChatInfoWidget) {
		TextChatInfoWidget->SetIsEnabled(false);
	}
	if (IsValid(TextChatWidget)) {
		TextChatWidget->SetIsEnabled(true);
		SwitcherWidget->SetActiveWidget(TextChatWidget);
		TextChatWidget->Reset();
	}
}

void UTextChatRootWidget::DoHideFullMode() {
	if (IsValid(TextChatWidget)) {
		TextChatWidget->SetIsEnabled(false);
	}
	if (IsValid(TextChatInfoWidget)) {
		TextChatInfoWidget->SetIsEnabled(true);
		SwitcherWidget->SetActiveWidget(TextChatInfoWidget);
	}
}

UWidget* UTextChatRootWidget::GetWidgetToFocus() const {
	if (IsValid(TextChatWidget)) {
		return TextChatWidget->GetWidgetToFocus();
	}
	return nullptr;
}

void UTextChatRootWidget::TextChatSubsystem_OnShowFullModeCallback() {
	if (auto* World = GetWorld()) {
		World->GetTimerManager().ClearTimer(HideTimerHandle);
	}
	DoShowFullMode();
}

void UTextChatRootWidget::TextChatSubsystem_OnHideFullModeCallback(float Delay) {
	if (auto* World = GetWorld()) {
		World->GetTimerManager().ClearTimer(HideTimerHandle);
	}

	if (Delay > 0.f) {
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindWeakLambda(this, [this]() {
			DoHideFullMode();
		});

		if (auto* World = GetWorld()) {
			World->GetTimerManager().SetTimer(HideTimerHandle, TimerDelegate, Delay, false);
		}
	} else {
		DoHideFullMode();
	}
}

void UTextChatRootWidget::TextChatWidget_OnFocusReceivedCallback() {
	if (auto* World = GetWorld()) {
		World->GetTimerManager().ClearTimer(HideTimerHandle);
	}
}

void UTextChatRootWidget::TextChatWidget_OnFocusLostCallback() {
	GET_MODULE_SUBSYSTEM(RpcTextChatSubsystem, Rpc, WebSocketsTextChat);
	if (RpcTextChatSubsystem) {
		RpcTextChatSubsystem->HideFullMode(UnfocusedHideTime);
	}
}

void UTextChatRootWidget::TextChatWidget_OnCloseCallback() {
	GET_MODULE_SUBSYSTEM(RpcTextChatSubsystem, Rpc, WebSocketsTextChat);
	if (RpcTextChatSubsystem) {
		RpcTextChatSubsystem->HideFullMode(0.0f);
	}
}
