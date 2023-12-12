// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "UIWidgetBase.h"

#include "ICallbackInterface.h"
#include "UIKeyboardWidget.generated.h"

class UUIHighlightableKeyWidget;
class UUIKeyWidget;

/**
 * 
 */
UCLASS()
class VEUI_API UUIKeyboardWidget final : public UUIWidgetBase
 {
	GENERATED_BODY()

	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	/**
	 * Represents the Virtual User Index.  Each virtual user should be represented by a different 
	 * index number, this will maintain separate capture and focus states for them.  Each
	 * controller or finger-tip should get a unique PointerIndex.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction", meta=( ClampMin = "0", ExposeOnSpawn = true ))
	int32 VirtualUserIndex = 0;

private:
	virtual void RegisterCallbacks() override;
	virtual void UnregisterCallbacks() override;

	bool CanSendInput() const;
	bool SimulateSystemKeyPress(FKey Key, bool bRepeat) const;
	bool SimulateSystemKeyRelease(FKey Key) const;
	bool SimulateSystemKeyPressAndRelease(FKey Key) const;
	bool ProcessKeyPressAndRelease(FKey Key);

	/** Flag for keys to track the shift key case. */
	bool bShiftPressed = false;
	bool bCapsLockOn = false;

	/** Used to switch case (by clicking shift or capslock). */
	void SetUppercase(const bool bInUppercase) const;

	/** The modifier keys to simulate during key presses. */
	FModifierKeysState ModifierKeys;

	/** Get updated modifier keys state from the system and cache it. */
	void UpdateModifierKeys();

	/**
	 * Represents the virtual user in slate.  When this component is registered, it gets a handle to the 
	 * virtual slate user it will be, so virtual slate user 0, is probably real slate user 8, as that's the first
	 * index by default that virtual users begin - the goal is to never have them overlap with real input
	 * hardware as that will likely conflict with focus states you don't actually want to change - like where
	 * the mouse and keyboard focus input (the viewport), so that things like the player controller receive
	 * standard hardware input.
	 */
	TSharedPtr<FSlateVirtualUserHandle> VirtualUser;

	/** Default key press handler. */
	void OnKeyPressed(const FKey& InKey);
	void UpdateCase() const;

	/** Shift key press handler. */
	void OnShiftKeyPressed(const FKey& InKey);
	
	/** Caps lock key press handler. */
	void OnCapsLockKeyPressed(const FKey& InKey);

#pragma region Functional

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyEscape;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyF1;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyF2;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyF3;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyF4;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyF5;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyF6;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyF7;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyF8;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyF9;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyF10;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyF11;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyF12;

#pragma endregion

#pragma region 1-0

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyTilde;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* Key1;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* Key2;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* Key3;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* Key4;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* Key5;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* Key6;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* Key7;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* Key8;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* Key9;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* Key0;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyDash;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyEquals;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyBackspace;

#pragma endregion

#pragma region Q-P

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyTab;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyQ;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyW;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyE;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyR;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyT;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyY;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyU;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyI;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyO;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyP;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyBracketOpen;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyBracketClose;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyBackslash;

#pragma endregion

#pragma region A-L

	UPROPERTY(meta=(BindWidgetOptional))
	UUIHighlightableKeyWidget* KeyCapsLock;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyA;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyS;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyD;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyF;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyG;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyH;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyJ;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyK;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyL;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeySemicolon;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyQuote;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyEnter;

#pragma endregion

#pragma region Z-M

	UPROPERTY(meta=(BindWidgetOptional))
	UUIHighlightableKeyWidget* KeyShiftLeft;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyZ;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyX;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyC;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyV;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyB;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyN;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyM;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyComma;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyPeriod;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeySlash;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIHighlightableKeyWidget* KeyShiftRight;

#pragma endregion

#pragma region Ctrl-Space

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyControlLeft;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyCommandLeft;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyAltLeft;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeySpace;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyAltRight;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyCommandRight;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyControlRight;

#pragma endregion

#pragma region Right Keyboard

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyNumLock;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyNumDivide;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyNumMultiply;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyNumMinus;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyNumPlus;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyNumEnter;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyNum0;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyNum1;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyNum2;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyNum3;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyNum4;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyNum5;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyNum6;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyNum7;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyNum8;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyNum9;

	UPROPERTY(meta=(BindWidgetOptional))
	UUIKeyWidget* KeyNumPeriod;

#pragma endregion
};
