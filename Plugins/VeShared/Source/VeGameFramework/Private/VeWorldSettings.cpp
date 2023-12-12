// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "VeWorldSettings.h"

#include "Net/UnrealNetwork.h"
#include "VeGameFramework.h"
#include "GameFrameworkServerSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"
#include "GameFrameworkWorldSubsystem.h"
#include "VeGameStateBase.h"
#include "VeApi2.h"
#include "Api2AuthSubsystem.h"

#undef VE_LOG_MODULE
#define VE_LOG_MODULE Initialization

static FGuid NotCompleteGuid(0, 0, 0, 1);
static FGuid NullGuid(0, 0, 0, 0);


AVeWorldSettings::AVeWorldSettings() {
	// SetReplicates(true);
	bReplicates = true;

	WorldId = NotCompleteGuid;
	ServerId = NotCompleteGuid;
}

void AVeWorldSettings::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AVeWorldSettings, WorldId, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(AVeWorldSettings, ServerId, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(AVeWorldSettings, bServerInitializationComplete, COND_None);
}

void AVeWorldSettings::NotifyBeginPlay() {
	VeLog("[%p] Begin initialization", this);
	bBegunPlay = true;

	// Set ServerId, WorldId
	if (!IsNetMode(NM_Client)) {
		if (auto* World = GetWorld()) {
			if (IsRunningDedicatedServer()) {
				GET_MODULE_SUBSYSTEM(ServerSubsystem, GameFramework, Server);
				if (ServerSubsystem) {
					SetServerId(ServerSubsystem->GetServerId());
					SetWorldId(WorldId = ServerSubsystem->GetWorldId());
				}
			} else {
				FGuid NewWorldId;

				if (const auto* GameMode = World->GetAuthGameMode()) {
					NewWorldId = FGuid(UGameplayStatics::ParseOption(GameMode->OptionsString, TEXT("WorldId")));
				}

#if WITH_EDITORONLY_DATA
				if (!NewWorldId.IsValid()) {
					NewWorldId = EditorWorldId;
				}
#endif

				SetServerId(FGuid());
				SetWorldId(NewWorldId);
			}
		}
	}

	// Set GameStateComplete
	if (GetWorld()->GetGameState()) {
		VeLog("[%p] GameState already valid: %p", this, GetWorld()->GetGameState());
		bGameStateComplete = true;

		CheckInitializationComplete();

	} else {
		VeLog("[%p] GameState not valid: %p", this, GetWorld()->GetGameState());
		World_GameStateSetHandle = GetWorld()->GameStateSetEvent.AddWeakLambda(this, [=](AGameStateBase* InGameStateBase) {
			VeLog("[%p] GameState changed: %p", this, InGameStateBase);

			if (InGameStateBase) {
				bGameStateComplete = true;

				CheckInitializationComplete();

				if (World_GameStateSetHandle.IsValid()) {
					GetWorld()->GameStateSetEvent.Remove(World_GameStateSetHandle);
					World_GameStateSetHandle.Reset();
				}
			}
		});
	}

	// Super::NotifyBeginPlay();
}

void AVeWorldSettings::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);

	GET_MODULE_SUBSYSTEM(AuthSubsystem, Api2, Auth);
	if (AuthSubsystem) {
		if (AuthSubsystem_OnChangeAuthorizationStateHandle.IsValid()) {
			AuthSubsystem->GetOnChangeAuthorizationState().Remove(AuthSubsystem_OnChangeAuthorizationStateHandle);
		}
	}
}

const FGuid& AVeWorldSettings::GetWorldId() const {
	if (WorldId == NotCompleteGuid) {
		return NullGuid;
	} else {
		return WorldId;
	}
}

const FGuid& AVeWorldSettings::GetServerId() const {
	if (ServerId == NotCompleteGuid) {
		return NullGuid;
	} else {
		return ServerId;
	}
}

FGameFrameworkWorldItem* AVeWorldSettings::GetWorldItem() const {
	return WorldItem.Get();
}

void AVeWorldSettings::OnReady_WorldId(const FWorldIdDelegate& Callback) {
	if (WorldId == NotCompleteGuid) {
		WorldIdCallbacks.Emplace(Callback);
	} else {
		Callback.ExecuteIfBound(WorldId);
	}
}

