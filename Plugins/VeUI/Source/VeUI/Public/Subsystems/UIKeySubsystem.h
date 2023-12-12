// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#pragma once

#include "VeShared.h"

class UUIWidget;

class VEUI_API FUIKeySubsystem final : public IModuleSubsystem {
public:
	const static FName Name;

	virtual void Initialize() override;
	virtual void Shutdown() override;

	static void GetKeyAndCharCodes(const FKey& Key, bool& bHasKeyCode, uint32& KeyCode, bool& bHasCharCode, uint32& CharCode);
	static void GetCharCode(const FKey& Key, bool& bHasCharCode, uint32& CharCode);
	static TCHAR ToLower(TCHAR InChar);
	static TCHAR ToUpper(TCHAR InChar);
};

typedef TSharedPtr<FUIKeySubsystem> FUIKeySubsystemPtr;
