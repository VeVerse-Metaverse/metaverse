// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.


#include "UIKeyboardWidget.h"
#include "Subsystems/UIFocusSubsystem.h"
#include "Subsystems/UIKeySubsystem.h"
#include "UIKeyWidget.h"
#include "VeShared.h"
#include "VeUI.h"
#include "Framework/Application/SlateUser.h"

void UUIKeyboardWidget::NativeOnInitialized() {
	bIsFocusable = false;

	Super::NativeOnInitialized();
}

void UUIKeyboardWidget::NativeConstruct() {
	Super::NativeConstruct();

	if (FSlateApplication::IsInitialized()) {
		if (!VirtualUser.IsValid()) {
			VirtualUser = FSlateApplication::Get().FindOrCreateVirtualUser(VirtualUserIndex);
		}
	}

	UpdateModifierKeys();
	SetUppercase(ModifierKeys.AreCapsLocked());

	GET_MODULE_SUBSYSTEM(FocusSubsystem, UI, Focus);
	if (FocusSubsystem) {
		FocusSubsystem->RegisterKeyboard(this);
	}

	RegisterCallbacks();
}

void UUIKeyboardWidget::NativeDestruct() {
	Super::NativeDestruct();

	if (FSlateApplication::IsInitialized()) {
		if (VirtualUser.IsValid()) {
			FSlateApplication::Get().UnregisterUser(VirtualUser->GetUserIndex());
			VirtualUser.Reset();
		}
	}

	GET_MODULE_SUBSYSTEM(FocusSubsystem, UI, Focus);
	if (FocusSubsystem) {
		FocusSubsystem->UnregisterKeyboard(this);
	}

	UnregisterCallbacks();
}

