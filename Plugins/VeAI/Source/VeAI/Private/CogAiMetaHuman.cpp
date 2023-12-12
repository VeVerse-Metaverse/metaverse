// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "CogAiMetaHuman.h"

#include "CogAiMetaHumanGroomWind.h"
#include "GroomComponent.h"
#include "RuntimeAudioImporterLibrary.h"
#include "VeAI.h"
#include "VeShared.h"
#include "Components/SphereComponent.h"

ACogAiMetaHuman::ACogAiMetaHuman() {
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("USphereComponent"));
	SphereComponent->SetupAttachment(RootComponent);
	SphereComponent->SetSphereRadius(100.0f);
	SphereComponent->SetGenerateOverlapEvents(true);

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->SetupAttachment(RootComponent);
}

void ACogAiMetaHuman::OnSphereComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (auto* GroomWindActor = Cast<ACogAiMetaHumanGroomWind>(OtherActor); IsValid(GroomWindActor)) {
		GroomWind = GroomWindActor;
	}
}

void ACogAiMetaHuman::OnSphereComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	if (const auto* GroomWindActor = Cast<ACogAiMetaHumanGroomWind>(OtherActor); IsValid(GroomWindActor)) {
		GroomWind = nullptr;
	}
}

void ACogAiMetaHuman::BeginPlay() {
	Super::BeginPlay();

	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ACogAiMetaHuman::OnSphereComponentBeginOverlap);
	SphereComponent->OnComponentEndOverlap.AddDynamic(this, &ACogAiMetaHuman::OnSphereComponentEndOverlap);

	CurrentEmotion = DefaultEmotion;
}

void ACogAiMetaHuman::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);

	SphereComponent->OnComponentBeginOverlap.RemoveDynamic(this, &ACogAiMetaHuman::OnSphereComponentBeginOverlap);
	SphereComponent->OnComponentEndOverlap.RemoveDynamic(this, &ACogAiMetaHuman::OnSphereComponentEndOverlap);
}

