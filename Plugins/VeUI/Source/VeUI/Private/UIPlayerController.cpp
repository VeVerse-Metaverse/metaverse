// Authors: Khusan T.Yadgarov, Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UIPlayerController.h"

#include "HeadMountedDisplayFunctionLibrary.h"
#include "VeHUD.h"
#include "UIRootWidget.h"
#include "VR/VRRootWidget.h"
#include "VeApi.h"
#include "VeEnvironmentSubsystem.h"
#include "Actors/VRHUD.h"
#include "GameFramework/HUD.h"
#include "Kismet/KismetSystemLibrary.h"


void AUIPlayerController::Owner_InitializeHUD(const TSubclassOf<AHUD>& InHUDClass) {
	Owner_UpdateInteractionMode();

	if (InteractionMode == EUIInteractionMode::VR) {
		Owner_SpawnVRHUD(VRHUDClass);
	} else {
		Owner_SpawnUIHUD(InHUDClass);
	}
}

void AUIPlayerController::OpenLevel(const FName LevelName, const bool bAbsolute, const FString Options) {
	if (!GEngine) {
		return;
	}

	auto* World = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull);
	if (World == nullptr) {
		return;
	}

	Input_HideMenu();

	const ETravelType TravelType = (bAbsolute ? TRAVEL_Absolute : TRAVEL_Relative);
	FWorldContext& WorldContext = GEngine->GetWorldContextFromWorldChecked(World);
	FString Cmd = LevelName.ToString();
	if (Options.Len() > 0) {
		Cmd += FString(TEXT("?")) + Options;
	}
	FURL TestURL(&WorldContext.LastURL, *Cmd, TravelType);
	if (TestURL.IsLocalInternal()) {
		// make sure the file exists if we are opening a local file
		if (!GEngine->MakeSureMapNameIsValid(TestURL.Map)) {
			UE_LOG(LogLevel, Warning, TEXT("WARNING: The map '%s' does not exist."), *TestURL.Map);
		}
	}

	GEngine->SetClientTravel(World, *Cmd, TravelType);
}

void AUIPlayerController::SwitchToUIMode() {
	// Show mouse cursor.
	SetShowMouseCursor(true);

	// Switch to the UI input mode.
	FInputModeUIOnly InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetWidgetToFocus(nullptr);
	SetInputMode(InputMode);
}

void AUIPlayerController::SwitchToGameMode() {
	// Hide mouse cursor.
	SetShowMouseCursor(false);

	// Switch to the game input mode.
	const FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
}

void AUIPlayerController::Owner_UpdateInteractionMode() {
	// Check if the HMD connected to spawn the VR pawn.
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayConnected() && UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled()) {
		// Enable VR at the client.
		UKismetSystemLibrary::ExecuteConsoleCommand(this, TEXT("vr.bEnableStereo true"), this);

		// Set the flag at the client.
		InteractionMode = EUIInteractionMode::VR;
	} else {
		if (FVeEnvironmentSubsystem::IsMobilePlatform()) {
			InteractionMode = EUIInteractionMode::Mobile;
		} else {
			InteractionMode = EUIInteractionMode::Desktop;
		}
	}
}

void AUIPlayerController::Owner_SpawnUIHUD(const TSubclassOf<AHUD>& InHUDClass) {
	if (!IsValid(InHUDClass)) {
		LogErrorF("no HUD actor class");
		return;
	}

	if (MyHUD) {
		MyHUD->Destroy();
		MyHUD = nullptr;
	}

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Owner = this;
	SpawnInfo.Instigator = GetInstigator();
	SpawnInfo.ObjectFlags |= RF_Transient; // We never want to save HUDs into a map

	if (auto* const World = GetWorld()) {
		MyHUD = World->SpawnActor<AHUD>(InHUDClass, SpawnInfo);
	} else {
		LogErrorF("no world");
	}
}

void AUIPlayerController::Owner_SpawnVRHUD(const TSubclassOf<AVRHUD>& InHUDClass) {
	if (!IsValid(InHUDClass)) {
		LogErrorF("no VRHUD actor class");
		return;
	}

	if (MyVRHUD) {
		MyVRHUD->Destroy();
		MyVRHUD = nullptr;
	}

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Owner = this;
	SpawnInfo.Instigator = GetInstigator();
	SpawnInfo.ObjectFlags |= RF_Transient; // Never save menus to a map.

	if (auto* const World = GetWorld()) {
		MyVRHUD = World->SpawnActor<AVRHUD>(InHUDClass, SpawnInfo);
	} else {
		LogErrorF("no world");
	}
}

void AUIPlayerController::ClientRPC_InitializeHUD_Implementation(const TSubclassOf<AHUD> NewHUDClass) {
	Owner_InitializeHUD(NewHUDClass);
}

bool AUIPlayerController::Input_ShowMenu() {
	if (InteractionMode == EUIInteractionMode::VR) {
		if (IsValid(MyVRHUD)) {
			auto* RootWidget = MyVRHUD->GetRootWidget();
			if (IsValid(RootWidget)) {
				RootWidget->ShowMenu();
				return true;
			}
		}
		return false;
	}

	auto* HUD = Cast<AVeHUD>(MyHUD);
	if (IsValid(HUD)) {
		return HUD->ShowMenu();
	}
	return false;
}

bool AUIPlayerController::Input_HideMenu() {
	if (InteractionMode == EUIInteractionMode::VR) {
		if (IsValid(MyVRHUD)) {
			auto* RootWidget = MyVRHUD->GetRootWidget();
			if (IsValid(RootWidget)) {
				RootWidget->HideMenu();
				return true;
			}
		}
		return false;
	}

	auto* HUD = Cast<AVeHUD>(MyHUD);
	if (IsValid(HUD)) {
		return HUD->HideMenu();
	}
	return false;
}

void AUIPlayerController::Owner_ShowMenuCurrentWorld(const FGuid& InWorldId) {
	const auto* HUD = Cast<AVeHUD>(MyHUD);
	if (IsValid(HUD)) {
		auto* RootWidget = HUD->GetRootWidget();
		if (IsValid(RootWidget)) {
			SetAppMode(EAppMode::Menu);
			RootWidget->ShowMenuWorldDetail(InWorldId);
		}
	}
}

bool AUIPlayerController::Input_ShowTextChat() {
	const auto* HUD = Cast<AVeHUD>(MyHUD);
	if (IsValid(HUD)) {
		auto* RootWidget = HUD->GetRootWidget();
		if (IsValid(RootWidget)) {
			RootWidget->ShowTextChatFullMode();
			return true;
		}
	}
	return false;
}

bool AUIPlayerController::Input_HideTextChat() {
	const auto* HUD = Cast<AVeHUD>(MyHUD);
	if (IsValid(HUD)) {
		auto* RootWidget = HUD->GetRootWidget();
		if (IsValid(RootWidget)) {
			RootWidget->HideTextChatFullMode();
			return true;
		}
	}
	return false;
}

bool AUIPlayerController::InputTouch(const uint32 Handle, const ETouchType::Type Type, const FVector2D& TouchLocation, const float Force, const FDateTime DeviceTimestamp, const uint32 TouchpadIndex) {
	OnInputTouch.Broadcast(Handle, Type, TouchLocation, Force);
	return Super::InputTouch(Handle, Type, TouchLocation, Force, DeviceTimestamp, TouchpadIndex);
}
