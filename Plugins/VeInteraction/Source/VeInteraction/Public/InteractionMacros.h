// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

/** Check if the actor is in the dedicated server network mode. */
#define ACTOR_IS_DEDICATED(Actor) (Actor->GetNetMode() == NM_DedicatedServer)
/** Check if the actor has a network authority and not in the client network mode. */
#define ACTOR_IS_AUTHORITY(Actor) (Actor->HasAuthority() && (Actor->GetNetMode() < NM_Client))
/** Check if the actor is in the standalone (offline) network mode. */
#define ACTOR_IS_STANDALONE(Actor) (Actor->GetNetMode() == NM_Standalone)
/** Check if the actor is cosmetic and able to process cosmetic (visual) events (is not in the dedicated server net mode). */
#define ACTOR_IS_COSMETIC(Actor) (Actor->GetNetMode() != NM_DedicatedServer)
/** Check if the actor is simulated proxy, so its state is fully replicated from the authority. */
#define ACTOR_IS_SIMULATED(Actor) ((Actor->GetLocalRole() == ROLE_SimulatedProxy))
/** Check if the actor is autonomous proxy, meaning that is belongs to the local machine. Valid for pawns, player controllers and client-spawned actors which do not exist at the server. */
#define ACTOR_IS_AUTONOMOUS(Actor) ((Actor->GetLocalRole() == ROLE_AutonomousProxy))

#if PLATFORM_WINDOWS

/** Simplifies proxying input actions from the pawn to the interactive control component. */
#define DEFINE_PAWN_INPUT_ACTION(Class, ActionType) void Class::OnInput ##ActionType ##Begin() { \
if (InteractiveControl == nullptr) return; \
InteractiveControl->OnInputBegin(EInteractionInputActionType ##:: ## ActionType); } \
void Class::OnInput ##ActionType ##End() { \
if (InteractiveControl == nullptr) return; \
InteractiveControl->OnInputEnd(EInteractionInputActionType ##:: ## ActionType); }

/** Simplifies proxying input axis actions from the pawn to the interactive control component. */
#define DEFINE_PAWN_INPUT_AXIS(Class, AxisType) void Class::OnInput ##AxisType (const float Value) { \
if (InteractiveControl == nullptr) return; \
InteractiveControl->OnInputAxis(EInteractionInputAxisType ##:: ## AxisType, Value); }

/** Simplifies proxying input actions from the VR pawn and right/left motion controllers to the interactive control component. */
#define DEFINE_VR_PAWN_INPUT_ACTION(Class, ActionType) void Class::OnInput ##ActionType ##Right ##Begin() { \
if (InteractiveControl == nullptr || RightMotionController == nullptr || RightMotionController->InteractiveControl == nullptr) return; \
InteractiveControl->OnInputBegin(EInteractionInputActionType ##:: ## ActionType, RightMotionController->InteractiveControl); } \
void Class::OnInput ##ActionType ##Right ##End() { \
if (InteractiveControl == nullptr || RightMotionController == nullptr || RightMotionController->InteractiveControl == nullptr) return; \
InteractiveControl->OnInputEnd(EInteractionInputActionType ##:: ## ActionType, RightMotionController->InteractiveControl); } \
void Class::OnInput ##ActionType ##Left ##Begin() { \
if (InteractiveControl == nullptr || LeftMotionController == nullptr || LeftMotionController->InteractiveControl == nullptr) return; \
InteractiveControl->OnInputBegin(EInteractionInputActionType ##:: ## ActionType, LeftMotionController->InteractiveControl); } \
void Class::OnInput ##ActionType ##Left ##End() { \
if (InteractiveControl == nullptr || LeftMotionController == nullptr || LeftMotionController->InteractiveControl == nullptr) return; \
InteractiveControl->OnInputEnd(EInteractionInputActionType ##:: ## ActionType, LeftMotionController->InteractiveControl); }

/** Simplifies proxying input axis actions from the VR pawn and the right/left motion controllers to the interactive control component. */
#define DEFINE_VR_PAWN_INPUT_AXIS(Class, AxisType) void Class::OnInput ##AxisType ##Right (const float Value) { \
if (InteractiveControl == nullptr || RightMotionController == nullptr || RightMotionController->InteractiveControl == nullptr) return; \
InteractiveControl->OnInputAxis(EInteractionInputAxisType ##:: ## AxisType, Value, RightMotionController->InteractiveControl); } \
void Class::OnInput ##AxisType ##Left (const float Value) { \
if (InteractiveControl == nullptr || LeftMotionController == nullptr || LeftMotionController->InteractiveControl == nullptr) return; \
InteractiveControl->OnInputAxis(EInteractionInputAxisType ##:: ## AxisType, Value, LeftMotionController->InteractiveControl); \
}

#endif