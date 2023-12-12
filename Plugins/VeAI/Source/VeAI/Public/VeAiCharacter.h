// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "CogAiMetaHuman.h"
#include "Download1Metadata.h"
#include "VeAiEnums.h"
#include "Pawns/InteractiveCharacter.h"
#include "VeAiCharacter.generated.h"

class ACogAiMetaHuman;
class UYnnkLipsyncController;
class ACogAiFloatingThought;
class UCogAiActionMessage;
class UCogAiEmotionMappingDataAsset;
class URuntimeAudioImporterLibrary;
class UFileDownloadComponent;
class AVeAiPlayerState;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSayTtsMessageDelegate, const FString&, Message);

UCLASS()
class VEAI_API AVeAiCharacter : public AInteractiveCharacter {
	GENERATED_BODY()

	DECLARE_EVENT_OneParam(AVeAiCharacter, FOnAiPlayerStateChangedEvent, AVeAiPlayerState* /* PlayerState*/);

public:
	AVeAiCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	/**
	 * @brief If Pawn is possessed by a player, points to its Player State.  Needed for network play as controllers are not replicated to clients.
	 */
	UPROPERTY(replicatedUsing=OnRep_AiPlayerState, VisibleAnywhere, BlueprintReadOnly, Category=Pawn, meta=(AllowPrivateAccess="true"))
	TObjectPtr<AVeAiPlayerState> AiPlayerState = nullptr;

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** @brief Called when the pawn receives the player state. */
	FOnAiPlayerStateChangedEvent OnAiPlayerStateChanged;

	/** @brief Called when possessed by the AI controller, inits player state and user metadata. */
	virtual void PossessedBy(AController* NewController) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/**
	 * @brief If Pawn is possessed by a player, returns its Player State. Needed for network play as controllers are not replicated to clients.
	 */
	AVeAiPlayerState* GetAiPlayerState() const {
		return AiPlayerState;
	}

	/** @brief Templated convenience version of GetPlayerState. */
	template <class T>
	T* GetAiPlayerState() const {
		return Cast<T>(AiPlayerState);
	}

	/** @brief Callback for player state replication. */
	UFUNCTION()
	void OnRep_AiPlayerState();

	/** @brief Sets the player state, and assigns self as a current pawn to the state. */
	void SetAiPlayerState(AVeAiPlayerState* NewPlayerState);

	/** @brief Called when player state is changed. */
	virtual void OnAiPlayerStateChange();

	/** @brief User information widget displayed over remote pawns. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UAiInfoWidgetComponent> AiInfoWidgetComponent = nullptr;

	/** @brief User id, used to identify the AI character, including the persona avatar mesh. */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	FGuid UserId = {};

	/** @brief Custom persona avatar mesh URL, for cases where User ID is not used. */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	FString CustomPersonaAvatarMeshUrl = {};

#pragma region Persona

	/**
	 * @brief Current persona ID.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	FGuid CurrentPersonaId = {};

	/**
	 * @brief Current persona file URL, replicated to clients to request a new persona avatar mesh.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_CurrentPersonaFileUrl)
	FString CurrentPersonaFileUrl = {};

	/**
	 * @brief Update current persona information.
	 */
	UFUNCTION(BlueprintAuthorityOnly)
	void Authority_SetCurrentPersonaId(const FGuid& InPersonaId);

	/**
	 * @brief Update current persona file URL.
	 */
	UFUNCTION(BlueprintAuthorityOnly)
	void Authority_SetCurrentPersonaFileUrl(const FString& InPersonaFileUrl);

	/**
	 * @brief Handle current persona file URL replication.
	 */
	UFUNCTION()
	void OnRep_CurrentPersonaFileUrl();

	/**
	 * @brief Current persona URL changed event, called for any remote character.
	 */
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="On Remote Character Persona URL Changed"))
	void Remote_OnPersonaFileUrlChangedBP(const FString& InPersonaFileUrl);

	/**
	 * @brief Current persona URL changed event, called only for the owning character.
	 */
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="On Owner Character Persona URL Changed"))
	void Owner_OnPersonaFileUrlChangedBP(const FString& InPersonaFileUrl);

#pragma endregion

#pragma region MetaHuman

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	bool bUseMetaHuman = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	TSubclassOf<ACogAiMetaHuman> MetaHumanClass = ACogAiMetaHuman::StaticClass();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	TObjectPtr<ACogAiMetaHuman> MetaHuman = nullptr;

#pragma endregion