void ACogAiMetaHuman::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);

	if (IsValid(GroomWind)) {
		TArray<UGroomComponent*> GroomComponents;
		GetComponents<UGroomComponent>(GroomComponents);
		for (const auto* GroomComponent : GroomComponents) {
			if (GroomComponent->GetName() == TEXT("Hair")) {
				for (auto& HairGroupPhysics : GroomComponent->GroomAsset->HairGroupsPhysics) {
					HairGroupPhysics.ExternalForces.AirVelocity = GroomWind->GetWindVelocity();
				}
			}
		}
	}

	if (bUseEyeBlinking) {
		UpdateBlinking(DeltaSeconds);
	}

	if (bUseSay) {
		UpdateSay(DeltaSeconds);
	}

	if (!NextEmotion.IsNone()) {
		if (!FMath::IsNearlyEqual(NextEmotionWeight, NextEmotionTargetWeight, KINDA_SMALL_NUMBER)) {
			NextEmotionWeight = FMath::FInterpTo(NextEmotionWeight, NextEmotionTargetWeight, DeltaSeconds, EmotionInterpolationSpeed);

			if (IsValid(EmotionMapping)) {
				auto CurrentEmotionMapping = EmotionMapping->EmotionMapping.FindRef(CurrentEmotion);
				// Fall back to default emotion if the number of emotion values is invalid.
				if (CurrentEmotionMapping.EmotionValues.Num() == 0) {
					CurrentEmotionMapping = EmotionMapping->EmotionMapping.FindRef(DefaultEmotion);
				}

				const auto NextEmotionMapping = EmotionMapping->EmotionMapping.FindRef(NextEmotion);
				const auto CurrentEmotionValues = CurrentEmotionMapping.EmotionValues;
				const auto NextEmotionValues = NextEmotionMapping.EmotionValues;

				for (auto& EmotionValue : CurrentEmotionValues) {
					if (EmotionValue.Key == TEXT("Sternness")) {
						Sternness = FMath::Lerp(CurrentEmotionValues.FindRef(EmotionValue.Key), NextEmotionValues.FindRef(EmotionValue.Key), NextEmotionWeight);
					} else if (EmotionValue.Key == TEXT("Indignation")) {
						Indignation = FMath::Lerp(CurrentEmotionValues.FindRef(EmotionValue.Key), NextEmotionValues.FindRef(EmotionValue.Key), NextEmotionWeight);
					} else if (EmotionValue.Key == TEXT("Anger")) {
						Anger = FMath::Lerp(CurrentEmotionValues.FindRef(EmotionValue.Key), NextEmotionValues.FindRef(EmotionValue.Key), NextEmotionWeight);
					} else if (EmotionValue.Key == TEXT("Rage")) {
						Rage = FMath::Lerp(CurrentEmotionValues.FindRef(EmotionValue.Key), NextEmotionValues.FindRef(EmotionValue.Key), NextEmotionWeight);
					} else if (EmotionValue.Key == TEXT("Disdain")) {
						Disdain = FMath::Lerp(CurrentEmotionValues.FindRef(EmotionValue.Key), NextEmotionValues.FindRef(EmotionValue.Key), NextEmotionWeight);
					} else if (EmotionValue.Key == TEXT("Aversion")) {
						Aversion = FMath::Lerp(CurrentEmotionValues.FindRef(EmotionValue.Key), NextEmotionValues.FindRef(EmotionValue.Key), NextEmotionWeight);
					} else if (EmotionValue.Key == TEXT("Disgust")) {
						Disgust = FMath::Lerp(CurrentEmotionValues.FindRef(EmotionValue.Key), NextEmotionValues.FindRef(EmotionValue.Key), NextEmotionWeight);
					} else if (EmotionValue.Key == TEXT("Revulsion")) {
						Revulsion = FMath::Lerp(CurrentEmotionValues.FindRef(EmotionValue.Key), NextEmotionValues.FindRef(EmotionValue.Key), NextEmotionWeight);
					} else if (EmotionValue.Key == TEXT("Concern")) {
						Concern = FMath::Lerp(CurrentEmotionValues.FindRef(EmotionValue.Key), NextEmotionValues.FindRef(EmotionValue.Key), NextEmotionWeight);
					} else if (EmotionValue.Key == TEXT("Anxiety")) {
						Anxiety = FMath::Lerp(CurrentEmotionValues.FindRef(EmotionValue.Key), NextEmotionValues.FindRef(EmotionValue.Key), NextEmotionWeight);
					} else if (EmotionValue.Key == TEXT("Fear")) {
						Fear = FMath::Lerp(CurrentEmotionValues.FindRef(EmotionValue.Key), NextEmotionValues.FindRef(EmotionValue.Key), NextEmotionWeight);
					} else if (EmotionValue.Key == TEXT("Terror")) {
						Terror = FMath::Lerp(CurrentEmotionValues.FindRef(EmotionValue.Key), NextEmotionValues.FindRef(EmotionValue.Key), NextEmotionWeight);
					} else if (EmotionValue.Key == TEXT("Satisfaction")) {
						Satisfaction = FMath::Lerp(CurrentEmotionValues.FindRef(EmotionValue.Key), NextEmotionValues.FindRef(EmotionValue.Key), NextEmotionWeight);
					} else if (EmotionValue.Key == TEXT("Amusement")) {
						Amusement = FMath::Lerp(CurrentEmotionValues.FindRef(EmotionValue.Key), NextEmotionValues.FindRef(EmotionValue.Key), NextEmotionWeight);
					} else if (EmotionValue.Key == TEXT("Joy")) {
						Joy = FMath::Lerp(CurrentEmotionValues.FindRef(EmotionValue.Key), NextEmotionValues.FindRef(EmotionValue.Key), NextEmotionWeight);
					} else if (EmotionValue.Key == TEXT("Laughter")) {
						Laughter = FMath::Lerp(CurrentEmotionValues.FindRef(EmotionValue.Key), NextEmotionValues.FindRef(EmotionValue.Key), NextEmotionWeight);
					} else if (EmotionValue.Key == TEXT("Dejection")) {
						Dejection = FMath::Lerp(CurrentEmotionValues.FindRef(EmotionValue.Key), NextEmotionValues.FindRef(EmotionValue.Key), NextEmotionWeight);
					} else if (EmotionValue.Key == TEXT("Melancholy")) {
						Melancholy = FMath::Lerp(CurrentEmotionValues.FindRef(EmotionValue.Key), NextEmotionValues.FindRef(EmotionValue.Key), NextEmotionWeight);
					} else if (EmotionValue.Key == TEXT("Sadness")) {
						Sadness = FMath::Lerp(CurrentEmotionValues.FindRef(EmotionValue.Key), NextEmotionValues.FindRef(EmotionValue.Key), NextEmotionWeight);
					} else if (EmotionValue.Key == TEXT("Grief")) {
						Grief = FMath::Lerp(CurrentEmotionValues.FindRef(EmotionValue.Key), NextEmotionValues.FindRef(EmotionValue.Key), NextEmotionWeight);
					} else if (EmotionValue.Key == TEXT("Alertness")) {
						Alertness = FMath::Lerp(CurrentEmotionValues.FindRef(EmotionValue.Key), NextEmotionValues.FindRef(EmotionValue.Key), NextEmotionWeight);
					} else if (EmotionValue.Key == TEXT("Wonder")) {
						Wonder = FMath::Lerp(CurrentEmotionValues.FindRef(EmotionValue.Key), NextEmotionValues.FindRef(EmotionValue.Key), NextEmotionWeight);
					} else if (EmotionValue.Key == TEXT("Surprise")) {
						Surprise = FMath::Lerp(CurrentEmotionValues.FindRef(EmotionValue.Key), NextEmotionValues.FindRef(EmotionValue.Key), NextEmotionWeight);
					} else if (EmotionValue.Key == TEXT("Shock")) {
						Shock = FMath::Lerp(CurrentEmotionValues.FindRef(EmotionValue.Key), NextEmotionValues.FindRef(EmotionValue.Key), NextEmotionWeight);
					} else if (EmotionValue.Key == TEXT("Outrage")) {
						Outrage = FMath::Lerp(CurrentEmotionValues.FindRef(EmotionValue.Key), NextEmotionValues.FindRef(EmotionValue.Key), NextEmotionWeight);
					} else if (EmotionValue.Key == TEXT("Anguish")) {
						Anguish = FMath::Lerp(CurrentEmotionValues.FindRef(EmotionValue.Key), NextEmotionValues.FindRef(EmotionValue.Key), NextEmotionWeight);
					} else if (EmotionValue.Key == TEXT("Cruelty")) {
						Cruelty = FMath::Lerp(CurrentEmotionValues.FindRef(EmotionValue.Key), NextEmotionValues.FindRef(EmotionValue.Key), NextEmotionWeight);
					} else if (EmotionValue.Key == TEXT("Betrayal")) {
						Betrayal = FMath::Lerp(CurrentEmotionValues.FindRef(EmotionValue.Key), NextEmotionValues.FindRef(EmotionValue.Key), NextEmotionWeight);
					} else if (EmotionValue.Key == TEXT("Appalled")) {
						Appalled = FMath::Lerp(CurrentEmotionValues.FindRef(EmotionValue.Key), NextEmotionValues.FindRef(EmotionValue.Key), NextEmotionWeight);
					} else if (EmotionValue.Key == TEXT("Horror")) {
						Horror = FMath::Lerp(CurrentEmotionValues.FindRef(EmotionValue.Key), NextEmotionValues.FindRef(EmotionValue.Key), NextEmotionWeight);
					} else if (EmotionValue.Key == TEXT("Ewww")) {
						Ewww = FMath::Lerp(CurrentEmotionValues.FindRef(EmotionValue.Key), NextEmotionValues.FindRef(EmotionValue.Key), NextEmotionWeight);
					} else if (EmotionValue.Key == TEXT("PainEmpathy")) {
						PainEmpathy = FMath::Lerp(CurrentEmotionValues.FindRef(EmotionValue.Key), NextEmotionValues.FindRef(EmotionValue.Key), NextEmotionWeight);
					} else if (EmotionValue.Key == TEXT("Disbelief")) {
						Disbelief = FMath::Lerp(CurrentEmotionValues.FindRef(EmotionValue.Key), NextEmotionValues.FindRef(EmotionValue.Key), NextEmotionWeight);
					} else if (EmotionValue.Key == TEXT("Desperation")) {
						Desperation = FMath::Lerp(CurrentEmotionValues.FindRef(EmotionValue.Key), NextEmotionValues.FindRef(EmotionValue.Key), NextEmotionWeight);
					} else if (EmotionValue.Key == TEXT("Devastation")) {
						Devastation = FMath::Lerp(CurrentEmotionValues.FindRef(EmotionValue.Key), NextEmotionValues.FindRef(EmotionValue.Key), NextEmotionWeight);
					} else if (EmotionValue.Key == TEXT("Spooked")) {
						Spooked = FMath::Lerp(CurrentEmotionValues.FindRef(EmotionValue.Key), NextEmotionValues.FindRef(EmotionValue.Key), NextEmotionWeight);
					} else if (EmotionValue.Key == TEXT("FaintHope")) {
						FaintHope = FMath::Lerp(CurrentEmotionValues.FindRef(EmotionValue.Key), NextEmotionValues.FindRef(EmotionValue.Key), NextEmotionWeight);
					} else if (EmotionValue.Key == TEXT("Amazement")) {
						Amazement = FMath::Lerp(CurrentEmotionValues.FindRef(EmotionValue.Key), NextEmotionValues.FindRef(EmotionValue.Key), NextEmotionWeight);
					} else if (EmotionValue.Key == TEXT("Dissappointment")) {
						Dissappointment = FMath::Lerp(CurrentEmotionValues.FindRef(EmotionValue.Key), NextEmotionValues.FindRef(EmotionValue.Key), NextEmotionWeight);
					}
				}
			}
		} else {
			CurrentEmotion = NextEmotion;
			NextEmotion = NAME_None;
		}
	}
}

