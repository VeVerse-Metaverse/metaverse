// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

/** Type of interaction between a control (pawn or motion controller) and the target (interactive object). */
UENUM(BlueprintType)
enum class EInteractionType : uint8 {
	/** Control has line of sight on a target. */
	Focus,
	/** Control overlaps with a target. */
	Overlap,
	/** Activate input action from control to a target. */
	Activate,
	/** Grab, attach or detach a target. */
	Grab,
	/** Use input action sent from a control to a target. */
	Use,
	/** Custom events sent from a control to a target. */
	Events,
	/** Custom user inputs from a control to a target. */
	User
};

/** Type of input (activate or use) actions to send from a control to the target. */
UENUM(BlueprintType)
enum class EInteractionInputActionType : uint8 {
	/** No action type. */
	ActionNone UMETA(Hidden),
	/** Activate a focused or overlapped interactive object placed at the world. */
	ActionActivate UMETA(DisplayName="Activate"),
	/** Custom user input*/
	ActionUserInput UMETA(DisplayName="User"),
	/** Grab or drop an interactive object overlapped by a control. Usually means that the object would be attached to or detached from the control. */
	ActionGrab UMETA(DisplayName="Grab"),
	/** Use an interactive object attached to the control. */
	ActionUse UMETA(DisplayName="Use"),
	/** Use an interactive object attached to the control. */
	ActionUse1 UMETA(DisplayName="Use (1)"),
	/** Use an interactive object attached to the control. */
	ActionUse2 UMETA(DisplayName="Use (2)"),
	/** Use an interactive object attached to the control. */
	ActionUse3 UMETA(DisplayName="Use (3)"),
};

/** Type of the input (activate or use) axis to send from a control to a target. */
UENUM(BlueprintType)
enum class EInteractionInputAxisType : uint8 {
	AxisX UMETA(DisplayName="X"),
	AxisY UMETA(DisplayName="Y"),
	AxisZ UMETA(DisplayName="Z"),
};

UENUM(BlueprintType)
enum class EInteractionControlMode : uint8 {
	Unknown UMETA(Hidden),
	PawnFP,
	PawnTP,
	PawnVR,
	PawnMobile,
	Controller,
	Pawn,
	MotionController,
};

UENUM(BlueprintType)
enum class EInteractionPlatform : uint8 {
	Unknown UMETA(Hidden, DisplayName="Unknown"),
	Windows UMETA(DisplayName="Windows"),
	Mac UMETA(DisplayName="Mac"),
	Android UMETA(DisplayName="Android"),
	IOS UMETA(DisplayName="IOS"),
};

UENUM()
enum class EInteractionReplication : uint8 {
	None UMETA(DisplayName="None"),

	// Server only
	Server UMETA(DisplayName="Server"),
	
	// Server, Owner
	Owner UMETA(DisplayName="Owner"),

	// Server, Owner, Clients
	Multicast UMETA(DisplayName="Multicast")
};
