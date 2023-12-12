// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/AudioComponent.h"
#include "GameFramework/Actor.h"
#include "Sound/ImportedSoundWave.h"
#include "CogAiMetaHuman.generated.h"

class USphereComponent;
class ACogAiMetaHumanGroomWind;
class AVeAiCharacter;

USTRUCT(BlueprintType)
struct FCogAiMetaHumanEmotionMapping {
	GENERATED_BODY()

	/** @brief The emotion values. E.g. "Joy" = 0.5, "Surprise" = 0.5. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAI")
	TMap<FName, float> EmotionValues;
};

UCLASS(BlueprintType, Blueprintable)
class VEAI_API UCogAiMetaHumanEmotionMappingDataAsset : public UDataAsset {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAI")
	TMap<FName, FCogAiMetaHumanEmotionMapping> EmotionMapping;
};

UCLASS(BlueprintType, Blueprintable)
class VEAI_API ACogAiMetaHuman : public AActor {
	GENERATED_BODY()

public:
	ACogAiMetaHuman();

protected:
	UFUNCTION()
	void OnSphereComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaSeconds) override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VeAI")
	TObjectPtr<USphereComponent> SphereComponent = nullptr;

	/** @brief Make character speak a message, including lip sync, wave will be converted to a 16-bit PCM sound data. */
	void Speak(UImportedSoundWave* InSoundWave, const FString& InMessage);

	/** @brief Set the current emotion. */
	void SetNextEmotion(const FName& InEmotion, const float InIntensity = 1.0f);

	/** @brief Callback for when the audio data is ready to be recognized and then played. */
	UFUNCTION()
	void OnSpeakAudioDataReady(bool bSucceeded, const TArray<uint8>& AudioData);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAI")
	TObjectPtr<ACogAiMetaHumanGroomWind> GroomWind = nullptr;

	/** @brief Audio component used to play TTS. Client only. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI")
	TObjectPtr<UAudioComponent> AudioComponent = nullptr;

	/** @brief The character that this meta human is attached to. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAI")
	TObjectPtr<AVeAiCharacter> Character = nullptr;

	/** @brief The current sound wave. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VeAI")
	UImportedSoundWave* CurrentSoundWave = nullptr;

	/** @brief The current message. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VeAI")
	FString CurrentMessage = "";

	/** @brief The current emotion. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VeAI")
	FName CurrentEmotion = NAME_None;

	/** @brief The current emotion. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VeAI")
	FName DefaultEmotion = TEXT("neutral");

	/** @brief The next emotion to interpolate to. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VeAI")
	FName NextEmotion = NAME_None;

	/** @brief Interpolated weight of the next emotion over the current one. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VeAI")
	float NextEmotionWeight = 0.0f;

	/** @brief Interpolated weight of the next emotion over the current one. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VeAI")
	float NextEmotionTargetWeight = 0.0f;

	/** @brief Emotion interpolation speed. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAI")
	float EmotionInterpolationSpeedMin = 2.5f;

	/** @brief Emotion interpolation speed. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAI")
	float EmotionInterpolationSpeedMax = 7.5f;

	/** @brief Emotion interpolation speed. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VeAI")
	float EmotionInterpolationSpeed = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAI")
	UCogAiMetaHumanEmotionMappingDataAsset* EmotionMapping = nullptr;

	UFUNCTION(BlueprintCallable, Category = "VeAI")
	FCogAiMetaHumanEmotionMapping GetEmotionMapping(const FName& InEmotion) const;

	FOnAudioFinishedNative OnAudioFinishedNative;
	FDelegateHandle SayAudioFinishedHandle;

#pragma region Emotions

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAI")
	float Sternness = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAI")
	float Indignation = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAI")
	float Anger = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAI")
	float Rage = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAI")
	float Disdain = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAI")
	float Aversion = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAI")
	float Disgust = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAI")
	float Revulsion = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAI")
	float Concern = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAI")
	float Anxiety = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAI")
	float Fear = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAI")
	float Terror = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAI")
	float Satisfaction = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAI")
	float Amusement = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAI")
	float Joy = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAI")
	float Laughter = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAI")
	float Dejection = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAI")
	float Melancholy = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAI")
	float Sadness = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAI")
	float Grief = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAI")
	float Alertness = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAI")
	float Wonder = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAI")
	float Surprise = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAI")
	float Shock = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAI")
	float Outrage = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAI")
	float Anguish = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAI")
	float Cruelty = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAI")
	float Betrayal = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAI")
	float Appalled = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAI")
	float Horror = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAI")
	float Ewww = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAI")
	float PainEmpathy = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAI")
	float Disbelief = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAI")
	float Desperation = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAI")
	float Devastation = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAI")
	float Spooked = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAI")
	float FaintHope = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAI")
	float Amazement = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeAI")
	float Dissappointment = 0.0f;
#pragma endregion

#pragma region Say

	/** @brief If true, the character will use eye blinking to modify morph targets. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	bool bUseSay = true;

	UPROPERTY(VisibleInstanceOnly, Category="Say", meta=(AllowPrivateAccess="true"))
	bool bIsSaying = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Blinking", meta=(AllowPrivateAccess="true"))
	float SayAlpha = 0.0f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Blinking", meta=(AllowPrivateAccess="true"))
	float SayTargetAlpha = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Blinking", meta=(AllowPrivateAccess="true"))
	float SayLerpSpeed = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Blinking", meta=(AllowPrivateAccess="true"))
	float SayTargetMinValue = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Blinking", meta=(AllowPrivateAccess="true"))
	float SayTargetMaxValue = 0.33f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Blinking", meta=(AllowPrivateAccess="true"))
	float SayInterval = 0.1f;

	FTimerHandle SayTimerHandle;

	UFUNCTION(BlueprintImplementableEvent, Category="AI")
	void OnStartSay();

	UFUNCTION(BlueprintCallable, Category="AI")
	void StartSay(UImportedSoundWave* InSoundWave, const FString& InMessage);

	UFUNCTION(BlueprintImplementableEvent, Category="AI")
	void OnStopSay();

	UFUNCTION(BlueprintCallable, Category="AI")
	void StopSay();

	UFUNCTION(BlueprintCallable, Category="AI")
	void UpdateSay(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category="AI")
	void SaySetRandomTargetValue();

#pragma endregion

#pragma region Blinking

	UPROPERTY(VisibleInstanceOnly, Category="Blinking", meta=(AllowPrivateAccess="true"))
	float BlinkTimer = 0.0f;

	UPROPERTY(VisibleInstanceOnly, Category="Blinking", meta=(AllowPrivateAccess="true"))
	float CurrentBlinkInterval = 0.0f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Blinking", meta=(AllowPrivateAccess="true"))
	float BlinkAlpha = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Blinking", meta = (AllowPrivateAccess = "true"))
	float MinBlinkInterval = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Blinking", meta = (AllowPrivateAccess = "true"))
	float MaxBlinkInterval = 6.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Blinking", meta = (AllowPrivateAccess = "true"))
	float BlinkCloseSpeed = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Blinking", meta = (AllowPrivateAccess = "true"))
	float BlinkOpenSpeed = 5.0f;

	/** @brief If true, the character will use eye blinking to modify morph targets. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	bool bUseEyeBlinking = true;

	UPROPERTY(VisibleInstanceOnly, Category="Blinking", meta=(AllowPrivateAccess="true"))
	bool bIsBlinking = false;

	void StartBlinking();
	void UpdateBlinking(float DeltaTime);

#pragma endregion
};
