// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "VePlayerControllerBase.h"

#include "VeShared.h"
#include "VeGameFramework.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "PlayerComponent.h"
#include "Perception/AISense_Hearing.h"
#include "VePlayerStateBase.h"

namespace Options {
	const FName InputToggleInspectorMode = TEXT("ToggleInspectMode");
	const FName InputToggleEditorMode = TEXT("ToggleEditMode");
	const FName InputToggleMenuMode = TEXT("ToggleMenu");
	const FName InputToggleTextChat = TEXT("ToggleTextChat");
}

AVePlayerControllerBase::AVePlayerControllerBase() {
	MappingContextsCollections.Emplace(EAppMode::Game);
	MappingContextsCollections.Emplace(EAppMode::Editor);
	MappingContextsCollections.Emplace(EAppMode::Inspector);
}

void AVePlayerControllerBase::PostInitializeComponents() {
	Super::PostInitializeComponents();
	SetPlayerControllerForPlayerComponents(this, this);

	if (auto* World = GetWorld()) {
		World->GameStateSetEvent.AddUObject(this, &AVePlayerControllerBase::NativeOnGameStateChanged);
	}
}

void AVePlayerControllerBase::OnRep_PlayerState() {
	Super::OnRep_PlayerState();
	NativeOnPlayerStateChanged(PlayerState.Get());
}

#pragma region Pawn

void AVePlayerControllerBase::OnReady_Pawn(FPawnChangedDelegate Callback) {
	OnPawnChanged.AddWeakLambda(this, [Callback](APawn* InNewPawn, APawn* InPrevPawn) {
		Callback.ExecuteIfBound(InNewPawn, InPrevPawn);
	});
	Callback.ExecuteIfBound(GetPawn(), nullptr);
}

void AVePlayerControllerBase::NativeOnPawnChanged(APawn* NewPawn, APawn* PrevPawn) {
	SetPlayerControllerForPlayerComponents(PrevPawn, nullptr);
	SetPlayerControllerForPlayerComponents(NewPawn, this);
	OnPawnChanged_BP(NewPawn, PrevPawn);
	OnPawnChanged.Broadcast(NewPawn, PrevPawn);
}

#pragma endregion Pawn

#pragma region PlayerState

void AVePlayerControllerBase::OnReady_PlayerState(FPlayerStateDelegate Callback) {
	if (PlayerState) {
		Callback.ExecuteIfBound(PlayerState.Get());
		return;
	}
	OnPlayerStateChanged.AddWeakLambda(this, [Callback](APlayerState* InPlayerState) {
		Callback.ExecuteIfBound(InPlayerState);
	});
}

void AVePlayerControllerBase::NativeOnPlayerStateChanged(APlayerState* InPlayerState) {
	OnPlayerStateChanged.Broadcast(InPlayerState);
	OnPlayerStateChanged.Clear();
}

#pragma endregion PlayerState

#pragma region EnhancedInput

void AVePlayerControllerBase::SetupInputComponent() {
	Super::SetupInputComponent();

	InputComponent->BindAction(Options::InputToggleMenuMode, IE_Pressed, this, &ThisClass::Input_ToggleMenuMode);
	InputComponent->BindAction(Options::InputToggleEditorMode, IE_Pressed, this, &ThisClass::Input_ToggleEditorMode);
	InputComponent->BindAction(Options::InputToggleInspectorMode, IE_Pressed, this, &ThisClass::Input_ToggleInspectorMode);
	InputComponent->BindAction(Options::InputToggleTextChat, IE_Pressed, this, &ThisClass::Input_ToggleTextChatMode);

	RegisterMappingContextArray(EAppMode::Game, this, GameMappingContexts);
	RegisterMappingContextArray(EAppMode::Editor, this, EditorMappingContexts);
	RegisterMappingContextArray(EAppMode::Inspector, this, InspectorMappingContexts);

	SetAppMode(EAppMode::Game);
}

void AVePlayerControllerBase::RegisterMappingContext(EAppMode InPlayerInputMode, UObject* Object, UInputMappingContext* MappingContext) {
	if (IsRunningDedicatedServer()) {
		VeLogWarningFunc("Failed to register MappingContext for dedicated server.");
		return;
	}

	auto* MappingContextsCollection = MappingContextsCollections.Find(InPlayerInputMode);
	if (!MappingContextsCollection) {
		return;
	}

	if (!MappingContextsCollection->AddMappingContext(Object, MappingContext)) {
		return;
	}

	if (AppMode == InPlayerInputMode) {
		auto* EnhancedInputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
		if (EnhancedInputSubsystem) {
			const FModifyContextOptions& ModifyContextOptions = FModifyContextOptions();
			EnhancedInputSubsystem->AddMappingContext(MappingContext, 0, ModifyContextOptions);
		}
	}
}

