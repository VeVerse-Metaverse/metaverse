// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#include "VeGameMode.h"

#include "Api2AnalyticsSubsystem.h"
#include "EngineUtils.h"
#include "VeGameState.h"
#include "VePlayerState.h"
#include "VePlayerController.h"
#include "GameFramework/GameSession.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "VePortal.h"
#include "VeShared.h"
#include "PlaceableLib.h"
#include "VeApi2.h"
#include "Api2AuthSubsystem.h"
#include "Api2PlaceableSubsystem.h"
#include "Api2UserSubsystem.h"
#include "VeGameModule.h"
#include "PlaceableComponent.h"
#include "VeHUD.h"
#include "VeMainCharacter.h"
#include "VeWorldSettings.h"

#define LOCTEXT_NAMESPACE "VeGame"

namespace Options {
	const TCHAR* UserId = TEXT("UserId");
	const TCHAR* MetaverseKey = TEXT("MetaverseKey");
	const TCHAR* Name = TEXT("Name");
	const TCHAR* InteractionMode = TEXT("InteractionMode");
	const TCHAR* SpaceId = TEXT("SpaceId");
}

// Space id of the default (home) map shipped with the client build.
// const static FGuid DefaultSpaceId = FGuid("0b9d138c-9d49-41ae-bd36-5c4baf687238");
#if WITH_EDITOR
const static FGuid DefaultUserId = FGuid("00000000-0000-4000-a000-000000000000");
#endif

AVeGameMode::AVeGameMode() {
	PlayerControllerClass = AVePlayerController::StaticClass();
	PlayerStateClass = AVePlayerState::StaticClass();
	GameStateClass = AVeGameState::StaticClass();
	HUDClass = AVeHUD::StaticClass();

	// static ConstructorHelpers::FClassFinder<AVeMainCharacter> VeMainCharacterClassFinder(TEXT("/VeGame/Shared/Blueprints/BP_VeCharacter"));
	// DefaultPawnClass = VeMainCharacterClassFinder.Class;
	DefaultPawnClass = nullptr;

	// {
	// 	static ConstructorHelpers::FClassFinder<APawn> Pawn(TEXT("/VeGame/Shared/Blueprints/BP_VeCharacter"));
	// 	DefaultPawnClass = Pawn.Class;
	// }
}

void AVeGameMode::PostInitializeComponents() {
	Super::PostInitializeComponents();

	if (const auto* World = GetWorld(); IsValid(World)) {
		for (TActorIterator<AVePortal> It(World); It; ++It) {
			if (auto* VePortal = *It; IsValid(VePortal)) {
				FGuid PortalId = VePortal->GetId();
				if (PortalId.IsValid()) {
					StaticPortals.Add(PortalId, VePortal);
				}
			}
		}

		for (TActorIterator<APlayerStart> It(World); It; ++It) {
			if (auto* PlayerStart = *It; IsValid(PlayerStart)) {
				if (PlayerStart->PlayerStartTag == FName("Default")) {
					DefaultPlayerStart = PlayerStart;
					break;
				}
			}
		}
	}
}

void AVeGameMode::BeginPlay() {
	Super::BeginPlay();

	SpawnPlaceables();
}

void AVeGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);
}

APlayerController* AVeGameMode::Login(UPlayer* NewPlayer, const ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueNetId,
	FString& ErrorMessage) {
	// Login a new player and create a player controller.
	auto* const NewPlayerController = Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueNetId, ErrorMessage);
	if (!IsValid(NewPlayerController)) {
		ErrorMessage = TEXT("failed to create a player controller");
		LogErrorF("[GameMode] %s", *ErrorMessage);
		return nullptr;
	}

	// Check if the player controller is a valid VePlayerController.
	auto* VePlayerController = Cast<AVePlayerController>(NewPlayerController);
	if (!IsValid(VePlayerController)) {
		ErrorMessage = TEXT("invalid player controller class");
		LogErrorF("[GameMode] %s", *ErrorMessage);
		return nullptr;
	}

	VePlayerController->Login_SetOptions(Options, FGuid{Portal});

#pragma region Standalone Game

	auto Test1 = GetNetMode();
	if (GetNetMode() == NM_Standalone) {
		VePlayerController->Owner_UpdateInteractionMode();

		GET_MODULE_SUBSYSTEM(AuthSubsystem, Api2, Auth);
		if (AuthSubsystem) {
			const FGuid UserId = AuthSubsystem->GetUserId();
			if (!Login_RequestUserMetadata(VePlayerController, UserId)) {
				LogErrorF("[GameMode] failed to request user metadata for user: %s", *UserId.ToString());
			}
		}

		// Return player controller to complete login in standalone mode. 
		return VePlayerController;
	}

