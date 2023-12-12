// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

// Define a message as an enumeration.
#define REGISTER_NAME(name) const static FName NAME_##name = TEXT(#name)
REGISTER_NAME(MenuModeInspect);
REGISTER_NAME(TraceInteractionFocus);
REGISTER_NAME(TraceInteractionOverlap);

REGISTER_NAME(InteractActivate);
REGISTER_NAME(InteractUserCustom);
REGISTER_NAME(InteractGrab);
REGISTER_NAME(InteractUse);
REGISTER_NAME(InteractActivateLeft);
REGISTER_NAME(InteractGrabLeft);
REGISTER_NAME(InteractUseLeft);
REGISTER_NAME(InteractActivateRight);
REGISTER_NAME(InteractGrabRight);
REGISTER_NAME(InteractUseRight);
REGISTER_NAME(InteractJump);
REGISTER_NAME(InteractSprint);
REGISTER_NAME(InteractWalk);
REGISTER_NAME(InteractAxisMoveForward);
REGISTER_NAME(InteractAxisMoveRight);
REGISTER_NAME(InteractAxisLookUp);
REGISTER_NAME(InteractAxisTurnRight);
REGISTER_NAME(InteractAxisX);
REGISTER_NAME(InteractAxisY);
REGISTER_NAME(InteractAxisZ);
REGISTER_NAME(InteractResetVR);
REGISTER_NAME(InteractTeleportLeft);
REGISTER_NAME(InteractTeleportRight);
REGISTER_NAME(InteractTeleportLeftDirectionAxisX);
REGISTER_NAME(InteractTeleportLeftDirectionAxisY);
REGISTER_NAME(InteractTeleportRightDirectionAxisX);
REGISTER_NAME(InteractTeleportRightDirectionAxisY);

REGISTER_NAME(DefaultSceneRoot);
REGISTER_NAME(VROrigin);
#undef REGISTER_NAME
