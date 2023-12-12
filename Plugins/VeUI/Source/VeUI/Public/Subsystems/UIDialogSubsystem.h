// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#pragma once
#include "UIDialogWidget.h"
#include "VR/VRRootWidget.h"
#include "VeShared.h"

class UUIRootWidget;

class VEUI_API FUIDialogSubsystem final : public IModuleSubsystem {
public:
	const static FName Name;

	virtual void Initialize() override;
	virtual void Shutdown() override;

	DECLARE_EVENT(FUIDialogSubsystem, FOnExitConfirmed);

	/** Fired when the user confirms exit. */
	FOnExitConfirmed OnExitConfirmed;

	FOnExitConfirmed& GetOnExitConfirmed() { return OnExitConfirmed; }

	/** Show popup dialog widget. */
	UUIDialogWidget* ShowDialog(const FUIDialogData& DialogData, const FUIDialogButtonClicked& InOnButtonClicked) const;

	/** Request dialog widget to confirm exiting the application. */
	void RequestExitConfirmation(float InExitDelay = 1.0f);

	void SetRootWidget(UVRRootWidget* InRootWidget);
	void SetRootWidget(UUIRootWidget* InRootWidget);
	void SetRootWidget(UUserWidget* InRootWidget);

private:
	/** Root widget for the UI. */
	TWeakObjectPtr<UVRRootWidget> VRRootWidget = nullptr;
	TWeakObjectPtr<UUIRootWidget> UIRootWidget = nullptr;
	TWeakObjectPtr<UUserWidget> SdkRootWidget = nullptr;

};

typedef TSharedPtr<FUIDialogSubsystem> FUIDialogSubsystemPtr;