void ACogAiMetaHuman::Speak(UImportedSoundWave* InSoundWave, const FString& InMessage) {
	// FRuntimeAudioExportOverrideOptions OverrideOptions;
	// OverrideOptions.SampleRate = 24000;
	// OverrideOptions.NumOfChannels = 1;
	// FOnAudioExportToBufferResult Result;
	// Result.BindDynamic(this, &ACogAiMetaHuman::OnSpeakAudioDataReady);
	// URuntimeAudioImporterLibrary::ExportSoundWaveToBuffer(InSoundWave, ERuntimeAudioFormat::Wav, 0, OverrideOptions, Result);

	CurrentSoundWave = InSoundWave;
	CurrentMessage = InMessage;
}

void ACogAiMetaHuman::SetNextEmotion(const FName& InEmotion, const float InIntensity) {
	EmotionInterpolationSpeed = FMath::RandRange(EmotionInterpolationSpeedMin, EmotionInterpolationSpeedMax);

	if (EmotionInterpolationSpeed <= 0.f) {
		if (InEmotion.IsNone()) {
			CurrentEmotion = DefaultEmotion;
		} else {
			CurrentEmotion = InEmotion;
			NextEmotion = NAME_None;
		}
	} else {
		NextEmotionWeight = 0.f;
		NextEmotion = InEmotion;
		NextEmotionTargetWeight = FMath::Max(0.0f, InIntensity);
	}
}