void AVePlayerControllerBase::RegisterMappingContextArray(EAppMode InPlayerInputMode, UObject* Object, const TArray<TObjectPtr<UInputMappingContext>>& MappingContexts) {
	if (IsRunningDedicatedServer()) {
		VeLogWarningFunc("Failed to register MappingContext for dedicated server.");
		return;
	}

	auto* MappingContextsCollection = MappingContextsCollections.Find(InPlayerInputMode);
	if (!MappingContextsCollection) {
		return;
	}

	auto AddedMappingContexts = MappingContextsCollection->AddMappingContextArray(Object, MappingContexts);
	if (!AddedMappingContexts.Num()) {
		return;
	}

	if (AppMode == InPlayerInputMode) {
		auto* EnhancedInputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
		if (EnhancedInputSubsystem) {
			const FModifyContextOptions& ModifyContextOptions = FModifyContextOptions();
			for (auto& MappingContext : AddedMappingContexts) {
				EnhancedInputSubsystem->AddMappingContext(MappingContext, 0, ModifyContextOptions);
			}
		}
	}
}

void AVePlayerControllerBase::UnregisterMappingContext(EAppMode InPlayerInputMode, UObject* Object, UInputMappingContext* MappingContext) {
	if (IsRunningDedicatedServer()) {
		VeLogWarningFunc("Failed to unregister MappingContext for dedicated server.");
		return;
	}

	auto* MappingContextsCollection = MappingContextsCollections.Find(InPlayerInputMode);
	if (!MappingContextsCollection) {
		return;
	}

	if (!MappingContextsCollection->RemoveMappingContext(Object, MappingContext)) {
		return;
	}

	if (AppMode == InPlayerInputMode) {
		auto* EnhancedInputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
		if (EnhancedInputSubsystem) {
			const FModifyContextOptions& ModifyContextOptions = FModifyContextOptions();
			EnhancedInputSubsystem->RemoveMappingContext(MappingContext, ModifyContextOptions);
		}
	}
}

void AVePlayerControllerBase::UnregisterMappingContextArray(EAppMode InPlayerInputMode, UObject* Object, const TArray<TObjectPtr<UInputMappingContext>>& MappingContexts) {
	if (IsRunningDedicatedServer()) {
		VeLogWarningFunc("Failed to unregister MappingContext for dedicated server.");
		return;
	}

	auto* MappingContextsCollection = MappingContextsCollections.Find(InPlayerInputMode);
	if (!MappingContextsCollection) {
		return;
	}

	auto RemovedMappingContexts = MappingContextsCollection->RemoveMappingContextArray(Object, MappingContexts);
	if (!RemovedMappingContexts.Num()) {
		return;
	}

	if (AppMode == InPlayerInputMode) {
		auto* EnhancedInputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
		if (EnhancedInputSubsystem) {
			const FModifyContextOptions& ModifyContextOptions = FModifyContextOptions();
			for (auto& MappingContext : RemovedMappingContexts) {
				EnhancedInputSubsystem->RemoveMappingContext(MappingContext, ModifyContextOptions);
			}
		}
	}
}

void AVePlayerControllerBase::UnregisterMappingContextsAll(EAppMode InPlayerInputMode, UObject* Object) {
	if (IsRunningDedicatedServer()) {
		VeLogWarningFunc("Failed to unregister MappingContext for dedicated server.");
		return;
	}

	auto* MappingContextsCollection = MappingContextsCollections.Find(InPlayerInputMode);
	if (!MappingContextsCollection) {
		return;
	}

	auto RemovedMappingContexts = MappingContextsCollection->ClearMappingContext(Object);
	if (!RemovedMappingContexts.Num()) {
		return;
	}

	if (AppMode == InPlayerInputMode) {
		auto* EnhancedInputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
		if (EnhancedInputSubsystem) {
			const FModifyContextOptions& ModifyContextOptions = FModifyContextOptions();
			for (auto& MappingContext : RemovedMappingContexts) {
				EnhancedInputSubsystem->RemoveMappingContext(MappingContext, ModifyContextOptions);
			}
		}
	}
}

void AVePlayerControllerBase::SetAppMode(EAppMode InAppMode) {
	if (AppMode == InAppMode) {
		return;
	}

	auto* EnhancedInputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (!EnhancedInputSubsystem) {
		return;
	}

	EnhancedInputSubsystem->ClearAllMappings();

	auto* MappingContextsCollection = MappingContextsCollections.Find(InAppMode);
	if (MappingContextsCollection) {
		const FModifyContextOptions& ModifyContextOptions = FModifyContextOptions();

		TArray<TObjectPtr<UInputMappingContext>> MappingContexts;
		MappingContextsCollection->GetMappingContexts(MappingContexts);

		for (auto& MappingContext : MappingContexts) {
			EnhancedInputSubsystem->AddMappingContext(MappingContext, 0, ModifyContextOptions);
		}
	}

	AppMode = InAppMode;
	NativeOnAppModeChanged(AppMode);
}

