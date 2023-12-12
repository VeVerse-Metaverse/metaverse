// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#include "Pawns/InteractiveCharacter.h"

#include "VeShared.h"
#include "Interaction.h"
#include "Components/ALSDebugComponent.h"
#include "VePlayerControllerBase.h"
#include "Components/InteractionSubjectComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "EnhancedActionKeyMapping.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"


AInteractiveCharacter::AInteractiveCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {
	PrimaryActorTick.bCanEverTick = true;

	SetReplicates(true);
	SetReplicateMovement(true);

	InteractionSubjectComponent = CreateDefaultSubobject<UInteractionSubjectComponent>("InteractionSubject");

	PhysicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>("PhysicsHandle");
}

void AInteractiveCharacter::PostInitializeComponents() {
	Super::PostInitializeComponents();
}

void AInteractiveCharacter::Tick(const float DeltaSeconds) {
	Super::Tick(DeltaSeconds);
}

void AInteractiveCharacter::BeginPlay() {
	Super::BeginPlay();
}

void AInteractiveCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	SetPlayerController(nullptr);
	Super::EndPlay(EndPlayReason);
}

void AInteractiveCharacter::PossessedBy(AController* NewController) {
	Super::PossessedBy(NewController);
	NativeOnPlayerStateChanged(GetPlayerState());
}

void AInteractiveCharacter::UnPossessed() {
	Super::UnPossessed();
	NativeOnPlayerStateChanged(GetPlayerState());
}

void AInteractiveCharacter::OnRep_PlayerState() {
	Super::OnRep_PlayerState();
	NativeOnPlayerStateChanged(GetPlayerState());
}

void AInteractiveCharacter::SetPlayerController(APlayerController* NewPlayerController) {
	if (MyPlayerController == NewPlayerController) {
		return;
	}

	auto* OldPlayerController = MyPlayerController.Get();
	MyPlayerController = NewPlayerController;

	NativeOnPlayerControllerChanged(NewPlayerController, OldPlayerController);
}

void AInteractiveCharacter::NativeOnPlayerControllerChanged(APlayerController* NewPlayerController, APlayerController* OldPlayerController) {
	if (!IsRunningDedicatedServer()) {
		if (OldPlayerController) {
			for (auto& MappingContext : GameMappingContexts) {
				UnbindInputActions(OldPlayerController->InputComponent.Get(), MappingContext);
			}

			if (auto* BasePlayerController = Cast<AVePlayerControllerBase>(OldPlayerController)) {
				BasePlayerController->UnregisterMappingContextsAll(EAppMode::Game, this);
			}
		}

		if (NewPlayerController) {
			for (auto& MappingContext : GameMappingContexts) {
				BindInputActions(NewPlayerController->InputComponent.Get(), MappingContext);
			}

			if (auto* BasePlayerController = Cast<AVePlayerControllerBase>(NewPlayerController)) {
				BasePlayerController->RegisterMappingContextArray(EAppMode::Game, this, GameMappingContexts);
			}
		}
	}

	OnPlayerControllerChanged(NewPlayerController, OldPlayerController);
}

#pragma region PlayerState

void AInteractiveCharacter::OnReady_PlayerState(FPlayerStateDelegate Callback) {
	OnPlayerStateChanged.AddWeakLambda(this, [Callback](APlayerState* InPlayerState) {
		Callback.ExecuteIfBound(InPlayerState);
	});
	Callback.ExecuteIfBound(GetPlayerState());
}

void AInteractiveCharacter::NativeOnPlayerStateChanged(APlayerState* InPlayerState) {
	OnPlayerStateChanged.Broadcast(InPlayerState);
}

#pragma endregion PlayerState

#pragma region EnhancedInput

