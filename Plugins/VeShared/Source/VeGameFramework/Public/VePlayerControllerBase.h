// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


#pragma once

#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "MappingContextCollection.h"
#include "VePlayerControllerBase.generated.h"

class UInputMappingContext;

UENUM()
enum class EAppMode : uint8 {
	None UMETA(Hidden),
	Game UMETA(DisplayName="Game"),
	Menu UMETA(DisplayName="Menu"),
	TextChat UMETA(DisplayName="TextChat"),
	Editor UMETA(DisplayName="Editor"),
	Inspector UMETA(DisplayName="Inspector"),
};


/**
 * 
 */
UCLASS()
class VEGAMEFRAMEWORK_API AVePlayerControllerBase : public APlayerController {
	GENERATED_BODY()

	DECLARE_DYNAMIC_DELEGATE_OneParam(FPlayerStateDelegate, APlayerState*, InPlayerState);
	DECLARE_DYNAMIC_DELEGATE_TwoParams(FPawnChangedDelegate, APawn*, NewPawn, APawn*, PrevPawn);

	DECLARE_EVENT_OneParam(AInteractiveCharacter, FOnPlayerStateChanged, APlayerState* InPlayerState);
	DECLARE_EVENT_TwoParams(AInteractiveCharacter, FOnPawnChanged, APawn* InNewPawn, APawn* InPrevPawn);

protected:
	virtual void PostInitializeComponents() override;
	virtual void SetPawn(APawn* InPawn) override;
	virtual void SetupInputComponent() override;
	virtual void OnRep_PlayerState() override;

	virtual void NativeOnGameStateChanged(AGameStateBase* InGameState) { }

	void SetPlayerControllerForPlayerComponents(AActor* InActor, APlayerController* InPlayerController);

private:
	UPROPERTY()
	TWeakObjectPtr<APawn> CurrentPawn;

#pragma region Pawn

public:
	UFUNCTION(BlueprintCallable, Category="Metaverse")
	void OnReady_Pawn(FPawnChangedDelegate Callback);

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName = "OnPawnChanged", ScriptName = "OnPawnChanged"))
	void OnPawnChanged_BP(APawn* NewPawn, APawn* PrevPawn);

	FOnPawnChanged OnPawnChanged;

protected:
	virtual void NativeOnPawnChanged(APawn* NewPawn, APawn* PrevPawn);

#pragma endregion Pawn
	
#pragma region PlayerState

public:
	UFUNCTION(BlueprintCallable, Category="Metaverse")
	void OnReady_PlayerState(FPlayerStateDelegate Callback);

	FOnPlayerStateChanged OnPlayerStateChanged;

protected:
	virtual void NativeOnPlayerStateChanged(APlayerState* InPlayerState);

#pragma endregion PlayerState

#pragma region EnhancedInput

public:
	AVePlayerControllerBase();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MappingContext")
	TArray<TObjectPtr<UInputMappingContext>> GameMappingContexts;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MappingContext")
	TArray<TObjectPtr<UInputMappingContext>> EditorMappingContexts;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MappingContext")
	TArray<TObjectPtr<UInputMappingContext>> InspectorMappingContexts;

	/** Add game MappingContext */
	UFUNCTION(BlueprintCallable, Category="MappingContext")
	void RegisterMappingContext(EAppMode InPlayerInputMode, UObject* Object, UInputMappingContext* MappingContext);

	void RegisterMappingContextArray(EAppMode InPlayerInputMode, UObject* Object, const TArray<TObjectPtr<UInputMappingContext>>& MappingContexts);

	/** Remove game MappingContext */
	UFUNCTION(BlueprintCallable, Category="MappingContext")
	void UnregisterMappingContext(EAppMode InPlayerInputMode, UObject* Object, UInputMappingContext* MappingContext);

	void UnregisterMappingContextArray(EAppMode InPlayerInputMode, UObject* Object, const TArray<TObjectPtr<UInputMappingContext>>& MappingContexts);

	/** Remove all game MappingContext by Object */
	UFUNCTION(BlueprintCallable, Category="MappingContext")
	void UnregisterMappingContextsAll(EAppMode InPlayerInputMode, UObject* Object);

	UFUNCTION(BlueprintPure)
	EAppMode GetAppMode() const {
		return AppMode;
	}

	UFUNCTION(BlueprintCallable)
	virtual void SetAppMode(EAppMode InAppMode);

protected:
	virtual void NativeOnAppModeChanged(EAppMode NewPlayerInputMode);

	UFUNCTION(BlueprintImplementableEvent)
	void OnAppModeChanged(EAppMode NewPlayerInputMode);

	void Input_ToggleInspectorMode();
	void Input_ToggleEditorMode();
	void Input_ToggleMenuMode();
	void Input_ToggleTextChatMode();

	UPROPERTY()
	TMap<EAppMode, FMappingContextCollection> MappingContextsCollections;

	UPROPERTY()
	FMappingContextCollection MenuMappingContextsCollection;

	UPROPERTY()
	FMappingContextCollection TextChatMappingContextsCollection;

	UPROPERTY()
	FMappingContextCollection EditorMappingContextsCollection;

	UPROPERTY()
	FMappingContextCollection InspectorMappingContextsCollection;

	UPROPERTY()
	FMappingContextCollection GameMappingContextsCollection;

	void BindInputActions(UInputMappingContext* MappingContext);

private:
	EAppMode AppMode = EAppMode::None;


#pragma endregion EnhancedInput

#pragma region Chat Commands

public:
	UFUNCTION()
	void ProcessChatCommand(const FString& InCommand);
#pragma endregion

#pragma region AI
	UFUNCTION(Exec)
	void SayToAI(const FString& InMessage);

	UFUNCTION(Server, Reliable)
	void Server_SayToAI(const FString& InMessage);
#pragma endregion
};