#pragma region Speech and Emotions
	/** @brief File download component used to download TTS audio. Client only. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI")
	TObjectPtr<UFileDownloadComponent> FileDownloadComponent = nullptr;

	/** @brief Audio component used to play TTS. Client only. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI")
	TObjectPtr<UAudioComponent> AudioComponent = nullptr;

	/** @brief If true, TTS is enabled and calls to SayTtsMessage will make the character speak. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	bool bIsTtsEnabled = true;

	/** @brief Currently CogAiVoice */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	ECogAiVoice CurrentCogAiVoice;

	/** @brief Currently processed TTS message or phrase to say. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category="AI")
	FString CurrentTtsMessage = {};

	/** @brief Current TTS audio file URL to play. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_CurrentTtsUrl, Category="AI")
	FString CurrentTtsUrl = {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	bool bStreamTTS = false;

	UFUNCTION()
	void OnRep_CurrentTtsUrl();

	/** @brief Current TTS audio file path to play. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI")
	FString CurrentTtsFilePath = {};

	/** @brief Currently used audio importer. */
	UPROPERTY()
	TObjectPtr<URuntimeAudioImporterLibrary> AudioImporter = nullptr;

	FDelegateHandle SayAudioFinishedHandle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Blinking", meta = (AllowPrivateAccess = "true"))
	float MinBlinkInterval = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Blinking", meta = (AllowPrivateAccess = "true"))
	float MaxBlinkInterval = 6.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Blinking", meta = (AllowPrivateAccess = "true"))
	float BlinkCloseSpeed = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Blinking", meta = (AllowPrivateAccess = "true"))
	float BlinkOpenSpeed = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Blinking", meta = (AllowPrivateAccess = "true"))
	FName BlinkLeftMorphTargetName = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Blinking", meta = (AllowPrivateAccess = "true"))
	FName BlinkRightMorphTargetName = NAME_None;

	UPROPERTY(VisibleInstanceOnly, Category="Blinking", meta=(AllowPrivateAccess="true"))
	float BlinkTimer = 0.0f;

	UPROPERTY(VisibleInstanceOnly, Category="Blinking", meta=(AllowPrivateAccess="true"))
	float CurrentBlinkInterval = 0.0f;

	UPROPERTY(VisibleInstanceOnly, Category="Blinking", meta=(AllowPrivateAccess="true"))
	float BlinkAlpha = 0.0f;

	/** @brief If true, the character will use eye blinking to modify morph targets. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	bool bUseEyeBlinking = true;

	UPROPERTY(VisibleInstanceOnly, Category="Blinking", meta=(AllowPrivateAccess="true"))
	bool bIsBlinking = false;

	void UpdateBlinking(float DeltaTime);
	void StartBlinking();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	bool bUseEyeLooking = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Eye Looking", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AActor> EyeLookingDebugTargetActor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Eye Looking", meta = (AllowPrivateAccess = "true"))
	FName LeftEyeBoneName = "LeftEye";

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Eye Looking", meta = (AllowPrivateAccess = "true"))
	FName RightEyeBoneName = "RightEye";

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Eye Looking", meta = (AllowPrivateAccess = "true"))
	FName LeftEyeLookUpMorphTargetName = "eyeLookUpLeft";
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Eye Looking", meta = (AllowPrivateAccess = "true"))
	FName LeftEyeLookDownMorphTargetName = "eyeLookDownLeft";
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Eye Looking", meta = (AllowPrivateAccess = "true"))
	FName LeftEyeLookInMorphTargetName = "eyeLookInLeft";
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Eye Looking", meta = (AllowPrivateAccess = "true"))
	FName LeftEyeLookOutMorphTargetName = "eyeLookOutLeft";

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Eye Looking", meta = (AllowPrivateAccess = "true"))
	FName RightEyeLookUpMorphTargetName = "eyeLookUpRight";
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Eye Looking", meta = (AllowPrivateAccess = "true"))
	FName RightEyeLookDownMorphTargetName = "eyeLookDownRight";
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Eye Looking", meta = (AllowPrivateAccess = "true"))
	FName RightEyeLookInMorphTargetName = "eyeLookInRight";
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Eye Looking", meta = (AllowPrivateAccess = "true"))
	FName RightEyeLookOutMorphTargetName = "eyeLookOutRight";

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Eye Looking", meta = (AllowPrivateAccess = "true"))
	float EyeLookUpDownClamp = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Eye Looking", meta = (AllowPrivateAccess = "true"))
	float EyeLookLeftRightClamp = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Eye Looking", meta = (AllowPrivateAccess = "true"))
	float EyeRandomMovementTimeMin = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Eye Looking", meta = (AllowPrivateAccess = "true"))
	float EyeRandomMovementTimeMax = 2.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Eye Looking", meta = (AllowPrivateAccess = "true"))
	float EyeRandomMovementOffsetMin = -0.1f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Eye Looking", meta = (AllowPrivateAccess = "true"))
	float EyeRandomMovementOffsetMax = 0.1f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Eye Looking", meta = (AllowPrivateAccess = "true"))
	FVector2D LeftEyeRandomOffset = FVector2D::ZeroVector;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Eye Looking", meta = (AllowPrivateAccess = "true"))
	FVector2D RightEyeRandomOffset = FVector2D::ZeroVector;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Eye Looking", meta = (AllowPrivateAccess = "true"))
	float RandomEyeMovementTimer = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Eye Looking", meta = (AllowPrivateAccess = "true"))
	bool RandomEyeMovementEnabled = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Eye Looking", meta = (AllowPrivateAccess = "true"))
	float EyeLookLerpSpeed = 5.0f;

	void UpdateEyeLooking(float DeltaTime);

	/** @brief If true, the character will use emotions to modify morph targets. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category="AI")
	bool bUseEmotions = true;

	/** @brief Emotion to morph target mapping data asset. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI")
	TObjectPtr<UCogAiEmotionMappingDataAsset> EmotionMappingDataAssetRef = nullptr;

	UFUNCTION()
	void EmoteModifyMorphTargets(float DeltaSeconds);

	/** @brief Currently processed emotion. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category="AI")
	FName CurrentEmotion = NAME_None;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="AI")
	float ElapsedEmotionTime = 0.0f;

	/** @brief Currently processed emotion intensity. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category="AI")
	float CurrentEmotionIntensity = 1.0f;

	/** @brief Emotion morph target animation speed. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	float EmotionMorphTargetSpeed = 25.0f;

	/** @brief Convert text to speech and play it using audio component. */
	UFUNCTION(BlueprintCallable, Category="AI")
	void SetEmotion(const FName& InEmotionName, float InIntensity = 1.0f);

	UFUNCTION(BlueprintImplementableEvent, Category="AI")
	void OnEmotionSet(const FName& InEmotionName, float InIntensity);

	/** @brief Convert text to speech and play it using audio component. */
	UFUNCTION(BlueprintCallable, Category="AI")
	void ResetEmotion();

	/** @brief On action taken. */
	UFUNCTION(BlueprintNativeEvent, Category="AI")
	void OnActionTaken(UCogAiActionMessage* ActionMessage);

	UFUNCTION(NetMulticast, Reliable, Category="AI")
	void Client_OnActionTaken(const FCogAiActionMessageReplicated& InActionMessageReplicated);

	UFUNCTION(BlueprintNativeEvent, Category="AI")
	void Client_OnActionTakenBP(const FCogAiActionMessageClient& ActionMessage);

	/** @brief Convert text to speech and play it using audio component. */
	UFUNCTION(BlueprintCallable, Category="AI")
	void SayTtsMessage(const FString& InText, ECogAiVoice InVoice);

	/** @brief Sets random morph target value to make the character's mouth move. */
	UFUNCTION()
	void SaySetRandomMorphTargetValue();

	/** @brief Resets morph target value to make the character's mouth stop moving. */
	UFUNCTION()
	void SayResetMorphTargetValue();

	/** @brief Modify morph target value to make the character's mouth move. */
	UFUNCTION()
	void SayModifyMorphTargets(float DeltaSeconds) const;

	/** @brief If true, TTS will use morph targets to animate the character's mouth. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	bool bUseSayMorphTargets = true;

	/** @brief Morph target animation interval. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	float SayMorphTargetTimerInterval = 0.1f;

	/** @brief Morph target animation min value. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	float SayMorphTargetMinValue = 0.0f;

	/** @brief Morph target animation max value. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	float SayMorphTargetMaxValue = 0.85f;

	/** @brief Current morph target value. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="AI")
	float SayMorphTargetValue = 0.0f;

	/** @brief Morph target animation speed. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	float SayMorphTargetSpeed = 25.0f;

	/** @brief Morph target to animate when TTS is playing. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	FName SayMorphTargetName = "mouthOpen";

	/** @brief Timer handle for morph target animation. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI")
	FTimerHandle MorphTargetTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	bool bUseFloatingThoughts = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TObjectPtr<UArrowComponent> FloatingThoughtsSpawnLocation = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool bFloatingThoughtsUseRandomLocation = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	float FloatingThoughtsSpawnOffset = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	TSubclassOf<ACogAiFloatingThought> FloatingThoughtClass = nullptr;

	UFUNCTION(BlueprintNativeEvent, Category="AI")
	void SpawnFloatingThoughts(const FString& Thoughts, const FString& Emotion = TEXT(""));

	/** @brief Called when TTS message being said. */
	UFUNCTION(BlueprintNativeEvent, meta=(DisplayName="On Say TTS Message Start"))
	void OnSayTtsMessageStart(const FString& Message);

	/** @brief Called when TTS message being said. */
	UFUNCTION(BlueprintNativeEvent, meta=(DisplayName="On Say TTS Message Finish"))
	void OnSayTtsMessageFinish();

	/** @brief Called when TTS message URL is generated. */
	UFUNCTION()
	void OnTtsUrlGenerated(const FString& InUrl, const FString& InErrorString);

	UFUNCTION()
	void OnStreamDownloadCompleted(const FString &Path, const FString &ErrorString);

	/** @brief Called when TTS message audio file is downloaded. */
	UFUNCTION()
	void OnDownloadCompleted(const FDownload1ResultMetadata& Metadata);

	/** @brief Called when TTS message file is loaded to actually play it. */
	UFUNCTION()
	void OnAudioImportComplete(URuntimeAudioImporterLibrary* InAudioImporter, UImportedSoundWave* InSoundWave, ERuntimeImportStatus InStatus);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI")
	UImportedSoundWave* CurrentSpeechSoundWave = nullptr;

	UFUNCTION()
	void OnAudioFinished();

	/** @brief Playing custom animation. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	bool bIsPlayingCustomSpeechAnimation = false;

	FTimerHandle SpeechTimerHandle;
	FTimerDelegate SpeechTimerDelegate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	float SpeechExtraDelay = 5.0f;

#pragma endregion

};