#pragma endregion

#pragma region Network Game

#pragma region User Interaction Mode Option

	// Connection string must include user id.
	const FString UserInteractionModeOption = UGameplayStatics::ParseOption(Options, Options::InteractionMode);
	if (UserInteractionModeOption == TEXT("Desktop") || UserInteractionModeOption.IsEmpty()) {
		VePlayerController->Login_SetInteractionMode(EUIInteractionMode::Desktop);
	} else if (UserInteractionModeOption == TEXT("VR")) {
		VePlayerController->Login_SetInteractionMode(EUIInteractionMode::VR);
	} else if (UserInteractionModeOption == TEXT("Mobile")) {
		VePlayerController->Login_SetInteractionMode(EUIInteractionMode::Mobile);
	} else {
		NewPlayerController->Destroy();
		ErrorMessage = TEXT("invalid interaction mode option");
		LogErrorF("[GameMode] %s", *ErrorMessage);
		return nullptr;
	}

#pragma endregion

#pragma region User Id Option

	// Connection string must include user id.
	const FString UserIdOption = UGameplayStatics::ParseOption(Options, Options::UserId);

#if UE_EDITOR

	FGuid UserId;
	if (UserIdOption.IsEmpty() || !FGuid::Parse(UserIdOption, UserId)) {
		if (GIsPlayInEditorWorld) {
			// Fallback to the authenticated user ID for PIE builds to allow testing server-client.
			GET_MODULE_SUBSYSTEM(AuthSubsystem, Api2, Auth);
			if (AuthSubsystem) {
				UserId = AuthSubsystem->GetUserId();
			}
		}

		if (!UserId.IsValid()) {
			// Fallback to the default user ID for SDK builds to allow testing without logging in.
			UserId = DefaultUserId;
		}
	}

#else

	if (UserIdOption.IsEmpty()) {
		NewPlayerController->Destroy();
		ErrorMessage = TEXT("no user id option");
		LogErrorF("[GameMode] %s", *ErrorMessage);
		return nullptr;
	}

	FGuid UserId;
	if (!UserIdOption.IsEmpty()) {
		FGuid::Parse(UserIdOption, UserId);
	}
	
#endif

	if (!UserId.IsValid()) {
		NewPlayerController->Destroy();
		ErrorMessage = TEXT("invalid user id");
		LogErrorF("[GameMode] %s", *ErrorMessage);
		return nullptr;
	}

	if (!Login_RequestUserMetadata(VePlayerController, UserId)) {
		NewPlayerController->Destroy();
		ErrorMessage = TEXT("failed to request user metadata");
		LogErrorF("[GameMode] %s", *ErrorMessage);
		return nullptr;
	}

#pragma endregion

#pragma endregion

	LogF("[GameMode] Player controller created. id={%s}; name={%s}", *UserId.ToString(), *UGameplayStatics::ParseOption(Options, Options::Name));

	return VePlayerController;
}

void AVeGameMode::Logout(AController* Exiting) {
	Super::Logout(Exiting);

	if (AVePlayerController* const VePlayerController = Cast<AVePlayerController>(Exiting)) {
		VePlayerController->Authority_OnLogout();
	}

	if (auto* PlayerController = Cast<APlayerController>(Exiting)) {
		if (const auto* UserIdPtr = ActivePlayerControllers.Find(PlayerController)) {
			// Unregister player
			NativeOnUserLogout(PlayerController, *UserIdPtr);
			Analytics_ReportUserLeavingWorld(*UserIdPtr);
			ActivePlayerControllers.Remove(PlayerController);
		}
	}
}

