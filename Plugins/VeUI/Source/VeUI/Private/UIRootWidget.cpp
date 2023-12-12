// Authors: Khusan T.Yadgarov, Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UIRootWidget.h"

#include "VePlayerControllerBase.h"
#include "RpcWebSocketsTextChatSubsystem.h"
#include "VR/UIObjectDetailWidget.h"
#include "Subsystems/UIDialogSubsystem.h"
#include "UIAuthRootWidget.h"
#include "VeApi2.h"
#include "Api2AuthSubsystem.h"
#include "VeUI.h"
#include "UIMainMenuRootWidget.h"
#include "GameFramework/InputSettings.h"
#include "TextChat/TextChatRootWidget.h"
#include "VeRpc.h"
#include "Debug/UIDebuggerRootWidget.h"
#include "Editor/UIEditorRootWidget.h"
#include "Inspector/UIInspectorRootWidget.h"


UUIRootWidget::UUIRootWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {
	bIsFocusable = true;

	bAuthenticationVisible = false;
	bMenuVisible = false;
	bTextChatFullMode = false;
}

void UUIRootWidget::NativeOnInitialized() {
	Super::NativeOnInitialized();

	OnHideMenu.BindUObject(this, &UUIRootWidget::HideMenu);
	
	// Set EscapeActionList
	{
		const UInputSettings* InputSettings = UInputSettings::StaticClass()->GetDefaultObject<UInputSettings>();
		for (const FInputActionKeyMapping& ActionMapping : InputSettings->GetActionMappings()) {
			if (ActionMapping.ActionName == EscapeInputAction) {
				FInputActionKeyMapping Item(ActionMapping);
				Item.ActionName = NAME_None;
				EscapeActionMappings.Add(Item);
			}
		}
	}

	if (EditorRootWidget) {
		EditorRootWidget->OnClose.AddUObject(this, &UUIRootWidget::OnClose);
	}

	if (InspectorRootWidget) {
		InspectorRootWidget->OnClose.AddUObject(this, &UUIRootWidget::OnClose);
	}

	if (DebuggerRootWidget) {
		DebuggerRootWidget->OnClose.AddUObject(this, &UUIRootWidget::OnClose);
	}

	GET_MODULE_SUBSYSTEM(DialogSubsystem, UI, Dialog);
	if (DialogSubsystem) {
		DialogSubsystem->SetRootWidget(this);
	}
}

void UUIRootWidget::NativeConstruct() {
	Super::NativeConstruct();

	if (MainMenuRootWidget) {
		MainMenuRootWidget->SetVisibility(ESlateVisibility::Hidden);
	}

	GET_MODULE_SUBSYSTEM(AuthSubsystem, Api2, Auth);
	if (AuthSubsystem) {

		auto OnChangeAuthorizationStateCallback = [=](EAuthorizationState AuthorizationState) {
			if (AuthSubsystem->IsAuthorized()) {
				HideAuthentication();
				HideBackground();
				return;
			}

			if (AuthorizationState == EAuthorizationState::Unknown) {
				HideAuthentication();
				ShowBackground();
				return;
			}

			ShowAuthentication();
		};

		AuthSubsystem->GetOnChangeAuthorizationState().AddWeakLambda(this, OnChangeAuthorizationStateCallback);
		OnChangeAuthorizationStateCallback(AuthSubsystem->GetAuthorizationState());
	}

	GET_MODULE_SUBSYSTEM(RpcTextChatSubsystem, Rpc, WebSocketsTextChat);
	if (RpcTextChatSubsystem) {
		RpcTextChatSubsystem->OnShowFullMode.AddUObject(this, &UUIRootWidget::TextChatSubsystem_OnShowFullMode);
		RpcTextChatSubsystem->OnHideFullMode.AddUObject(this, &UUIRootWidget::TextChatSubsystem_OnHideFullMode);
	}
}

void UUIRootWidget::NativeDestruct() {
	Super::NativeDestruct();

	GET_MODULE_SUBSYSTEM(AuthSubsystem, Api2, Auth);
	if (AuthSubsystem) {
		AuthSubsystem->GetOnChangeAuthorizationState().RemoveAll(this);
	}

	GET_MODULE_SUBSYSTEM(RpcTextChatSubsystem, Rpc, WebSocketsTextChat);
	if (RpcTextChatSubsystem) {
		RpcTextChatSubsystem->OnShowFullMode.RemoveAll(this);
		RpcTextChatSubsystem->OnHideFullMode.RemoveAll(this);
	}
}

