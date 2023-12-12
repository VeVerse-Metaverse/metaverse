// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "Subsystems/UIKeySubsystem.h"

const FName FUIKeySubsystem::Name = FName("UIKeySubsystem");

void FUIKeySubsystem::Initialize() {
}

void FUIKeySubsystem::Shutdown() {
}

void FUIKeySubsystem::GetKeyAndCharCodes(const FKey& Key, bool& bHasKeyCode, uint32& KeyCode, bool& bHasCharCode, uint32& CharCode) {
	const uint32* KeyCodePtr;
	const uint32* CharCodePtr;
	FInputKeyManager::Get().GetCodesFromKey(Key, KeyCodePtr, CharCodePtr);

	bHasKeyCode = KeyCodePtr ? true : false;
	bHasCharCode = CharCodePtr ? true : false;

	KeyCode = KeyCodePtr ? *KeyCodePtr : 0;
	CharCode = CharCodePtr ? *CharCodePtr : 0;

	// These special keys are not handled by the platform layer, and while not printable
	// have character mappings that several widgets look for, since the hardware sends them.
	if (CharCodePtr == nullptr) {
		if (Key == EKeys::Tab) {
			CharCode = '\t';
			bHasCharCode = true;
		} else if (Key == EKeys::BackSpace) {
			CharCode = '\b';
			bHasCharCode = true;
		} else if (Key == EKeys::Enter) {
			CharCode = '\n';
			bHasCharCode = true;
		}
	}
}

void FUIKeySubsystem::GetCharCode(const FKey& Key, bool& bHasCharCode, uint32& CharCode) {
	const uint32* KeyCodePtr;
	const uint32* CharCodePtr;
	FInputKeyManager::Get().GetCodesFromKey(Key, KeyCodePtr, CharCodePtr);

	bHasCharCode = CharCodePtr ? true : false;
	CharCode = CharCodePtr ? *CharCodePtr : 0;

	// These special keys are not handled by the platform layer, and while not printable
	// have character mappings that several widgets look for, since the hardware sends them.
	if (CharCodePtr == nullptr) {
		if (Key == EKeys::Tab) {
			CharCode = '\t';
			bHasCharCode = true;
		} else if (Key == EKeys::BackSpace) {
			CharCode = '\b';
			bHasCharCode = true;
		} else if (Key == EKeys::Enter) {
			CharCode = '\n';
			bHasCharCode = true;
		}
	}
}

TCHAR FUIKeySubsystem::ToLower(const TCHAR InChar) {
	if (InChar == TEXT('`') || InChar == TEXT('~')) {
		return '`';
	}

	if (InChar == TEXT('1') || InChar == TEXT('!')) {
		return '1';
	}

	if (InChar == TEXT('2') || InChar == TEXT('@')) {
		return '2';
	}

	if (InChar == TEXT('3') || InChar == TEXT('#')) {
		return '3';
	}

	if (InChar == TEXT('4') || InChar == TEXT('$')) {
		return '4';
	}

	if (InChar == TEXT('5') || InChar == TEXT('%')) {
		return '5';
	}

	if (InChar == TEXT('6') || InChar == TEXT('^')) {
		return '6';
	}

	if (InChar == TEXT('7') || InChar == TEXT('&')) {
		return '7';
	}

	if (InChar == TEXT('8') || InChar == TEXT('*')) {
		return '8';
	}
	
	if (InChar == TEXT('9') || InChar == TEXT('(')) {
		return '9';
	}
	
	if (InChar == TEXT('0') || InChar == TEXT(')')) {
		return '0';
	}
	
	if (InChar == TEXT('-') || InChar == TEXT('_')) {
		return '-';
	}

	if (InChar == TEXT('=') || InChar == TEXT('+')) {
		return '=';
	}

	if (InChar == TEXT('{') || InChar == TEXT('[')) {
		return '[';
	}

	if (InChar == TEXT(']') || InChar == TEXT('}')) {
		return ']';
	}
	
	if (InChar == TEXT('\\') || InChar == TEXT('|')) {
		return '\\';
	}
	
	if (InChar == TEXT('\'') || InChar == TEXT('\"')) {
		return '\'';
	}

	if (InChar == TEXT(';') || InChar == TEXT(':')) {
		return ';';
	}

	if (InChar == TEXT(',') || InChar == TEXT('<')) {
		return ',';
	}

	if (InChar == TEXT('.') || InChar == TEXT('>')) {
		return '.';
	}
	
	if (InChar == TEXT('/') || InChar == TEXT('?')) {
		return '/';
	}

	return TChar<TCHAR>::ToLower(InChar);
}

TCHAR FUIKeySubsystem::ToUpper(const TCHAR InChar) {
	if (InChar == TEXT('`') || InChar == TEXT('~')) {
		return '~';
	}

	if (InChar == TEXT('1') || InChar == TEXT('!')) {
		return '!';
	}

	if (InChar == TEXT('2') || InChar == TEXT('@')) {
		return '@';
	}

	if (InChar == TEXT('3') || InChar == TEXT('#')) {
		return '#';
	}

	if (InChar == TEXT('4') || InChar == TEXT('$')) {
		return '$';
	}

	if (InChar == TEXT('5') || InChar == TEXT('%')) {
		return '%';
	}

	if (InChar == TEXT('6') || InChar == TEXT('^')) {
		return '^';
	}

	if (InChar == TEXT('7') || InChar == TEXT('&')) {
		return '&';
	}

	if (InChar == TEXT('8') || InChar == TEXT('*')) {
		return '*';
	}
	
	if (InChar == TEXT('9') || InChar == TEXT('(')) {
		return '(';
	}
	
	if (InChar == TEXT('0') || InChar == TEXT(')')) {
		return ')';
	}
	
	if (InChar == TEXT('-') || InChar == TEXT('_')) {
		return '_';
	}

	if (InChar == TEXT('=') || InChar == TEXT('+')) {
		return '+';
	}

	if (InChar == TEXT('{') || InChar == TEXT('[')) {
		return '{';
	}

	if (InChar == TEXT(']') || InChar == TEXT('}')) {
		return '}';
	}
	
	if (InChar == TEXT('\\') || InChar == TEXT('|')) {
		return '|';
	}
	
	if (InChar == TEXT('\'') || InChar == TEXT('\"')) {
		return '\"';
	}

	if (InChar == TEXT(';') || InChar == TEXT(':')) {
		return ':';
	}

	if (InChar == TEXT(',') || InChar == TEXT('<')) {
		return '<';
	}

	if (InChar == TEXT('.') || InChar == TEXT('>')) {
		return '>';
	}
	
	if (InChar == TEXT('/') || InChar == TEXT('?')) {
		return '?';
	}

	return TChar<TCHAR>::ToUpper(InChar);
}
