// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "Character/ALSCharacter.h"
#include "InputActionValue.h"
#include "InteractiveCharacter.generated.h"

class UInputAction;
class UInputMappingContext;

/**
 * Base class for all interactive characters. Has interactive control component.
 */
UCLASS(Blueprintable)
class VEINTERACTION_API AInteractiveCharacter : public AALSCharacter {
	GENERATED_BODY()

	DECLARE_DYNAMIC_DELEGATE_OneParam(FPlayerStateDelegate, APlayerState*, InPlayerState);

	DECLARE_EVENT_OneParam(AInteractiveCharacter, FOnPlayerStateChanged, APlayerState*);

public:
	AInteractiveCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void PostInitializeComponents() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"), Category="Metaverse Interaction")
	class UInteractionSubjectComponent* InteractionSubjectComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Metaverse Interaction (Deprecated)")
	class UPhysicsHandleComponent* PhysicsHandle;

	void SetPlayerController(APlayerController* NewPlayerController);

	UFUNCTION(BlueprintPure)
	APlayerController* GetPlayerController() const { return MyPlayerController.Get(); }

protected:
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;
	virtual void OnRep_PlayerState() override;

	virtual void NativeOnPlayerControllerChanged(APlayerController* NewPlayerController, APlayerController* OldPlayerController);

	UFUNCTION(BlueprintImplementableEvent)
	void OnPlayerControllerChanged(APlayerController* NewPlayerController, APlayerController* OldPlayerController);

private:
	/** Handles of bindings for remove */
	UPROPERTY()
	TMap<const UInputAction*, uint32> ActionBindingHandles;

	UPROPERTY()
	TWeakObjectPtr<APlayerController> MyPlayerController;

#pragma region PlayerState

public:
	UFUNCTION(BlueprintCallable, Category="Metaverse")
	void OnReady_PlayerState(FPlayerStateDelegate Callback);

	FOnPlayerStateChanged OnPlayerStateChanged;

protected:
	virtual void NativeOnPlayerStateChanged(APlayerState* InPlayerState);

#pragma endregion PlayerState

#pragma region EnhancedInput

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MappingContext")
	TArray<TObjectPtr<UInputMappingContext>> GameMappingContexts;

	void BindInputActions(UInputComponent* InInputComponent, UInputMappingContext* MappingContext);
	void UnbindInputActions(UInputComponent* InInputComponent, UInputMappingContext* MappingContext);

private:
	UFUNCTION()
	void OnForwardMovementAction(const FInputActionValue& Value);

	UFUNCTION()
	void OnRightMovementAction(const FInputActionValue& Value);

	UFUNCTION()
	void OnCameraUpAction(const FInputActionValue& Value);

	UFUNCTION()
	void OnCameraRightAction(const FInputActionValue& Value);

	UFUNCTION()
	void OnJumpAction(const FInputActionValue& Value);

	UFUNCTION()
	void OnSprintAction(const FInputActionValue& Value);

	UFUNCTION()
	void OnAimAction(const FInputActionValue& Value);

	UFUNCTION()
	void OnCameraTapAction(const FInputActionValue& Value);

	UFUNCTION()
	void OnCameraHeldAction(const FInputActionValue& Value);

	UFUNCTION()
	void OnStanceAction(const FInputActionValue& Value);

	UFUNCTION()
	void OnWalkAction(const FInputActionValue& Value);

	UFUNCTION()
	void OnRagdollAction(const FInputActionValue& Value);

	UFUNCTION()
	void OnVelocityDirectionAction(const FInputActionValue& Value);

	UFUNCTION()
	void OnLookingDirectionAction(const FInputActionValue& Value);

	// Debug actions
	UFUNCTION()
	void OnDebugToggleHudAction(const FInputActionValue& Value);

	UFUNCTION()
	void OnDebugToggleDebugViewAction(const FInputActionValue& Value);

	UFUNCTION()
	void OnDebugToggleTracesAction(const FInputActionValue& Value);

	UFUNCTION()
	void OnDebugToggleShapesAction(const FInputActionValue& Value);

	UFUNCTION()
	void OnDebugToggleLayerColorsAction(const FInputActionValue& Value);

	UFUNCTION()
	void OnDebugToggleCharacterInfoAction(const FInputActionValue& Value);

	UFUNCTION()
	void OnDebugToggleSlomoAction(const FInputActionValue& Value);

	UFUNCTION()
	void OnDebugFocusedCharacterCycleAction(const FInputActionValue& Value);

	UFUNCTION()
	void OnDebugToggleMeshAction(const FInputActionValue& Value);

	UFUNCTION()
	void OnDebugOpenOverlayMenuAction(const FInputActionValue& Value);

	UFUNCTION()
	void OnDebugOverlayMenuCycleAction(const FInputActionValue& Value);

#pragma endregion EnhancedInput

};