void AVeWorldSettings::OnReady_ServerId(const FServerIdDelegate& Callback) {
	if (ServerId == NotCompleteGuid) {
		ServerIdCallbacks.Emplace(Callback);
	} else {
		Callback.ExecuteIfBound(ServerId);
	}
}

void AVeWorldSettings::OnReady_WorldItem(const FWorldItemDelegate& Callback) {
	if (WorldItem.IsValid()) {
		Callback.ExecuteIfBound(WorldItem);
	} else {
		WorldItemCallbacks.Emplace(Callback);
	}
}

void AVeWorldSettings::OnReady_ClientInitializationComplete(const FInitializationDelegate& Callback) {
	if (bClientInitializationComplete) {
		Callback.ExecuteIfBound();
	} else {
		ClientInitCallbacks.Emplace(Callback);
	}
}

void AVeWorldSettings::OnReady_ServerInitializationComplete(const FInitializationDelegate& Callback) {
	if (bServerInitializationComplete) {
		Callback.ExecuteIfBound();
	} else {
		ServerInitCallbacks.Emplace(Callback);
	}
}

void AVeWorldSettings::NativeOnWorldId(bool IsServer) {
	for (auto& Callback : WorldIdCallbacks) {
		Callback.ExecuteIfBound(WorldId);
	}
	WorldIdCallbacks.Empty();


	GET_MODULE_SUBSYSTEM(AuthSubsystem, Api2, Auth);
	if (!AuthSubsystem) {
		NativeOnInitializationFailure();
		return;
	}

	// Wait for authorization
	if (AuthSubsystem->IsAuthorized()) {
		NativeOnAuthorization(IsServer);
	} else {
		const auto OnChangeAuthorizationStateCallback = [=](EAuthorizationState AuthorizationState) {
			if (AuthSubsystem->IsAuthorized()) {
				NativeOnAuthorization(IsServer);
				AuthSubsystem->GetOnChangeAuthorizationState().Remove(AuthSubsystem_OnChangeAuthorizationStateHandle);
			}
		};
		AuthSubsystem_OnChangeAuthorizationStateHandle = AuthSubsystem->GetOnChangeAuthorizationState().AddWeakLambda(this, OnChangeAuthorizationStateCallback);
	}
}

void AVeWorldSettings::NativeOnServerId(bool IsServer) {
	for (auto& Callback : ServerIdCallbacks) {
		Callback.ExecuteIfBound(ServerId);
	}
	ServerIdCallbacks.Empty();

	if (IsServer) {
		CheckServerInitializationComplete();
	} else {
		CheckClientInitializationComplete();
	}
}

void AVeWorldSettings::NativeOnAuthorization(bool IsServer) {
	GET_MODULE_SUBSYSTEM(WorldSubsystem, GameFramework, World);
	if (!WorldSubsystem) {
		NativeOnInitializationFailure();
		return;
	}

	const auto WorldItemCallback = MakeShared<FOnGameFrameworkWorldItemCreated>();
	WorldItemCallback->BindWeakLambda(this, [=](TSharedRef<FGameFrameworkWorldItem> InWorldItem, const bool bIsValid, const FString& InError) {
		WorldItem = InWorldItem;
		NativeOnWorldItem(IsServer);
	});
	WorldSubsystem->GetItem(WorldId, WorldItemCallback);
}

void AVeWorldSettings::NativeOnWorldItem(bool IsServer) {
	for (auto& Callback : WorldItemCallbacks) {
		Callback.ExecuteIfBound(WorldItem);
	}
	WorldItemCallbacks.Empty();

	if (IsServer) {
		CheckServerInitializationComplete();
	} else {
		CheckClientInitializationComplete();
	}
}

void AVeWorldSettings::CheckInitializationComplete() {
	if (!IsNetMode(NM_Client)) {
		CheckServerInitializationComplete();
	}
	if (!IsNetMode(NM_DedicatedServer)) {
		CheckClientInitializationComplete();
	}
}

void AVeWorldSettings::NativeOnClientInitializationComplete() {
	VeLog("[%p] World initialization complete: Client. WorldId=%s; ServerId=%s", this, *WorldId.ToString(EGuidFormats::DigitsWithHyphensLower), *ServerId.ToString(EGuidFormats::DigitsWithHyphensLower));

	for (auto& Callback : ClientInitCallbacks) {
		Callback.ExecuteIfBound();
	}
	ClientInitCallbacks.Empty();
}

