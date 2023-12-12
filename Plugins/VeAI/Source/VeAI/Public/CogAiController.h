// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "VeAiController.h"
#include "CogAiController.generated.h"

class UCogAiNpcSettingsComponent;
class UCogAiNpcComponent;

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, HideCategories=("AI Debug"))
class VEAI_API ACogAiController : public AVeAiController {
	GENERATED_BODY()

public:
	ACogAiController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;

	/** @brief Called when the AI controller possesses a pawn. */
	virtual void OnPossess(APawn* InPawn) override;

	/** @brief Called when the AI controller stops controlling a pawn. */
	virtual void OnUnPossess() override;

	/** @brief Component that handles the AI logic. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI")
	UCogAiNpcComponent* NpcComponent = nullptr;

	/** @brief Reference to a Pawn component that provides the logic component with controlled pawn AI settings. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI")
	UCogAiNpcSettingsComponent* NpcSettingsComponentRef = nullptr;
};
