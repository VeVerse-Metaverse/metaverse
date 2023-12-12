// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "CogAiMessage.h"
#include "CogAiPerceptibleComponent.h"
#include "CogPerceptible.h"
#include "VeAiEnums.h"
#include "VeUserMetadata.h"
#include "CogAiNpcSettingsComponent.generated.h"

class UCogAiEmotionMappingDataAsset;
class UCogAiPersonalityDataAsset;
class UCogAiPersonalityDefinitionDataAsset;
class UCogAiActionDataAsset;

/** @brief CogAI NPC component. Used to store the settings of the character. Attached to the pawn. Can be perceived by other characters. */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class VEAI_API UCogAiNpcSettingsComponent : public UCogAiPerceptibleComponent {
	GENERATED_BODY()

public:
	UCogAiNpcSettingsComponent();

#pragma region Properties

	/** @brief Voice of the character. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	ECogAiVoice Voice = ECogAiVoice::MaleA;

	/** @brief Data asset with the list of supported actions. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TObjectPtr<UCogAiActionDataAsset> ActionDataAssetRef = nullptr;

	/** @brief Personality definition data asset, defining the personality properties that should be used. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TObjectPtr<UCogAiPersonalityDefinitionDataAsset> PersonalityDefinitionDataAssetRef = nullptr;

	/** @brief Personality data asset configuring the personality values for the character. Properties used on the character should match defined properties. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta=(EditCondition="!bOverridePersonalityProperties"))
	TObjectPtr<UCogAiPersonalityDataAsset> PersonalityDataAssetRef = nullptr;

	/** @brief Should the personality properties be overriden by the user. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool bOverridePersonalityProperties = false;

	/** @brief Overrides for personality properties of the character. Key is a property name, value is a property value: -1 for low, 0 for medium, 1 for high. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta=(UIMin="-1", UIMax="1", ClampMin="-1", ClampMax="1", EditCondition="bOverridePersonalityProperties"))
	TMap<FString, int32> PersonalityPropertiesOverride;

	/** @brief Radius of the roaming area. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float RoamRadius = 4000.0f;

	/**
	 * @brief Should the user OpenAI key be used for the character.
	 * @note Warning! This is an experimental feature. Use at your own risk.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool bUseUserKey = true;

	/**
	 * @brief User OpenAI key to use for the character.
	 * @note Warning! This is an experimental feature. Use at your own risk.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	FString UserKey = TEXT("");

	/**
	 * @brief What OpenAI model should be used for the character.
	 * @note Warning! This is an experimental feature. Use at your own risk.
	 * @note Default value is "gpt-3.5-turbo"
	 * @note Supported values are: "gpt-3.5-turbo", "gpt-4", "gpt-4-32k" (if you have it enabled) and older models.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	FString UserVersion = TEXT("gpt-3.5-turbo");

#pragma endregion

	void OnAiUserMetadataUpdated(const FVeUserMetadata& VeUserMetadata);
};