UClass* AVeGameMode::GetDefaultPawnClassForController_Implementation(AController* InController) {
	const auto* VePlayerController = Cast<AVePlayerController>(InController);
	if (IsValid(VePlayerController)) {
		switch (VePlayerController->GetInteractionMode()) {
		case EUIInteractionMode::VR:
			return VRPawnClass;
		default:
			return DefaultPawnClass;
		}
	}
	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

AActor* AVeGameMode::FindPlayerStart_Implementation(AController* Player, const FString& IncomingName) {
	const auto* VePlayerController = Cast<AVePlayerController>(Player);
	if (IsValid(VePlayerController)) {
		if (VePlayerController->LoginPortal.IsValid()) {
			// todo: find portals in space
			if (const auto* Portal = StaticPortals.Find(VePlayerController->LoginPortal)) {
				return Portal->Get();
			}
		}
	}

	if (DefaultPlayerStart.IsValid()) {
		return DefaultPlayerStart.Get();
	}

	return Super::FindPlayerStart_Implementation(Player, IncomingName);
}

bool AVeGameMode::Login_RequestUserMetadata(APlayerController* PlayerController, const FGuid& PlayerId) {
	if (!PlayerId.IsValid()) {
		LogErrorF("[GameMode] invalid user id");
		return false;
	}

	GET_MODULE_SUBSYSTEM(UserSubsystem, Api2, User);
	if (!UserSubsystem) {
		return false;
	}

	const auto Callback = MakeShared<FOnUserRequestCompleted2>();
	Callback->BindWeakLambda(this, [=](const FApiUserMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError) {
		if (IsNetMode(NM_Standalone)) {
			auto* VePlayerState = PlayerController->GetPlayerState<AVePlayerStateBase>();
			if (IsValid(VePlayerState)) {
				VePlayerState->Login_SetUserMetadata(FVeUserMetadata(InMetadata));
			}

			// Register player
			ActivePlayerControllers.Emplace(PlayerController, PlayerId);
			Analytics_ReportUserEnteringWorld(PlayerId);
			NativeOnUserLogin(PlayerController, PlayerId);
			return;
		}

		if (InResponseCode != EApi2ResponseCode::Ok) {
			VeLogErrorFunc("[GameMode] failed to request user metadata: %s", *InError);
			GameSession->KickPlayer(PlayerController, FText::FromString(TEXT("failed to authenticate")));
			return;
		}

		if (!InMetadata.bIsActive) {
			VeLogWarningFunc("[GameMode] failed to login: inactive: %s", *InError);
			GameSession->KickPlayer(PlayerController, FText::FromString(TEXT("not active")));
			return;
		}

		if (InMetadata.bIsBanned) {
			VeLogWarningFunc("[GameMode] failed to login: banned: %s", *InError);
			GameSession->KickPlayer(PlayerController, FText::FromString(TEXT("banned")));
			return;
		}

		auto* VePlayerState = PlayerController->GetPlayerState<AVePlayerStateBase>();
		if (IsValid(VePlayerState)) {
			VePlayerState->Login_SetUserMetadata(FVeUserMetadata(InMetadata));
		}

		// Register player
		ActivePlayerControllers.Emplace(PlayerController, PlayerId);
		Analytics_ReportUserEnteringWorld(PlayerId);
		NativeOnUserLogin(PlayerController, PlayerId);

		VeLogFunc("[GameMode] login success id={%s} name={%s}", *InMetadata.Id.ToString(EGuidFormats::DigitsWithHyphensLower), *InMetadata.Name);
	});

	// Request user metadata.
	UserSubsystem->GetUser(PlayerId, Callback);

	return true;
}

void AVeGameMode::InitializeHUDForPlayer_Implementation(APlayerController* NewPlayer) {
	if (AVePlayerController* VePlayerController = Cast<AVePlayerController>(NewPlayer)) {
		if (VePlayerController->GetNetMode() == NM_Standalone) {
			VePlayerController->Owner_InitializeHUD(HUDClass);
		} else {
			VePlayerController->ClientRPC_InitializeHUD(HUDClass);
		}
	} else {
		Super::InitializeHUDForPlayer_Implementation(NewPlayer);
	}
}

bool AVeGameMode::Authority_RequestUserKick(const FGuid& RequesterUserId, APlayerController* PlayerController) {
	if (!PlayerController) {
		LogErrorF("[GameMode] no player to kick");
		return false;
	}

	GET_MODULE_SUBSYSTEM(UserSubsystem, Api2, User);
	if (UserSubsystem) {
		const auto CallbackPtr = MakeShared<FOnUserRequestCompleted2>();

		CallbackPtr->BindWeakLambda(this, [=](const FApiUserMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError) {
			if (InResponseCode != EApi2ResponseCode::Ok) {
				LogErrorF("[GameMode] failed to request: %s", *InError);
			}

			if (InMetadata.bIsAdmin) {
				GameSession->KickPlayer(PlayerController, LOCTEXT("KickedByAdmin", "Kicked by admin"));
			}
		});

		UserSubsystem->GetUser(RequesterUserId, CallbackPtr);

		return true;
	}

	return false;
}

FGuid AVeGameMode::GetCurrentWorldId() const {
	if (const auto* VeWorldSettings = Cast<AVeWorldSettings>(GetWorldSettings())) {
		return VeWorldSettings->GetWorldId();
	}
	VeLogErrorFunc("Failed to get VeWorldSettings");
	return {};
}

void AVeGameMode::SpawnPlaceables(const int32 InOffset, const int32 InLimit, const bool bRecursive) {
	GET_MODULE_SUBSYSTEM(PlaceableSubsystem, Api2, Placeable);
	if (!PlaceableSubsystem) {
		return;
	}

	FApi2PlaceableBatchRequestMetadata RequestMetadata;
	RequestMetadata.WorldId = GetCurrentWorldId();
	VeLog("SpawnPlaceables: WorldId=%s", *RequestMetadata.WorldId.ToString(EGuidFormats::DigitsWithHyphensLower));
	if (!RequestMetadata.WorldId.IsValid()) {
		VeLogWarningFunc("invalid current space id, skipped placeables loading");
		return;
	}
	RequestMetadata.Offset = InOffset;
	RequestMetadata.Limit = InLimit;

	const auto Callback = MakeShared<FOnPlaceableBatchRequestCompleted2>();
	Callback->BindWeakLambda(this, [=](const FApiPlaceableBatchMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError) {
		const bool bSuccessful = InResponseCode == EApi2ResponseCode::Ok;
		if (!bSuccessful) {
			LogErrorF("[GameMode] failed to load a placeable batch: %s", *InError);
			return;
		}

		// Spawn placeables
		for (const auto& Placeable : InMetadata.Entities) {
			SpawnPlaceableItem(FVePlaceableMetadata(*Placeable));
		}

		// Request the next page if required
		if (bRecursive && InMetadata.Offset + InMetadata.Limit < InMetadata.Total) {
			SpawnPlaceables(InOffset + InLimit, InLimit, bRecursive);
		}
	});

	PlaceableSubsystem->Index(RequestMetadata, Callback);
}

void AVeGameMode::SpawnPlaceableItem(const FVePlaceableMetadata& InVeMetadata) {
	if (HasAuthority()) {
		AActor* PlaceableActor = UPlaceableLib::SpawnPlaceable(this, InVeMetadata, PlaceableClass);
		if (auto* PlaceableComponent = UPlaceableLib::GetPlaceableComponent(PlaceableActor)) {
			PlaceableComponent->LocalServer_OnLoaded(InVeMetadata);
		}
	}
}

void AVeGameMode::Analytics_ReportUserEnteringWorld(const FGuid& UserId) {
	GET_MODULE_SUBSYSTEM(AnalyticsSubsystem, Api2, Analytics);
	if (!AnalyticsSubsystem) {
		return;
	}

	FGuid WorldId;
	if (const auto* VeWorldSettings = Cast<AVeWorldSettings>(GetWorldSettings())) {
		WorldId = VeWorldSettings->GetWorldId();
	}

	const auto Callback = MakeShared<FOnAnalyticsRequestCompleted2>();
	Callback->BindLambda([=](const FApi2AnalyticsRequestMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError) {
		if (InResponseCode == EApi2ResponseCode::Ok) {
			VeLogVerboseFunc("[GameMode] Succes to report user event: Entering");
		}
	});

	FApi2AnalyticsRequestMetadata Metadata;
	Metadata.UserId = UserId;
	Metadata.Payload = "{}";
	Metadata.AppId = FVeConfigLibrary::GetAppId();
	Metadata.ContextEntityType = "world";
	Metadata.ContextEntityId = WorldId;
	Metadata.Event = "enter";
	Metadata.Configuration = FVeConfigLibrary::GetConfiguration();

	AnalyticsSubsystem->ReportEvent(Metadata, Callback);
}

void AVeGameMode::Analytics_ReportUserLeavingWorld(const FGuid& UserId) {
	GET_MODULE_SUBSYSTEM(AnalyticsSubsystem, Api2, Analytics);
	if (!AnalyticsSubsystem) {
		return;
	}

	FGuid WorldId;
	if (const auto* VeWorldSettings = Cast<AVeWorldSettings>(GetWorldSettings())) {
		WorldId = VeWorldSettings->GetWorldId();
	}

	const auto Callback = MakeShared<FOnAnalyticsRequestCompleted2>();
	Callback->BindLambda([=](const FApi2AnalyticsRequestMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError) {
		VeLogVerboseFunc("[GameMode] Succes to report user event: Leaving");
	});

	FApi2AnalyticsRequestMetadata Metadata;
	Metadata.UserId = UserId;
	Metadata.Payload = "{}";
	Metadata.AppId = FVeConfigLibrary::GetAppId();
	Metadata.ContextEntityType = "world";
	Metadata.ContextEntityId = WorldId;
	Metadata.Event = "leave";
	Metadata.Configuration = FVeConfigLibrary::GetConfiguration();

	AnalyticsSubsystem->ReportEvent(Metadata, Callback);
}

#undef LOCTEXT_NAMESPACE
