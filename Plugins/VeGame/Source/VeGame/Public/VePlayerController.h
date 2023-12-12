// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "ArtheonWebSocketsPayload.h"
#include "GameFramework/DefaultPawn.h"
#include "InputTriggers.h"
#include "UIPlayerController.h"
#include "Sound/SampleBufferIO.h"
#include "VePlayerController.generated.h"

class UInspectComponent;
class UEditorComponent;
struct FInputActionValue;
class UInputMappingContext;
struct FVeServerMetadata;
class AALSBaseCharacter;
class UVeVoiceComponent;
class AVRHUD;
class UVeUserComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActorSpawn, AActor*, Actor);

UCLASS(Blueprintable, BlueprintType)
class VEGAME_API AVePlayerController final : public AUIPlayerController {
	GENERATED_BODY()

public:
	AVePlayerController();

	virtual void Tick(float DeltaTime) override;
	virtual void OnPossess(APawn* NewPawn) override;
	virtual void OnRep_Pawn() override;

protected:
	virtual void PostInitializeComponents() override;
	virtual void NativeOnGameStateChanged(AGameStateBase* InGameState) override;
	virtual void NativeOnPawnChanged(APawn* NewPawn, APawn* PrevPawn) override;

private:
	void Input_InspectOrbitAxis(FVector Vector);

	FVector OrbitAxes = FVector{0};
	void Input_InspectOrbitYaw(float X);
	void Input_InspectOrbitPitch(float Y);
	void Input_InspectWheelAxis(float ZoomValue);
	void Input_InspectResetView();

#pragma region Voice Capture and Speech Recognition
	UFUNCTION(BlueprintCallable, Category = "Voice")
	void Input_VoiceCapturePressed();

	UFUNCTION(BlueprintCallable, Category = "Voice")
	void Input_VoiceCaptureReleased();
#pragma endregion

	/** Handles setup of generic bindings */
	virtual void SetupInputComponent() override;

private:
	bool bIsTeleporting = false;

	/** Vivox voice state change callback. */
	void OnVivoxVoiceStateChanged(const FGuid& InUserId, bool bInVoiceDetected);

	/** Vivox voice state change callback. */
	void OnVivoxVoiceServerIdChanged(const FGuid& InServerId);

	/** Replicate speaking state to the server. */
	UFUNCTION(Server, Reliable)
	void ServerRPC_SetIsSpeaking(bool bInIsSpeaking);

	virtual void BeginPlay() override;

	/** Handles leaving online games. */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString LoginOptions;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FGuid LoginPortal;

	void Login_SetOptions(const FString& InOptions, const FGuid& InPortal) {
		LoginOptions = InOptions;
		LoginPortal = InPortal;
	}

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UEditorComponent> EditorComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInspectComponent> InspectComponent;

	void Authority_OnLogout();

	/**
	 * Notify game mode post login complete.
	 * @remark Authority only.
	 */
	// void Authority_OnLogout();

	/**
	 * Notify client about post login complete.
	 * @remark Owner client only.
	 */
	// UFUNCTION(BlueprintNativeEvent)
	// void Owner_OnLogout();

	/** Allows to spawn arbitrary actor at the server using RPC. */
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ServerRPC_SpawnActor(TSubclassOf<AActor> Class, FVector Location, FRotator Rotation);

	/** Notifies a client that a placeable actor has been placed. */
	// UFUNCTION(BlueprintCallable, Server, Reliable)
	// void Client_PlaceActor(const FPlaceableAssetMetadata& AssetMetadata, const FVector& Location, const FRotator& Rotation);

	/** Callback for the actor spawn events. */
	UPROPERTY(BlueprintAssignable)
	FOnActorSpawn OnActorSpawn;

protected:
	/**
	 * Notify client about login complete.
	 * @remark Authority only.
	 */
	// UFUNCTION(Client, Reliable)
	// void ClientRPC_OnLogout();

	virtual void SetAppMode(EAppMode InAppMode);

private:
	EAppMode BeforeMenuMode = EAppMode::None;

#pragma region Auth

protected:
	EArtheonVivoxChannelType SelectedVoiceChannel = EArtheonVivoxChannelType::Positional;

public:
	void SetSelectedVoiceChannel(const EArtheonVivoxChannelType InChannel) {
		SelectedVoiceChannel = InChannel;
	}

	EArtheonVivoxChannelType GetSelectedVoiceChannel() const {
		return SelectedVoiceChannel;
	}

private:
#pragma endregion

#pragma region UI
	FGuid GetUserId() const;
	FDelegateHandle OnHostGameDelegateHandle;
#pragma endregion

#pragma region Voice component and input handlers

public:
	virtual void PlayerTick(float DeltaTime) override;

	FDelegateHandle OnVivoxVoiceStateChangedDelegateHandle;

	bool bVoiceEnabled;

	/** Switch between positional and echo channels. */
	UFUNCTION(BlueprintCallable)
	void SwitchToEchoChannel();

	/** Starts transmitting voice data to the selected channel. */
	UFUNCTION(BlueprintCallable)
	void SwitchSpeakingState();

	/** Starts transmitting voice data to the selected channel. */
	UFUNCTION(BlueprintCallable)
	void StartSpeaking();

	/** Stops transmitting voice data to the selected channel. */
	UFUNCTION(BlueprintCallable)
	void StopSpeaking();

#pragma endregion

#pragma region Input handlers

private:
	/** Pin or unpin the main menu in VR. */
	void Input_PinVRMenu();