void AInteractiveCharacter::BindInputActions(UInputComponent* InInputComponent, UInputMappingContext* MappingContext) {
	if (!InInputComponent || !MappingContext) {
		VeLogErrorFunc("Failed to bind InputActions");
		return;
	}

	auto* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InInputComponent);
	if (!EnhancedInputComponent) {
		VeLogErrorFunc("Requires Enhanced Input System to be activated in project settings to function properly.");
		return;
	}

	TSet<const UInputAction*> UniqueActions;
	for (const FEnhancedActionKeyMapping& Keymapping : MappingContext->GetMappings()) {
		if (Keymapping.Action) {
			UniqueActions.Add(Keymapping.Action);
		}
	}

	for (const auto* Action : UniqueActions) {
		if (ActionBindingHandles.Find(Action)) {
			continue;
		}

		VeLogVerboseFunc("Bind input action: %s from %s", *Action->GetName(), *MappingContext->GetName());
		const FName FunctionName = FName(TEXT("On") + Action->GetFName().ToString());
		auto& EnhancedInputActionEventBinding = EnhancedInputComponent->BindAction(Action, ETriggerEvent::Triggered, Cast<UObject>(this), FunctionName);
		ActionBindingHandles.Emplace(Action, EnhancedInputActionEventBinding.GetHandle());
	}
}

void AInteractiveCharacter::UnbindInputActions(UInputComponent* InInputComponent, UInputMappingContext* MappingContext) {
	if (!InInputComponent || !MappingContext) {
		VeLogErrorFunc("Failed to unbind InputActions");
		return;
	}

	auto* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InInputComponent);
	if (!EnhancedInputComponent) {
		VeLogErrorFunc("Requires Enhanced Input System to be activated in project settings to function properly.");
		return;
	}

	// Unbind Actions

	TSet<const UInputAction*> UniqueActions;
	for (const FEnhancedActionKeyMapping& Keymapping : MappingContext->GetMappings()) {
		if (Keymapping.Action) {
			UniqueActions.Add(Keymapping.Action);
		}
	}

	for (const auto* Action : UniqueActions) {
		auto* Handle = ActionBindingHandles.Find(Action);
		if (!Handle) {
			continue;
		}

		VeLogVerboseFunc("Unbind input action: %s from %s", *Action->GetName(), *MappingContext->GetName());
		EnhancedInputComponent->RemoveBindingByHandle(*Handle);
		ActionBindingHandles.Remove(Action);
	}
}

void AInteractiveCharacter::OnForwardMovementAction(const FInputActionValue& Value) {
	Super::ForwardMovementAction(Value.GetMagnitude());
}

void AInteractiveCharacter::OnRightMovementAction(const FInputActionValue& Value) {
	Super::RightMovementAction(Value.GetMagnitude());
}

void AInteractiveCharacter::OnCameraUpAction(const FInputActionValue& Value) {
	Super::CameraUpAction(Value.GetMagnitude());
}

void AInteractiveCharacter::OnCameraRightAction(const FInputActionValue& Value) {
	Super::CameraRightAction(Value.GetMagnitude());
}

void AInteractiveCharacter::OnJumpAction(const FInputActionValue& Value) {
	Super::JumpAction(Value.Get<bool>());
}

void AInteractiveCharacter::OnSprintAction(const FInputActionValue& Value) {
	Super::SprintAction(Value.Get<bool>());
}

void AInteractiveCharacter::OnAimAction(const FInputActionValue& Value) {
	Super::AimAction(Value.Get<bool>());
}

void AInteractiveCharacter::OnCameraTapAction(const FInputActionValue& Value) {
	Super::CameraTapAction();
}

void AInteractiveCharacter::OnCameraHeldAction(const FInputActionValue& Value) {
	Super::CameraHeldAction();
}

void AInteractiveCharacter::OnStanceAction(const FInputActionValue& Value) {
	if (Value.Get<bool>()) {
		Super::StanceAction();
	}
}

void AInteractiveCharacter::OnWalkAction(const FInputActionValue& Value) {
	if (Value.Get<bool>()) {
		Super::WalkAction();
	}
}

void AInteractiveCharacter::OnRagdollAction(const FInputActionValue& Value) {
	if (Value.Get<bool>()) {
		Super::RagdollAction();
	}
}

void AInteractiveCharacter::OnVelocityDirectionAction(const FInputActionValue& Value) {
	if (Value.Get<bool>()) {
		Super::VelocityDirectionAction();
	}
}

void AInteractiveCharacter::OnLookingDirectionAction(const FInputActionValue& Value) {
	if (Value.Get<bool>()) {
		Super::LookingDirectionAction();
	}
}