UUIDialogWidget* UUIRootWidget::ShowDialog(const FUIDialogData& InDialogData, const FUIDialogButtonClicked& InOnButtonClicked) const {
	if (DialogRootWidget) {
		return DialogRootWidget->ShowDialog(InDialogData, InOnButtonClicked);
	}

	return nullptr;
}

void UUIRootWidget::ShowAuthentication() {
	if (bAuthenticationVisible) {
		return;
	}

	HideMenu();
	HideEditor();

	if (AuthRootWidget) {
		AuthRootWidget->Show();
	}

	ShowBackground();

	bAuthenticationVisible = true;
	UpdateInputMode();
}

void UUIRootWidget::HideAuthentication() {
	if (!bAuthenticationVisible) {
		return;
	}

	if (AuthRootWidget) {
		AuthRootWidget->Hide();
	}

	HideBackground();

	bAuthenticationVisible = false;
	UpdateInputMode();
}

void UUIRootWidget::ShowMenu_Implementation() {
	if (bAuthenticationVisible || bMenuVisible) {
		return;
	}

	if (MainMenuRootWidget) {
		MainMenuRootWidget->Show();
	}

	ShowBackground();

	bMenuVisible = true;
	UpdateInputMode();
}

void UUIRootWidget::HideMenu_Implementation() {
	if (bAuthenticationVisible || !bMenuVisible) {
		return;
	}

	if (MainMenuRootWidget) {
		MainMenuRootWidget->Hide();
	}

	HideBackground();

	bMenuVisible = false;
	UpdateInputMode();

	SetPlayerInputMode(EAppMode::Game);
}

void UUIRootWidget::ShowMenuWorldDetail(const FGuid& InWorldId) {
	if (MainMenuRootWidget) {
		ShowMenu();
		if (bMenuVisible) {
			MainMenuRootWidget->ShowMenuWorldDetail(InWorldId);
		}
	}
}

void UUIRootWidget::ShowEditor() {
	if (bAuthenticationVisible || bMenuVisible || bInspectorVisible || bEditorVisible || bDebuggerVisible) {
		return;
	}

	HideTextChat();

	if (EditorRootWidget) {
		EditorRootWidget->Show();
	}

	bEditorVisible = true;
	UpdateInputMode();
}

void UUIRootWidget::HideEditor() {
	if (bAuthenticationVisible || bMenuVisible || bInspectorVisible || !bEditorVisible || bDebuggerVisible) {
		return;
	}

	if (EditorRootWidget) {
		EditorRootWidget->Hide();
	}

	bEditorVisible = false;
	UpdateInputMode();

	ShowTextChat();
}

void UUIRootWidget::ShowInspector() {
	if (bAuthenticationVisible || bMenuVisible || bEditorVisible || bInspectorVisible || bDebuggerVisible) {
		return;
	}

	if (InspectorRootWidget) {
		InspectorRootWidget->Show();
	}

	bInspectorVisible = true;
	UpdateInputMode();
}

void UUIRootWidget::HideInspector() {
	if (bAuthenticationVisible || bMenuVisible || bEditorVisible || !bInspectorVisible || bDebuggerVisible) {
		return;
	}

	if (InspectorRootWidget) {
		InspectorRootWidget->Hide();
	}

	bInspectorVisible = false;
	UpdateInputMode();
}

void UUIRootWidget::ShowDebugger() {
	if (bAuthenticationVisible || bMenuVisible || bEditorVisible || bInspectorVisible || bDebuggerVisible) {
		return;
	}

	if (DebuggerRootWidget) {
		DebuggerRootWidget->Show();
	}

	bDebuggerVisible = true;
	UpdateInputMode();
}

void UUIRootWidget::HideDebugger() {
	if (bAuthenticationVisible || bMenuVisible || bEditorVisible || bInspectorVisible || !bDebuggerVisible) {
		return;
	}

	if (DebuggerRootWidget) {
		DebuggerRootWidget->Hide();
	}

	bDebuggerVisible = false;
	UpdateInputMode();
}

void UUIRootWidget::ShowTextChat() {
	if (bAuthenticationVisible || bMenuVisible || bEditorVisible || bInspectorVisible || bTextChatVisible || bDebuggerVisible) {
		return;
	}

	if (TextChatRootWidget) {
		TextChatRootWidget->Show();
	}

	bTextChatVisible = true;
	UpdateInputMode();
}

void UUIRootWidget::HideTextChat() {
	if (bAuthenticationVisible || bMenuVisible || bEditorVisible || bInspectorVisible || !bTextChatVisible || bDebuggerVisible) {
		return;
	}

	if (TextChatRootWidget) {
		TextChatRootWidget->Hide();
	}

	bTextChatVisible = false;
	UpdateInputMode();
}