void AVeWorldSettings::NativeOnServerInitializationComplete() {
	VeLog("[%p] World initialization complete: Server. WorldId=%s; ServerId=%s", this, *WorldId.ToString(EGuidFormats::DigitsWithHyphensLower), *ServerId.ToString(EGuidFormats::DigitsWithHyphensLower))

	for (auto& Callback : ServerInitCallbacks) {
		Callback.ExecuteIfBound();
	}
	ServerInitCallbacks.Empty();
}

void AVeWorldSettings::NativeOnInitializationSuccessful() {
	VeLog("[%p] World initialization complete: Successful", this)
	Super::NotifyBeginPlay();
}

void AVeWorldSettings::NativeOnInitializationFailure() {
	VeLog("[%p] World initialization complete: Failure", this)
	// Super::NotifyBeginPlay();
}

void AVeWorldSettings::CheckClientInitializationComplete() {
	if (bClientGameStateInitializationStage) {
		return;
	}

	VeLog("[%p] CheckClientInit. ServerId=%d, WorldId=%d, WorldItem=%d, bGameStateComplete=%d, ServerInitialization=%d",
		this,
		ServerId != NotCompleteGuid,
		WorldId != NotCompleteGuid,
		WorldItem.IsValid(),
		bGameStateComplete,
		bServerInitializationComplete
		);

	if (!bServerInitializationComplete) {
		return;
	}

	if (!IsInitComplete()) {
		return;
	}

	bClientGameStateInitializationStage = true;

	const auto OnComplete = [=]() {
		bClientInitializationComplete = true;
		NativeOnClientInitializationComplete();
		NativeOnInitializationSuccessful();
	};

	if (const auto* World = GetWorld()) {
		if (auto* VeGameState = World->GetGameState<AVeGameStateBase>()) {
			const auto OnClientInitialization = AVeGameStateBase::FGameStateDynamicDelegate::CreateWeakLambda(this, OnComplete);
			VeGameState->OnReady_ClientInitialization(OnClientInitialization);
			VeGameState->BeginInitialization();
			return;
		} else {
			VeLogFunc("Failed to get VeGameState.");
		}
	}

	OnComplete();
}

void AVeWorldSettings::CheckServerInitializationComplete() {
	if (bServerGameStateInitializationStage) {
		return;
	}

	VeLog("[%p] CheckServerInit. ServerId=%d, WorldId=%d, WorldItem=%d, bGameStateComplete=%d",
		this,
		ServerId != NotCompleteGuid,
		WorldId != NotCompleteGuid,
		WorldItem.IsValid(),
		bGameStateComplete
		);

	if (!IsInitComplete()) {
		return;
	}

	bServerGameStateInitializationStage = true;

	const auto OnComplete = [=]() {
		bServerInitializationComplete = true;
		NativeOnServerInitializationComplete();
		if (IsNetMode(NM_DedicatedServer)) {
			NativeOnInitializationSuccessful();
		} else {
			CheckClientInitializationComplete();
		}
	};

	if (const auto* World = GetWorld()) {
		if (auto* VeGameState = World->GetGameState<AVeGameStateBase>()) {
			const auto OnServerInitialization = AVeGameStateBase::FGameStateDynamicDelegate::CreateWeakLambda(this, OnComplete);
			VeGameState->OnReady_ServerInitialization(OnServerInitialization);
			VeGameState->BeginInitialization();
			return;
		}
	}

	OnComplete();
}

bool AVeWorldSettings::IsInitComplete() const {
	return bBegunPlay
		&& ServerId != NotCompleteGuid
		&& WorldId != NotCompleteGuid
		&& WorldItem.IsValid()
		&& bGameStateComplete;
}

void AVeWorldSettings::OnRep_WorldId() {
	NativeOnWorldId(false);
}

void AVeWorldSettings::OnRep_ServerId() {
	NativeOnServerId(false);
}

void AVeWorldSettings::OnRep_ServerInitializationComplete() {
	CheckClientInitializationComplete();
}

void AVeWorldSettings::SetWorldId(const FGuid& InWorldId) {
	WorldId = InWorldId;
	NativeOnWorldId(true);
}

void AVeWorldSettings::SetServerId(const FGuid& InServerId) {
	ServerId = InServerId;
	NativeOnServerId(true);
}
