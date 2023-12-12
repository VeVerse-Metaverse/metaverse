// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#pragma once
#include "VeShared.h"

class UUINavigatableWidget;

class VEUI_API FUINavigationSubsystem final : public IModuleSubsystem {
public:
	const static FName Name;

	virtual void Initialize() override;
	virtual void Shutdown() override;

	/** Return to the main menu. */
	void Home();

	/** Return back by the stack. */
	void Back();

	/** Navigate to arbitrary widget. */
	void Navigate(const FString& Path, const FString& Id = TEXT(""));

	/** Use to register widget to specified path. */
	void RegisterWidget(const FString& Path, UUINavigatableWidget* Widget);

	DECLARE_EVENT_OneParam(FUINavigationSubsystem, FOnNavigate, const FString& /* Path */);
	FOnNavigate OnNavigate;

private:
	TMap<FString, UUINavigatableWidget*> Widgets;
	
	TArray<FString> Stack = {};
};

typedef TSharedPtr<FUINavigationSubsystem> FUINavigationSubsystemPtr;
