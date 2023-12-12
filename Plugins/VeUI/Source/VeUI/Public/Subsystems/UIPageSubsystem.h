// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#pragma once
#include "VeShared.h"

class UUIPageManagerWidget;

class VEUI_API FUIPageSubsystem final : public IModuleSubsystem {
public:
	const static FName Name;

	virtual void Initialize() override;
	virtual void Shutdown() override;

	UUIPageManagerWidget* GetPageManagerUI() const;
	void SetPageManagerUI(UUIPageManagerWidget* InPageManager);

#if WITH_EDITOR
	UUIPageManagerWidget* GetPageManagerSDK() const;
	void SetPageManagerSDK(UUIPageManagerWidget* InPageManager);
#endif

private:
	TWeakObjectPtr<UUIPageManagerWidget> PageManagerUI;

#if WITH_EDITOR
	TWeakObjectPtr<UUIPageManagerWidget> PageManagerSDK;
#endif
};

typedef TSharedPtr<FUIPageSubsystem> FUIPageSubsystemPtr;
