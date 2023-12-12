// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#include "Subsystems/UIDialogSubsystem.h"

#include "Engine/GameEngine.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UIRootWidget.h"

#define LOCTEXT_NAMESPACE "VeUI"


const FName FUIDialogSubsystem::Name = FName("UIDialogSubsystem");

void FUIDialogSubsystem::Initialize() {
}

void FUIDialogSubsystem::Shutdown() {
}

UUIDialogWidget* FUIDialogSubsystem::ShowDialog(const FUIDialogData& DialogData, const FUIDialogButtonClicked& InOnButtonClicked) const {
	if (UIRootWidget.IsValid()) {
		return UIRootWidget->ShowDialog(DialogData, InOnButtonClicked);
	}
	if (VRRootWidget.IsValid()) {
		return VRRootWidget->ShowDialog(DialogData, InOnButtonClicked);
	}
	return nullptr;
}

void FUIDialogSubsystem::RequestExitConfirmation(const float InExitDelay) {
	FUIDialogData DialogData;
	DialogData.Type = EUIDialogType::OkCancel;
	DialogData.HeaderText = LOCTEXT("ConfirmQuitDialogHeader", "Confirm exit");
	DialogData.MessageText = LOCTEXT("ConfirmQuitDialogMessage", "Do you really want to exit?");
	DialogData.FirstButtonText = LOCTEXT("ConfirmQuitDialogButtonOk", "OK");
	DialogData.SecondButtonText = LOCTEXT("ConfirmQuitDialogButtonCancel", "Cancel");
	DialogData.bCloseOnButtonClick = true;

	FUIDialogButtonClicked OnDialogButtonClicked;
	OnDialogButtonClicked.BindLambda([this, InExitDelay](UUIDialogWidget*, const uint8& InButtonIndex) {
		if (InButtonIndex == 0) {
			OnExitConfirmed.Broadcast();
			if (GEngine) {
				//if (const UGameEngine* const GameEngine = Cast<UGameEngine>(GEngine)) {
				if (const UWorld* const World = GEngine->GetCurrentPlayWorld()) {
					FTimerHandle TempTimerHandle;
					World->GetTimerManager().SetTimer(TempTimerHandle, [this, World]() {
						if (APlayerController* WorldPlayerController = World->GetFirstPlayerController()) {
							WorldPlayerController->ConsoleCommand("quit");
						}
					}, InExitDelay, false);
				}
				//}
			}
		}
	});

	ShowDialog(DialogData, OnDialogButtonClicked);
}

void FUIDialogSubsystem::SetRootWidget(UVRRootWidget* InRootWidget) {
	VRRootWidget = InRootWidget;
}

void FUIDialogSubsystem::SetRootWidget(UUIRootWidget* InRootWidget) {
	UIRootWidget = InRootWidget;
}

void FUIDialogSubsystem::SetRootWidget(UUserWidget* InRootWidget) {
	SdkRootWidget = InRootWidget;
}

#undef LOCTEXT_NAMESPACE
