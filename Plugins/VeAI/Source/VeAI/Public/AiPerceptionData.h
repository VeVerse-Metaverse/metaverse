// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "AiPerceptionData.generated.h"

/**
 * @brief The EAiPerception enumeration is a BlueprintType UEnum that defines various categories of sensory inputs that an AI-powered character can perceive. These categories are used by the FAiPerception struct to represent the type of perception stimulus, allowing the AI neural network to process and respond to the stimulus accordingly.
 *
 * The enumeration values include:
 * - None: A hidden value to indicate no perception stimulus or an undefined category.
 * - Visual: Represents visual sensory input, such as objects or characters seen by the AI character.
 * - Audio: Represents auditory sensory input, such as sounds or voices heard by the AI character.
 * - Other: Represents other types of sensory input not covered by the Visual and Audio categories, which could include smell, taste, touch, or any custom sensory input required by the AI character.
 */
UENUM(BlueprintType)
enum class EAiPerception : uint8 {
	None UMETA(Hidden),
	Visual,
	Audio,
	Other
};

UENUM(BlueprintType)
enum class EAiTraitLevel : uint8 {
	None UMETA(Hidden),
	VeryLow,
	Low,
	Medium,
	High,
	VeryHigh
};

USTRUCT(BlueprintType)
struct FAiNpcData {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "NPC")
	FString Name = "Unknown";

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "NPC")
	FString Description = "Unknown";

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "NPC")
	EAiTraitLevel Confidence = EAiTraitLevel::Medium;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "NPC")
	EAiTraitLevel Generosity = EAiTraitLevel::Medium;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "NPC")
	EAiTraitLevel Honesty = EAiTraitLevel::Medium;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "NPC")
	EAiTraitLevel Ambition = EAiTraitLevel::Medium;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "NPC")
	EAiTraitLevel Loyalty = EAiTraitLevel::Medium;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "NPC")
	EAiTraitLevel Impulsiveness = EAiTraitLevel::Medium;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "NPC")
	EAiTraitLevel Creativity = EAiTraitLevel::Medium;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "NPC")
	EAiTraitLevel Independence = EAiTraitLevel::Medium;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "NPC")
	EAiTraitLevel Humor = EAiTraitLevel::Medium;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "NPC")
	EAiTraitLevel Skepticism = EAiTraitLevel::Medium;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "NPC")
	EAiTraitLevel Aggressiveness = EAiTraitLevel::Medium;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "NPC")
	EAiTraitLevel Hostility = EAiTraitLevel::Medium;
};

USTRUCT(BlueprintType)
struct VEAI_API FAiPerceptionContext {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	FString Description = "";

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	FString Location = "Nowhere";

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	FString Time = "Unknown";

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	FString Weather = "Unknown";

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	FString Vibe = "Neutral";
};

USTRUCT(BlueprintType)
struct FAiActionDefinitionField {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "NPC Action")
	FString Type;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "NPC Action")
	FString Name;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "NPC Action")
	FString Description;
};

USTRUCT(BlueprintType)
struct FAiActionDefinition {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "NPC Action")
	FString Type;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "NPC Action")
	TArray<FAiActionDefinitionField> Field;
};

USTRUCT(BlueprintType)
struct VEAI_API FAiPerceptionAction {
	GENERATED_BODY()

	/**
	 * @brief Type of action, e.g. "say", "use", "goto", etc.
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "NPC Action")
	FString Type;

	/**
	 * @brief Target of the action, whom to say to, to what entity to go, what entity to use, etc.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "NPC Action")
	FString Target;

	/**
	 * @brief Message to say, e.g. what to say
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "NPC Action")
	FString Message;

	/**
	 * @brief Emotion to express, e.g. smile
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "NPC Action")
	FString Emotion;

	/**
	 * @brief Thoughts to express, e.g. "Why did I say that?"
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "NPC Action")
	FString Thoughts;

	/**
	 * @brief Voice to use, whisper, say or yell
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "NPC Action")
	FString Voice;

	/**
	 * @brief Pace to use, walk, run or sprint
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "NPC Action")
	FString Pace;

	/**
	 * @brief Item to use, what item to use with the action
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "NPC Action")
	FString Item;

	/**
	 * @brief Duration to wait, in milliseconds
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "NPC Action")
	int32 Duration = 0;
};

/**
 * @brief The FAiPerception struct represents the sensory input perceived by an AI-powered character in a structured and machine-readable format. This struct is designed to be used as input for AI neural networks to process and respond to different types of stimuli detected by the AI character, such as visual, audio, or other sensory inputs.
 */
USTRUCT(BlueprintType)
struct VEAI_API FAiPerceptionData {
	GENERATED_BODY()

	/**
	 * @brief This field is of the EAiPerception enumerated type and represents the category of perception stimulus in a machine-readable format. It indicates whether the AI character perceives something visually, audibly, or through other sensory inputs. This field can be used as input to the AI neural network to determine the appropriate processing or response based on the type of perception.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	EAiPerception Type = EAiPerception::None;

	/**
	 * @brief This field represents a machine-readable identifier for the specific perception stimulus. The Name field can be used as input to the AI neural network to recognize the specific object, event, or situation being perceived by the AI character. For example, the Name field could be set to "Footstep" for an audio perception of a footstep sound or "Enemy" for a visual perception of an enemy character.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	FString Name;

	/**
	 * @brief This field provides additional context or details about the perception stimulus in a machine-readable format. The Description field can be used to store extra information about the stimulus, such as properties or attributes, that may be relevant to the AI neural network for further processing or decision-making. For example, the Description field could include "Running on wooden floor" for an audio perception of a footstep sound or "Enemy wearing red armor" for a visual perception of an enemy character.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	FString Description;
};

/**
 * @brief The FAiPerceptionEvent struct is a BlueprintType UObject that represents a specific perception event experienced by an AI-powered character. This struct combines the sensory input information from the FAiPerception struct with additional data related to the intensity, range, and direction of the perceived stimulus. It is designed to be used by AI neural networks to process and respond to various sensory events detected by the AI character.
 */
USTRUCT(BlueprintType)
struct VEAI_API FAiPerceptionEvent {
	GENERATED_BODY()

	/**
	 * @brief The perception of the AI, provides information about the type of perception stimulus, the name of the stimulus, and additional details about the stimulus.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	FAiPerceptionData Perception;

	/**
	 * @brief A float value that represents the intensity of the perception, e.g., how strong a smell is or how loud a sound is. 0.0f means no perception, 1.0f means maximum perception. Objects with lower Strength values can be ignored by the AI character.
	 */
	float StrengthValue;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	FString Strength;

	/**
	 * @brief A float value that represents the duration of the perception, e.g., how long a sound lasts or how long a visual object is visible. 0.0f means no duration, 1.0f means maximum duration. Objects with lower Duration values can be ignored by the AI character.
	 */
	float DurationValue;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	FString Duration;

	/**
	 * @brief A float value that represents the distance/range at which the AI character perceives the stimulus, e.g., how far away the character can see or hear something. Objects that are further away can be ignored by the AI character.
	 */
	float RangeValue;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	FString Range;

	/**
	 * @brief A FVector that represents the direction from which the perception stimulus is coming, e.g., the direction of a sound or a visual object. Objects that are not in the AI character's field of view can be ignored by the AI character.
	 */
	FVector DirectionValue;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	FString Direction;
};
