// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "VeAiEnums.generated.h"

/**
 * @brief The type of the AI entity.
 */
UENUM(BlueprintType)
enum class EVeAiSimpleFsmEntityType : uint8 {
	None UMETA(DisplayName = "None"),
	Location UMETA(DisplayName = "Location"),
	Object UMETA(DisplayName = "Object (Item)"),
	NPC UMETA(DisplayName = "NPC (AI Character)"),
	Player UMETA(DisplayName = "Player (Playable Character)"),
};

/**
 * @brief The sex of the AI entity.
 */
UENUM(BlueprintType)
enum class EVeAiSimpleFsmCharacterSex : uint8 {
	None UMETA(DisplayName = "None"),
	Female UMETA(DisplayName = "Female"),
	Male UMETA(DisplayName = "Male"),
};

/**
 * @brief The voice of the AI character.
 */
UENUM(BlueprintType)
enum class ECogAiVoice : uint8 {
	FemaleC UMETA(DisplayName = "Female (Deep)"),
	FemaleE UMETA(DisplayName = "Female (Neutral)"),
	FemaleF UMETA(DisplayName = "Female (High)"),
	FemaleG UMETA(DisplayName = "Female (Intense)"),
	FemaleH UMETA(DisplayName = "Female (Bright)"),
	MaleA UMETA(DisplayName = "Male (Intense)"),
	MaleD UMETA(DisplayName = "Male (Calm)"),
	MaleI UMETA(DisplayName = "Male (Bright)"),
	MaleJ UMETA(DisplayName = "Male (Deep)"),
};

/**
 * @brief The type of the AI action.
 */
UENUM(BlueprintType)
enum class EVeAiSimpleFsmActionType : uint8 {
	None UMETA(DisplayName = "None"),
	Go UMETA(DisplayName = "Go"),
	Use UMETA(DisplayName = "Use"),
	Say UMETA(DisplayName = "Say"),
	Take UMETA(DisplayName = "Take"),
	Give UMETA(DisplayName = "Give"),
	Emote UMETA(DisplayName = "Emote"),
};