	/** Start voice transmission. */
	UFUNCTION(BlueprintCallable)
	void Input_StartSpeaking();

	/** Stop voice transmission. */
	UFUNCTION(BlueprintCallable)
	void Input_StopSpeaking();

	/** Toggle voice transmission. */
	void Input_ToggleSpeaking();

	/** Switch to the echo channel to test microphone. */
	void Input_SwitchToEchoChannel();

#pragma endregion

#pragma region ALS

private:
	void SetupCamera() const;

	void SetupInputs();

public:
	/** Main character reference */
	UPROPERTY(BlueprintReadOnly, Category = "ALS Player Controller")
	AALSBaseCharacter* PossessedCharacter = nullptr;

	UPROPERTY(EditAnywhere)
	float OffsetZ = 50.f;

	UPROPERTY(EditAnywhere)
	float OffsetF = -100.f;

	UPROPERTY(EditAnywhere, Category = "ALS Player Controller")
	TSubclassOf<APawn> EditorPawnClass = ADefaultPawn::StaticClass();

	UPROPERTY(BlueprintReadOnly, Category = "ALS Player Controller")
	TObjectPtr<APawn> EditorPawn;

	UPROPERTY(BlueprintReadOnly, Category = "ALS Player Controller")
	TObjectPtr<APawn> MainPawn;

#pragma endregion

#pragma region EditorMode

protected:
	bool bEditorMode = false;

private:
	bool bCloning = false;
	bool bMultiSelect = false;

	bool Input_ToggleEditMode();

	UFUNCTION(Server, Reliable)
	void Server_ToggleEditMode();

	UFUNCTION(Client, Reliable)
	void Client_EnableEditMode();

	UFUNCTION(Client, Reliable)
	void Client_DisableEditMode();

	void Authority_EnableEditMode();
	void Authority_DisableEditMode();

	void Input_CloningPressed();
	void Input_CloningReleased();
	void Input_MultiSelectPressed();
	void Input_MultiSelectReleased();

	void Input_DeselectAll();
	void Input_DestroyAll();

	void Input_SelectPressed();
	void Input_SelectReleased();

#pragma endregion EditorMode

#pragma region InspectMode

private:
	bool Input_EnableInspectMode();
	bool Input_DisableInspectMode();

	void Input_InspectPanPressed();
	void Input_InspectPanReleased();
	void Input_InspectOrbitPressed();
	void Input_InspectOrbitReleased();

#pragma endregion InspectMode

#pragma region Utility

public:
	UFUNCTION(Exec)
	void OpenWorldOnline(const FString& InSpaceId);

	UFUNCTION(Exec)
	void OpenWorldOffline(const FString& InSpaceId);

	UFUNCTION(Exec)
	void LogCurrentSpace();

	void Input_CurrentWorldInfo();

#pragma endregion

#pragma region Voice Capture and STT (Hackerman)

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SpeechToText")
	bool bIsSpeechToTextActive = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SpeechToText")
	UAudioComponent* VoiceFeedbackAudioComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SpeechToText")
	TObjectPtr<class UAudioCaptureComponent> AudioCaptureComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SpeechToText")
	UInputAction* SpeechToTextInputAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SpeechToText")
	USoundSubmix* SpeechToTextSoundSubmix;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SpeechToText")
	bool bVoiceFeedbackEnabled = false;

	TUniquePtr<Audio::FAudioRecordingData> VoiceCaptureRecordingData;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SpeechToText")
	bool bIsVoiceCaptureActive = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SpeechToText")
	float VoiceCaptureMaxDuration = 15.0f;

	FTimerHandle VoiceCaptureTimeoutTimerHandle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SpeechToText")
	bool bIsSpeechRecognitionBusy = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SpeechToText")
	float SpeechRecognitionTimeout = 10.0f;

	FTimerHandle SpeechRecognitionTimeoutTimerHandle;

	DECLARE_EVENT_TwoParams(AVeCharacter, FOnSpeechRecognitionCompletedEvent, const APlayerState* /*Player*/, const FString& /*Result*/);

	FOnSpeechRecognitionCompletedEvent OnSpeechRecognitionCompletedEvent;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSpeechRecognitionCompleted, const APlayerState*, Player, const FString&, Result);

	UPROPERTY(BlueprintAssignable, Category="SpeechToText")
	FOnSpeechRecognitionCompleted OnSpeechRecognitionCompleted;

protected:
	void OnServerInputActionCallback(class UInteractionSubjectComponent* InInteractionSubjectComponent, class UInteractionObjectComponent* InInteractionObjectComponent, ETriggerEvent TriggerEvent, UE::Math::TVector<double> Vector, float X,
		float Arg, const UInputAction* InputAction);

	UFUNCTION()
	void OnSubmixRecordedFileDone(const USoundWave* ResultingSoundWave);

	UFUNCTION(Server, Reliable)
	void Server_OnSpeechRecognitionCompleted(const FString& Result);

	void StartVoiceCapture();
	void FinishVoiceCapture();

	FString VoiceCaptureOutputDir;
	FString VoiceCaptureOutputFileName;
#pragma endregion

#pragma region AI
	/**
	 * @brief Cached handle to the player state delegate.
	 */
	FDelegateHandle SpeechRecognizedEvent;

	void OnSpeechRecognized(const FString& String);
#pragma endregion

};