void UUIRootWidget::ToggleTextChatFullMode() {
	if (bMenuVisible) {
		HideTextChatFullMode();
	} else {
		ShowTextChatFullMode();
	}
}

void UUIRootWidget::ShowTextChatFullMode() {
	if (bAuthenticationVisible || bMenuVisible || bEditorVisible || bInspectorVisible || !bTextChatVisible || bTextChatFullMode || bDebuggerVisible) {
		return;
	}

	GET_MODULE_SUBSYSTEM(RpcTextChatSubsystem, Rpc, WebSocketsTextChat);
	if (RpcTextChatSubsystem) {
		RpcTextChatSubsystem->ShowFullMode();
	}
}

void UUIRootWidget::TextChatSubsystem_OnShowFullMode() {
	// SetInputModeUiOnly(TextChatRootWidget->GetWidgetToFocus());
	bTextChatFullMode = true;
	UpdateInputMode();
}

void UUIRootWidget::HideTextChatFullMode() {
	if (bAuthenticationVisible || bMenuVisible || bEditorVisible || bInspectorVisible || !bTextChatVisible || !bTextChatFullMode || bDebuggerVisible) {
		return;
	}

	GET_MODULE_SUBSYSTEM(RpcTextChatSubsystem, Rpc, WebSocketsTextChat);
	if (RpcTextChatSubsystem) {
		RpcTextChatSubsystem->HideFullMode();
	}
}

void UUIRootWidget::ShowBackground() {
	if (BackgroundWidget) {
		BackgroundWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void UUIRootWidget::HideBackground() {
	if (BackgroundWidget) {
		BackgroundWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UUIRootWidget::TextChatSubsystem_OnHideFullMode(float Delay) {
	bTextChatFullMode = false;
	UpdateInputMode();

	SetPlayerInputMode(EAppMode::Game);
}

void UUIRootWidget::UpdateInputMode() {
	if (AuthRootWidget && bAuthenticationVisible) {
		AuthRootWidget->UpdateInputMode();
		return;
	}

	if (MainMenuRootWidget && bMenuVisible) {
		MainMenuRootWidget->UpdateInputMode();
		return;
	}

	if (EditorRootWidget && bEditorVisible) {
		EditorRootWidget->UpdateInputMode();
		return;
	}

	if (InspectorRootWidget && bInspectorVisible) {
		InspectorRootWidget->UpdateInputMode();
		return;
	}

	if (DebuggerRootWidget && bDebuggerVisible) {
		DebuggerRootWidget->UpdateInputMode();
		return;
	}

	if (TextChatRootWidget && bTextChatVisible && bTextChatFullMode) {
		TextChatRootWidget->UpdateInputMode();
		return;
	}

	SetInputModeGame();
}

void UUIRootWidget::SetInputModeGame() {
	auto* PlayerController = GetOwningPlayer();
	if (!IsValid(PlayerController)) {
		return;
	}

	FInputModeGameOnly InputMode;
	PlayerController->SetInputMode(InputMode);
	PlayerController->SetShowMouseCursor(false);

	PlayerController->ResetIgnoreMoveInput();
}

FReply UUIRootWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) {
	FInputActionKeyMapping InActionMapping(
		NAME_None,
		InKeyEvent.GetKey(),
		InKeyEvent.IsShiftDown(),
		InKeyEvent.IsControlDown(),
		InKeyEvent.IsAltDown(),
		InKeyEvent.IsCommandDown()
		);

	for (const FInputActionKeyMapping& ActionMapping : EscapeActionMappings) {
		if (InActionMapping == ActionMapping) {
			Escape();
			return FReply::Handled();
		}
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UUIRootWidget::SetPlayerInputMode(EAppMode InInputMode) {
	auto* BasePlayerController = Cast<AVePlayerControllerBase>(GetOwningPlayer());
	if (BasePlayerController) {
		BasePlayerController->SetAppMode(InInputMode);
	}
}

void UUIRootWidget::Escape() {
	if (bTextChatFullMode) {
		HideTextChatFullMode();
		return;
	}

	// if (bEditorVisible) {
	// 	HideEditor();
	// 	return;
	// }

	if (bMenuVisible) {
		OnHideMenu.ExecuteIfBound();
		return;
	}
}

void UUIRootWidget::OnClose(UUIBaseRootWidget* RootWidget) {
	if (RootWidget == EditorRootWidget) {
		// VeLog(">>> Hide EDITOR");
	}
}