void UUIKeyboardWidget::RegisterCallbacks() {
	Super::RegisterCallbacks();

	if (KeyEscape) {
		KeyEscape->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyF1) {
		KeyF1->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyF2) {
		KeyF2->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyF3) {
		KeyF3->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyF4) {
		KeyF4->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyF5) {
		KeyF5->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyF6) {
		KeyF6->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyF7) {
		KeyF7->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyF8) {
		KeyF8->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyF9) {
		KeyF9->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyF10) {
		KeyF10->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyF11) {
		KeyF11->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyF12) {
		KeyF12->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyTilde) {
		KeyTilde->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (Key1) {
		Key1->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (Key2) {
		Key2->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (Key3) {
		Key3->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (Key4) {
		Key4->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (Key5) {
		Key5->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (Key6) {
		Key6->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (Key7) {
		Key7->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (Key8) {
		Key8->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (Key9) {
		Key9->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (Key0) {
		Key0->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyDash) {
		KeyDash->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyEquals) {
		KeyEquals->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyBackspace) {
		KeyBackspace->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyTab) {
		KeyTab->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyQ) {
		KeyQ->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyW) {
		KeyW->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyE) {
		KeyE->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyR) {
		KeyR->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyT) {
		KeyT->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyY) {
		KeyY->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyU) {
		KeyU->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyI) {
		KeyI->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyO) {
		KeyO->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyP) {
		KeyP->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyBracketOpen) {
		KeyBracketOpen->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyBracketClose) {
		KeyBracketClose->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyBackslash) {
		KeyBackslash->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyCapsLock) {
		KeyCapsLock->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnCapsLockKeyPressed);
	}

	if (KeyA) {
		KeyA->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyS) {
		KeyS->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyD) {
		KeyD->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyF) {
		KeyF->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyG) {
		KeyG->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyH) {
		KeyH->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyJ) {
		KeyJ->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyK) {
		KeyK->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyL) {
		KeyL->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeySemicolon) {
		KeySemicolon->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyQuote) {
		KeyQuote->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyEnter) {
		KeyEnter->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyShiftLeft) {
		KeyShiftLeft->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnShiftKeyPressed);
	}

	if (KeyZ) {
		KeyZ->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyX) {
		KeyX->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyC) {
		KeyC->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyV) {
		KeyV->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyB) {
		KeyB->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyN) {
		KeyN->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyM) {
		KeyM->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyComma) {
		KeyComma->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyPeriod) {
		KeyPeriod->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyShiftRight) {
		KeyShiftRight->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnShiftKeyPressed);
	}

	if (KeyControlLeft) {
		KeyControlLeft->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyCommandLeft) {
		KeyCommandLeft->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyAltLeft) {
		KeyAltLeft->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeySpace) {
		KeySpace->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyAltRight) {
		KeyAltRight->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyCommandRight) {
		KeyCommandRight->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyControlRight) {
		KeyControlRight->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyNum0) {
		KeyNum0->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyNum1) {
		KeyNum1->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyNum2) {
		KeyNum2->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyNum3) {
		KeyNum3->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyNum4) {
		KeyNum4->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyNum5) {
		KeyNum5->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyNum6) {
		KeyNum6->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyNum7) {
		KeyNum7->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyNum8) {
		KeyNum8->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyNum9) {
		KeyNum9->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyNumPeriod) {
		KeyNumPeriod->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyNumEnter) {
		KeyNumEnter->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyNumDivide) {
		KeyNumDivide->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyNumMultiply) {
		KeyNumMultiply->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyNumMinus) {
		KeyNumMinus->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}

	if (KeyNumPlus) {
		KeyNumPlus->GetOnClicked().BindUObject(this, &UUIKeyboardWidget::OnKeyPressed);
	}
}

void UUIKeyboardWidget::UnregisterCallbacks() {
	Super::UnregisterCallbacks();
}

void UUIKeyboardWidget::UpdateModifierKeys() {
	if (FSlateApplication::IsInitialized()) {
		ModifierKeys = FSlateApplication::Get().GetModifierKeys();
	}
}

void UUIKeyboardWidget::OnKeyPressed(const FKey& InKey) {
	if (!ProcessKeyPressAndRelease(InKey)) {
		LogWarningF("failed to send key %s", *InKey.ToString());
	}

	bShiftPressed = false;
	UpdateCase();
}

void UUIKeyboardWidget::UpdateCase() const {
	if (ModifierKeys.AreCapsLocked() || bCapsLockOn) {
		if (ModifierKeys.IsShiftDown() || bShiftPressed) {
			SetUppercase(false);
		} else {
			SetUppercase(true);
		}
	} else {
		if (ModifierKeys.IsShiftDown() || bShiftPressed) {
			SetUppercase(true);
		} else {
			SetUppercase(false);
		}
	}
}

void UUIKeyboardWidget::OnShiftKeyPressed(const FKey& InKey) {
	if (!ProcessKeyPressAndRelease(InKey)) {
		LogWarningF("failed to send key %s", *InKey.ToString());
	}

	UpdateModifierKeys();

	bShiftPressed = !bShiftPressed;

	UpdateCase();
}

void UUIKeyboardWidget::OnCapsLockKeyPressed(const FKey& InKey) {
	if (!ProcessKeyPressAndRelease(InKey)) {
		LogWarningF("failed to send key %s", *InKey.ToString());
	}

	bShiftPressed = false;
	bCapsLockOn = !bCapsLockOn;

	UpdateModifierKeys();

	UpdateCase();
}

void UUIKeyboardWidget::SetUppercase(const bool bInUppercase) const {
	if (KeyTilde) {
		KeyTilde->SetUppercase(bInUppercase);
	}

	if (Key1) {
		Key1->SetUppercase(bInUppercase);
	}

	if (Key2) {
		Key2->SetUppercase(bInUppercase);
	}

	if (Key3) {
		Key3->SetUppercase(bInUppercase);
	}

	if (Key4) {
		Key4->SetUppercase(bInUppercase);
	}

	if (Key5) {
		Key5->SetUppercase(bInUppercase);
	}

	if (Key6) {
		Key6->SetUppercase(bInUppercase);
	}

	if (Key7) {
		Key7->SetUppercase(bInUppercase);
	}

	if (Key8) {
		Key8->SetUppercase(bInUppercase);
	}

	if (Key9) {
		Key9->SetUppercase(bInUppercase);
	}

	if (Key0) {
		Key0->SetUppercase(bInUppercase);
	}

	if (KeyDash) {
		KeyDash->SetUppercase(bInUppercase);
	}

	if (KeyEquals) {
		KeyEquals->SetUppercase(bInUppercase);
	}

	if (KeyQ) {
		KeyQ->SetUppercase(bInUppercase);
	}

	if (KeyW) {
		KeyW->SetUppercase(bInUppercase);
	}

	if (KeyE) {
		KeyE->SetUppercase(bInUppercase);
	}

	if (KeyR) {
		KeyR->SetUppercase(bInUppercase);
	}

	if (KeyT) {
		KeyT->SetUppercase(bInUppercase);
	}

	if (KeyY) {
		KeyY->SetUppercase(bInUppercase);
	}

	if (KeyU) {
		KeyU->SetUppercase(bInUppercase);
	}

	if (KeyI) {
		KeyI->SetUppercase(bInUppercase);
	}

	if (KeyO) {
		KeyO->SetUppercase(bInUppercase);
	}

	if (KeyP) {
		KeyP->SetUppercase(bInUppercase);
	}

	if (KeyBracketOpen) {
		KeyBracketOpen->SetUppercase(bInUppercase);
	}

	if (KeyBracketClose) {
		KeyBracketClose->SetUppercase(bInUppercase);
	}

	if (KeyBackslash) {
		KeyBackslash->SetUppercase(bInUppercase);
	}

	if (KeyA) {
		KeyA->SetUppercase(bInUppercase);
	}

	if (KeyS) {
		KeyS->SetUppercase(bInUppercase);
	}

	if (KeyD) {
		KeyD->SetUppercase(bInUppercase);
	}

	if (KeyF) {
		KeyF->SetUppercase(bInUppercase);
	}

	if (KeyG) {
		KeyG->SetUppercase(bInUppercase);
	}

	if (KeyH) {
		KeyH->SetUppercase(bInUppercase);
	}

	if (KeyJ) {
		KeyJ->SetUppercase(bInUppercase);
	}

	if (KeyK) {
		KeyK->SetUppercase(bInUppercase);
	}

	if (KeyL) {
		KeyL->SetUppercase(bInUppercase);
	}

	if (KeySemicolon) {
		KeySemicolon->SetUppercase(bInUppercase);
	}

	if (KeyQuote) {
		KeyQuote->SetUppercase(bInUppercase);
	}

	if (KeyZ) {
		KeyZ->SetUppercase(bInUppercase);
	}

	if (KeyX) {
		KeyX->SetUppercase(bInUppercase);
	}

	if (KeyC) {
		KeyC->SetUppercase(bInUppercase);
	}

	if (KeyV) {
		KeyV->SetUppercase(bInUppercase);
	}

	if (KeyB) {
		KeyB->SetUppercase(bInUppercase);
	}

	if (KeyN) {
		KeyN->SetUppercase(bInUppercase);
	}

	if (KeyM) {
		KeyM->SetUppercase(bInUppercase);
	}

	if (KeyComma) {
		KeyComma->SetUppercase(bInUppercase);
	}

	if (KeyPeriod) {
		KeyPeriod->SetUppercase(bInUppercase);
	}

	if (KeyShiftLeft && !bCapsLockOn) {
		KeyShiftLeft->SetUppercase(bInUppercase);
	}

	if (KeyShiftRight && !bCapsLockOn) {
		KeyShiftRight->SetUppercase(bInUppercase);
	}

	// ReSharper disable once CppRedundantParentheses
	if ((KeyCapsLock && !bShiftPressed) || bCapsLockOn) {
		KeyCapsLock->SetUppercase(bInUppercase);
	}

	if (KeyF1) {
		KeyF1->SetUppercase(bInUppercase);
	}

	if (KeyF2) {
		KeyF2->SetUppercase(bInUppercase);
	}

	if (KeyF3) {
		KeyF3->SetUppercase(bInUppercase);
	}

	if (KeyF4) {
		KeyF4->SetUppercase(bInUppercase);
	}

	if (KeyF5) {
		KeyF5->SetUppercase(bInUppercase);
	}

	if (KeyF6) {
		KeyF6->SetUppercase(bInUppercase);
	}

	if (KeyF7) {
		KeyF7->SetUppercase(bInUppercase);
	}

	if (KeyF8) {
		KeyF8->SetUppercase(bInUppercase);
	}

	if (KeyF9) {
		KeyF9->SetUppercase(bInUppercase);
	}

	if (KeyF10) {
		KeyF10->SetUppercase(bInUppercase);
	}

	if (KeyF11) {
		KeyF11->SetUppercase(bInUppercase);
	}

	if (KeyF12) {
		KeyF12->SetUppercase(bInUppercase);
	}

	if (KeyNum0) {
		KeyNum0->SetUppercase(bInUppercase);
	}

	if (KeyNum1) {
		KeyNum1->SetUppercase(bInUppercase);
	}

	if (KeyNum2) {
		KeyNum2->SetUppercase(bInUppercase);
	}

	if (KeyNum3) {
		KeyNum3->SetUppercase(bInUppercase);
	}

	if (KeyNum4) {
		KeyNum4->SetUppercase(bInUppercase);
	}

	if (KeyNum5) {
		KeyNum5->SetUppercase(bInUppercase);
	}

	if (KeyNum6) {
		KeyNum6->SetUppercase(bInUppercase);
	}

	if (KeyNum7) {
		KeyNum7->SetUppercase(bInUppercase);
	}

	if (KeyNum8) {
		KeyNum8->SetUppercase(bInUppercase);
	}

	if (KeyNum9) {
		KeyNum9->SetUppercase(bInUppercase);
	}

	if (KeyNumPeriod) {
		KeyNumPeriod->SetUppercase(bInUppercase);
	}
}

bool UUIKeyboardWidget::CanSendInput() const {
	return FSlateApplication::IsInitialized() && VirtualUser.IsValid();
}

bool UUIKeyboardWidget::SimulateSystemKeyPress(const FKey Key, const bool bRepeat) const {
	if (!CanSendInput()) {
		return false;
	}

	bool bHasKeyCode, bHasCharCode;
	uint32 KeyCode, CharCode;
	FUIKeySubsystem::GetKeyAndCharCodes(Key, bHasKeyCode, KeyCode, bHasCharCode, CharCode);

	const FKeyEvent KeyEvent(Key, ModifierKeys, VirtualUser->GetUserIndex(), bRepeat, KeyCode, CharCode);
	const bool DownResult = FSlateApplication::Get().ProcessKeyDownEvent(KeyEvent);

	if (bHasCharCode) {
		const FCharacterEvent CharacterEvent(CharCode, ModifierKeys, VirtualUser->GetUserIndex(), bRepeat);
		return FSlateApplication::Get().ProcessKeyCharEvent(CharacterEvent);
	}

	return DownResult;
}

bool UUIKeyboardWidget::SimulateSystemKeyRelease(const FKey Key) const {
	if (!CanSendInput()) {
		return false;
	}

	bool bHasKeyCode, bHasCharCode;
	uint32 KeyCode, CharCode;
	FUIKeySubsystem::GetKeyAndCharCodes(Key, bHasKeyCode, KeyCode, bHasCharCode, CharCode);

	const FKeyEvent KeyEvent(Key, ModifierKeys, VirtualUser->GetUserIndex(), false, KeyCode, CharCode);
	return FSlateApplication::Get().ProcessKeyUpEvent(KeyEvent);
}

bool UUIKeyboardWidget::SimulateSystemKeyPressAndRelease(const FKey Key) const {
	const bool PressResult = SimulateSystemKeyPress(Key, false);
	const bool ReleaseResult = SimulateSystemKeyRelease(Key);

	return PressResult || ReleaseResult;
}

bool UUIKeyboardWidget::ProcessKeyPressAndRelease(const FKey Key) {
	UpdateModifierKeys();

	bool bHasKeyCode, bHasCharCode;
	uint32 KeyCode, CharCode;
	FUIKeySubsystem::GetKeyAndCharCodes(Key, bHasKeyCode, KeyCode, bHasCharCode, CharCode);
	const FKeyEvent KeyEvent(Key, ModifierKeys, VirtualUser->GetUserIndex(), false, KeyCode, CharCode);

	if (!CanSendInput()) {
		FSlateApplication::Get().SetAllUserFocusToGameViewport();
		return false;
	}

	GET_MODULE_SUBSYSTEM(FocusSubsystem, UI, Focus);
	if (FocusSubsystem) {
		if (const auto* FocusedWidget = FocusSubsystem->GetFocusedWidget()) {
			FocusedWidget->SetWidgetFocus();
		}
	}

	const TCHAR OriginalCharKey = CharCode;
	TCHAR CharKey;
	if (ModifierKeys.AreCapsLocked() || bCapsLockOn) {
		if (ModifierKeys.IsShiftDown() || bShiftPressed) {
			CharKey = FUIKeySubsystem::ToLower(OriginalCharKey);
		} else {
			CharKey = FUIKeySubsystem::ToUpper(OriginalCharKey);
		}
	} else {
		if (ModifierKeys.IsShiftDown() || bShiftPressed) {
			CharKey = FUIKeySubsystem::ToUpper(OriginalCharKey);
		} else {
			CharKey = FUIKeySubsystem::ToLower(OriginalCharKey);
		}
	}

	constexpr bool bRepeat = false;
	const FCharacterEvent CharacterEvent(CharKey, ModifierKeys, VirtualUserIndex, bRepeat);

	if (FSlateApplication::Get().ProcessKeyCharEvent(CharacterEvent)) {
		FSlateApplication::Get().SetAllUserFocusToGameViewport();
		return true;
	}

	const bool bResult = SimulateSystemKeyPressAndRelease(Key);
	FSlateApplication::Get().SetAllUserFocusToGameViewport();

	return bResult;
}