void AInteractiveCharacter::OnDebugToggleHudAction(const FInputActionValue& Value) {
	if (Value.Get<bool>()) {
		UALSDebugComponent* DebugComp = Cast<UALSDebugComponent>(GetComponentByClass(UALSDebugComponent::StaticClass()));
		if (DebugComp) {
			DebugComp->ToggleHud();
		}
	}
}

void AInteractiveCharacter::OnDebugToggleDebugViewAction(const FInputActionValue& Value) {
	if (Value.Get<bool>()) {
		UALSDebugComponent* DebugComp = Cast<UALSDebugComponent>(GetComponentByClass(UALSDebugComponent::StaticClass()));
		if (DebugComp) {
			DebugComp->ToggleDebugView();
		}
	}
}

void AInteractiveCharacter::OnDebugToggleTracesAction(const FInputActionValue& Value) {
	if (Value.Get<bool>()) {
		UALSDebugComponent* DebugComp = Cast<UALSDebugComponent>(GetComponentByClass(UALSDebugComponent::StaticClass()));
		if (DebugComp) {
			DebugComp->ToggleTraces();
		}
	}
}

void AInteractiveCharacter::OnDebugToggleShapesAction(const FInputActionValue& Value) {
	if (Value.Get<bool>()) {
		UALSDebugComponent* DebugComp = Cast<UALSDebugComponent>(GetComponentByClass(UALSDebugComponent::StaticClass()));
		if (DebugComp) {
			DebugComp->ToggleDebugShapes();
		}
	}
}

void AInteractiveCharacter::OnDebugToggleLayerColorsAction(const FInputActionValue& Value) {
	if (Value.Get<bool>()) {
		UALSDebugComponent* DebugComp = Cast<UALSDebugComponent>(GetComponentByClass(UALSDebugComponent::StaticClass()));
		if (DebugComp) {
			DebugComp->ToggleLayerColors();
		}
	}
}

void AInteractiveCharacter::OnDebugToggleCharacterInfoAction(const FInputActionValue& Value) {
	if (Value.Get<bool>()) {
		UALSDebugComponent* DebugComp = Cast<UALSDebugComponent>(GetComponentByClass(UALSDebugComponent::StaticClass()));
		if (DebugComp) {
			DebugComp->ToggleCharacterInfo();
		}
	}
}

void AInteractiveCharacter::OnDebugToggleSlomoAction(const FInputActionValue& Value) {
	if (Value.Get<bool>()) {
		UALSDebugComponent* DebugComp = Cast<UALSDebugComponent>(GetComponentByClass(UALSDebugComponent::StaticClass()));
		if (DebugComp) {
			DebugComp->ToggleSlomo();
		}
	}
}

void AInteractiveCharacter::OnDebugFocusedCharacterCycleAction(const FInputActionValue& Value) {
	UALSDebugComponent* DebugComp = Cast<UALSDebugComponent>(GetComponentByClass(UALSDebugComponent::StaticClass()));
	if (DebugComp) {
		DebugComp->FocusedDebugCharacterCycle(Value.GetMagnitude() > 0);
	}
}

void AInteractiveCharacter::OnDebugToggleMeshAction(const FInputActionValue& Value) {
	if (Value.Get<bool>()) {
		UALSDebugComponent* DebugComp = Cast<UALSDebugComponent>(GetComponentByClass(UALSDebugComponent::StaticClass()));
		if (DebugComp) {
			DebugComp->ToggleDebugMesh();
		}
	}
}

void AInteractiveCharacter::OnDebugOpenOverlayMenuAction(const FInputActionValue& Value) {
	UALSDebugComponent* DebugComp = Cast<UALSDebugComponent>(GetComponentByClass(UALSDebugComponent::StaticClass()));
	if (DebugComp) {
		DebugComp->OpenOverlayMenu(Value.Get<bool>());
	}
}

void AInteractiveCharacter::OnDebugOverlayMenuCycleAction(const FInputActionValue& Value) {
	UALSDebugComponent* DebugComp = Cast<UALSDebugComponent>(GetComponentByClass(UALSDebugComponent::StaticClass()));
	if (DebugComp) {
		DebugComp->OverlayMenuCycle(Value.GetMagnitude() > 0);
	}
}

#pragma endregion EnhancedInput