void ACogAiMetaHuman::OnSpeakAudioDataReady(bool bSucceeded, const TArray<uint8>& AudioData) {
	if (!bSucceeded) {
		VeLogErrorFunc("Failed to export audio data.");
	}
}

FCogAiMetaHumanEmotionMapping ACogAiMetaHuman::GetEmotionMapping(const FName& InEmotion) const {
	if (IsValid(EmotionMapping)) {
		return EmotionMapping->EmotionMapping.FindRef(InEmotion);
	}

	return {};
}

void ACogAiMetaHuman::StartSay(UImportedSoundWave* InSoundWave, const FString& InMessage) {
	GetWorldTimerManager().SetTimer(SayTimerHandle, this, &ACogAiMetaHuman::SaySetRandomTargetValue, SayInterval, true);

	CurrentSoundWave = InSoundWave;
	CurrentMessage = InMessage;

	OnStartSay();
}

void ACogAiMetaHuman::StopSay() {
	GetWorldTimerManager().ClearTimer(SayTimerHandle);
	SayTargetAlpha = 0.0f;

	OnStopSay();

	CurrentSoundWave = nullptr;
	CurrentMessage = TEXT("");
}

void ACogAiMetaHuman::UpdateSay(float DeltaTime) {
	SayAlpha = FMath::Lerp(SayAlpha, SayTargetAlpha, DeltaTime * SayLerpSpeed);
}

void ACogAiMetaHuman::SaySetRandomTargetValue() {
	SayTargetAlpha = FMath::RandRange(SayTargetMinValue, SayTargetMaxValue);
}

void ACogAiMetaHuman::StartBlinking() {
	bIsBlinking = true;
}

void ACogAiMetaHuman::UpdateBlinking(float DeltaTime) {
	BlinkTimer += DeltaTime;

	if (!bIsBlinking && BlinkTimer >= CurrentBlinkInterval) {
		StartBlinking();
	}

	if (bIsBlinking) {
		BlinkAlpha = FMath::Clamp(BlinkAlpha + DeltaTime * BlinkCloseSpeed, 0.0f, 1.0f);
		if (BlinkAlpha >= 1.0f) {
			bIsBlinking = false;
			BlinkTimer = 0.0f;
			CurrentBlinkInterval = FMath::RandRange(MinBlinkInterval, MaxBlinkInterval);
		}
	} else {
		BlinkAlpha = FMath::Clamp(BlinkAlpha - DeltaTime * BlinkOpenSpeed, 0.0f, 1.0f);
	}
}