void AVePlayerControllerBase::NativeOnAppModeChanged(EAppMode NewPlayerInputMode) {
	OnAppModeChanged(NewPlayerInputMode);
}

void AVePlayerControllerBase::SetPawn(APawn* InPawn) {
	Super::SetPawn(InPawn);
	if (InPawn != CurrentPawn) {
		auto* PrevPawn = CurrentPawn.Get();
		CurrentPawn = InPawn;
		NativeOnPawnChanged(InPawn, PrevPawn);
	}
}

void AVePlayerControllerBase::SetPlayerControllerForPlayerComponents(AActor* InActor, APlayerController* InPlayerController) {
	if (InActor) {
		TArray<UActorComponent*> Components;
		InActor->GetComponents(UPlayerComponent::StaticClass(), Components, true);
		for (auto* Component : Components) {
			if (auto* PlayerComponent = Cast<UPlayerComponent>(Component)) {
				PlayerComponent->SetPlayerController(InPlayerController);
			}
		}
	}
}

void AVePlayerControllerBase::BindInputActions(UInputMappingContext* MappingContext) {
	if (!IsValid(MappingContext)) {
		return;
	}

	auto* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (!EnhancedInputComponent) {
		VeLogErrorFunc("Requires Enhanced Input System to be activated in project settings to function properly.");
		return;
	}

	const TArray<FEnhancedActionKeyMapping>& Mappings = MappingContext->GetMappings();

	// There may be more than one keymapping assigned to one action. So, first filter duplicate action entries to prevent multiple delegate bindings
	TSet<const UInputAction*> UniqueActions;
	for (const FEnhancedActionKeyMapping& Keymapping : Mappings) {
		if (Keymapping.Action) {
			UniqueActions.Add(Keymapping.Action);
		}
	}

	for (const UInputAction* UniqueAction : UniqueActions) {
		VeLogVerboseFunc("Bind input action: %s", *UniqueAction->GetName());
		const FName FunctionName = FName(TEXT("On") + UniqueAction->GetFName().ToString());
		EnhancedInputComponent->BindAction(UniqueAction, ETriggerEvent::Triggered, Cast<UObject>(this), FunctionName);
	}
}

void AVePlayerControllerBase::ProcessChatCommand(const FString& InCommand) {
	if (InCommand.StartsWith("/ai ")) {
		// Cut off the "/ai" part of the command.
		const FString& Message = InCommand.RightChop(4).TrimStartAndEnd();
		SayToAI(Message);
	}
}

void AVePlayerControllerBase::SayToAI(const FString& InMessage) {
	if (HasAuthority()) {
		if (IsLocalController()) {
			// Is running on a listen server or a standalone game.
			Server_SayToAI(InMessage);
		} else {
			// Is running on a dedicated server.
			if (auto* State = GetPlayerState<AVePlayerStateBase>(); IsValid(State)) {
				State->OnSpeechRecognitionCompleted(InMessage);
			}

			if (const APawn* MyPawn = GetPawn(); IsValid(MyPawn)) {
				UAISense_Hearing::ReportNoiseEvent(this, MyPawn->GetActorLocation(), 1.0f, this, 0.0f, FName("SpeechFinished"));
			}
		}
	}
}

void AVePlayerControllerBase::Server_SayToAI_Implementation(const FString& InMessage) {
	if (auto* State = GetPlayerState<AVePlayerStateBase>(); IsValid(State)) {
		State->OnSpeechRecognitionCompleted(InMessage);
	}

	if (const APawn* MyPawn = GetPawn(); IsValid(MyPawn)) {
		UAISense_Hearing::ReportNoiseEvent(this, MyPawn->GetActorLocation(), 1.0f, this, 0.0f, FName("SpeechFinished"));
	}
}

void AVePlayerControllerBase::Input_ToggleInspectorMode() {
	if (GetAppMode() == EAppMode::Inspector) {
		SetAppMode(EAppMode::Game);
	} else if (GetAppMode() == EAppMode::Game) {
		SetAppMode(EAppMode::Inspector);
	}
}

void AVePlayerControllerBase::Input_ToggleEditorMode() {
	if (GetAppMode() == EAppMode::Editor) {
		SetAppMode(EAppMode::Game);
	} else if (GetAppMode() == EAppMode::Game) {
		SetAppMode(EAppMode::Editor);
	}
}

void AVePlayerControllerBase::Input_ToggleMenuMode() {
	if (GetAppMode() == EAppMode::Menu) {
		SetAppMode(EAppMode::Game);
	} else {
		SetAppMode(EAppMode::Menu);
	}
}

void AVePlayerControllerBase::Input_ToggleTextChatMode() {
	if (GetAppMode() == EAppMode::TextChat) {
		SetAppMode(EAppMode::Game);
	} else {
		SetAppMode(EAppMode::TextChat);
	}
}

#pragma endregion EnhancedInput
