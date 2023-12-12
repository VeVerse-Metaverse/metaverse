// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "VeAiCharacter.h"
#include "CogAiNpcComponent.h"
#include "IPlaceable.h"
#include "CogAiCharacter.generated.h"

class UFileDownloadComponent;
class UAIPerceptionStimuliSourceComponent;


UCLASS(Blueprintable, BlueprintType)
class VEAI_API ACogAiCharacter : public AVeAiCharacter, public IPlaceable {
	GENERATED_BODY()

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void PostInitializeComponents() override;

public:
	ACogAiCharacter(const FObjectInitializer& ObjectInitializer);

	/** @brief The Cog AI NPC settings component, stores the AI related settings. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI")
	TObjectPtr<UCogAiNpcSettingsComponent> CogAiNpcSettingsComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI")
	TObjectPtr<UAIPerceptionStimuliSourceComponent> AIPerceptionStimuliSourceComponent = nullptr;

	/** @brief Subscribes for the state change to update the perceptible properties. */
	virtual void OnAiPlayerStateChange() override;

#pragma region Placeable

	void OnPlaceableMetadataUpdated(const FVePlaceableMetadata& InMetadata);

	struct FPlaceableCogAiParams {
		FString UrlAvatar = "https://models.readyplayer.me/6454efc009c2656b9c9fbdb1.glb";
		FString OpenAIKey = "";
		FString AiName = "Kaya";
		FString AiDescription = "Cute and friendly.";
		FString PersonalityTraits = "";
		FString AiVoice = "";
	};

	void SetPlaceableCogAiParams(const FPlaceableCogAiParams& InDefaultCogAiParams);

	FPlaceableCogAiParams DefaultCogAiParams;
	FPlaceableCogAiParams CurrentCogAiParams;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Placeable")
	class UPlaceableComponent* PlaceableComponent;

	UFUNCTION(BlueprintPure, Category="Placeable")
	const FVePlaceableMetadata& GetPlaceableMetadata() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Placeable")
	TSubclassOf<class UUIEditorCogAiPropertiesWidget> PlaceablePropertiesClass;

	virtual UUIPlaceablePropertiesWidget* GetPlaceablePropertiesWidget_Implementation(APlayerController* PlayerController) override;
	void Properties_OnSave(UUIEditorCogAiPropertiesWidget* PropertiesWidget);

	// void Properties_OnSave_Create(class UUIPlaceablePropertiesWidget* PropertiesWidget, const FVePlaceableMetadata& InUpdateMetadata);
	// void Properties_OnSave_Update(class UUIPlaceablePropertiesWidget* PropertiesWidget, const FVePlaceableMetadata& InUpdateMetadata);

	UFUNCTION(BlueprintCallable, Category="Placeable")
	void UpdatePlaceableOnServer();

	UFUNCTION(BlueprintCallable, Category="Placeable")
	void SetPlaceableState(EPlaceableState InState, float InRatio = 0.0f);
	void Properties_OnChanged(UUIEditorCogAiPropertiesWidget* PropertiesWidget);

#pragma endregion
};
