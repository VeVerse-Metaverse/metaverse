// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "Enums/InteractionType.h"
#include "UObject/Interface.h"
#include "Interactive.generated.h"

class UInteractiveControlComponent;
class UInteractiveTargetComponent;
class UInteractionSubjectComponent;
class UInteractionObjectComponent;
class AActor;

/** The interface for interactive objects which receive events from the interaction control. Object must have an interaction target component attached as well. */
UINTERFACE(MinimalAPI)
class UInteractive : public UInterface {
	GENERATED_BODY()
};

/** The interface for interactive objects which receive events from the interaction control. Object must have an interaction target component attached as well. */
class VEINTERACTION_API IInteractive {
	GENERATED_BODY()

public:
#pragma region New API

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void ServerFocusBegin(UInteractionSubjectComponent* Subject,
		UInteractionObjectComponent* Object,
		const FVector& Origin,
		const FVector& Location,
		const FVector& Normal);

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void ClientFocusBegin(UInteractionSubjectComponent* Subject,
		UInteractionObjectComponent* Object,
		const FVector& Origin,
		const FVector& Location,
		const FVector& Normal);

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void ServerFocusEnd(UInteractionSubjectComponent* Subject,
		UInteractionObjectComponent* Object);

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void ClientFocusEnd(UInteractionSubjectComponent* Subject,
		UInteractionObjectComponent* Object);

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void ServerOverlapBegin(UInteractionSubjectComponent* Subject,
		UInteractionObjectComponent* Object,
		const FVector& Origin,
		const FVector& Location,
		const FVector& Normal);

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void ClientOverlapBegin(UInteractionSubjectComponent* Subject,
		UInteractionObjectComponent* Object,
		const FVector& Origin,
		const FVector& Location,
		const FVector& Normal);

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void ServerOverlapEnd(UInteractionSubjectComponent* Subject,
		UInteractionObjectComponent* Object);

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void ClientOverlapEnd(UInteractionSubjectComponent* Subject,
		UInteractionObjectComponent* Object);

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void ServerInputBegin(UInteractionSubjectComponent* Subject,
		UInteractionObjectComponent* Object,
		const FName& Name,
		const FVector& Origin,
		const FVector& Location,
		const FVector& Normal);

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void ClientInputBegin(UInteractionSubjectComponent* Subject,
		UInteractionObjectComponent* Object,
		const FName& Name,
		const FVector& Origin,
		const FVector& Location,
		const FVector& Normal);

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void ServerInputEnd(UInteractionSubjectComponent* Subject,
		UInteractionObjectComponent* Object,
		const FName& Name,
		const FVector& Origin,
		const FVector& Location,
		const FVector& Normal);

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void ClientInputEnd(UInteractionSubjectComponent* Subject,
		UInteractionObjectComponent* Object,
		const FName& Name,
		const FVector& Origin,
		const FVector& Location,
		const FVector& Normal);

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void ServerInputAxis(UInteractionSubjectComponent* Subject,
		UInteractionObjectComponent* Object,
		const FName& Name,
		const float Value);

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void ClientInputAxis(UInteractionSubjectComponent* Subject,
		UInteractionObjectComponent* Object,
		const FName& Name,
		const float Value);

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void ServerEvent(UInteractionSubjectComponent* Subject,
		UInteractionObjectComponent* Object,
		const FName& Name,
		const TArray<uint8>& Args);

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void ClientEvent(UInteractionSubjectComponent* Subject,
		UInteractionObjectComponent* Object,
		const FName& Name,
		const TArray<uint8>& Args);

#pragma endregion

#pragma region Custom RPC Events
	UFUNCTION(BlueprintNativeEvent, Category="Interaction", meta=(DeprecatedFunction))
	void OnStandaloneCustomEvent(UInteractiveTargetComponent* TargetComponent,
		UInteractiveControlComponent* ControlComponent,
		const FString& Topic,
		const TArray<uint8>& Parameters);

	UFUNCTION(BlueprintNativeEvent, Category="Interaction", meta=(DeprecatedFunction))
	void OnServerCustomEvent(UInteractiveTargetComponent* TargetComponent,
		UInteractiveControlComponent* ControlComponent,
		const FString& Topic,
		const TArray<uint8>& Parameters);

	UFUNCTION(BlueprintNativeEvent, Category="Interaction", meta=(DeprecatedFunction))
	void OnClientCustomEvent(UInteractiveTargetComponent* TargetComponent,
		UInteractiveControlComponent* ControlComponent,
		const FString& Topic,
		const TArray<uint8>& Parameters);
#pragma endregion Custom RPC Events

#pragma region Custom Input
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void OnStandaloneCustomInputBegin(UInteractiveTargetComponent* TargetComponent,
		UInteractiveControlComponent* ControlComponent,
		FKey Key);

	/** Server event, called when server receives an input action begin. */
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void OnServerCustomInputBegin(UInteractiveTargetComponent* TargetComponent,
		UInteractiveControlComponent* ControlComponent,
		FKey Key);

	/** Client event, called when client receives an input action begin. */
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void OnClientCustomInputBegin(UInteractiveTargetComponent* TargetComponent,
		UInteractiveControlComponent* ControlComponent,
		FKey Key);

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void OnStandaloneCustomInputEnd(UInteractiveTargetComponent* TargetComponent,
		UInteractiveControlComponent* ControlComponent,
		FKey Key);

	/** Server event, called when server receives an input action end. */
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void OnServerCustomInputEnd(UInteractiveTargetComponent* TargetComponent,
		UInteractiveControlComponent* ControlComponent,
		FKey Key);

	/** Client event, called when client receives an input action end. */
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void OnClientCustomInputEnd(UInteractiveTargetComponent* TargetComponent,
		UInteractiveControlComponent* ControlComponent,
		FKey Key);

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void OnStandaloneCustomInputAxis(UInteractiveTargetComponent* TargetComponent,
		UInteractiveControlComponent* ControlComponent,
		FName Axis,
		float Value);

	/** Server event, called when server receives input on axis.  */
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void OnServerCustomInputAxis(UInteractiveTargetComponent* TargetComponent,
		UInteractiveControlComponent* ControlComponent,
		FName Axis,
		float Value);

	/** Client event, called when client receives input on axis. */
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void OnClientCustomInputAxis(UInteractiveTargetComponent* TargetComponent,
		UInteractiveControlComponent* ControlComponent,
		FName Axis,
		float Value);
#pragma endregion Custom Input

#pragma region Focus
	/** Server event, called when interaction target enters focus of the control. */
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void OnStandaloneFocusBegin(UInteractiveTargetComponent* TargetComponent,
		UInteractiveControlComponent* ControlComponent,
		FVector Origin,
		FVector Location,
		FVector Normal);

	/** Server event, called when interaction target enters focus of the control. */
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void OnServerFocusBegin(UInteractiveTargetComponent* TargetComponent,
		UInteractiveControlComponent* ControlComponent,
		FVector Origin,
		FVector Location,
		FVector Normal);

	/** Client event, called when interaction target enters focus of the control. */
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void OnClientFocusBegin(UInteractiveTargetComponent* TargetComponent,
		UInteractiveControlComponent* ControlComponent,
		FVector Origin,
		FVector Location,
		FVector Normal);

	/** Server event, called when interaction target leaves focus of the control. */
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void OnStandaloneFocusEnd(UInteractiveTargetComponent* TargetComponent,
		UInteractiveControlComponent* ControlComponent);

	/** Server event, called when interaction target leaves focus of the control. */
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void OnServerFocusEnd(UInteractiveTargetComponent* TargetComponent,
		UInteractiveControlComponent* ControlComponent);

	/** Client event, called when interaction target leaves focus of the control. */
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void OnClientFocusEnd(UInteractiveTargetComponent* TargetComponent,
		UInteractiveControlComponent* ControlComponent);
#pragma endregion

#pragma region Overlap
	/** Server event, called when interaction target starts overlapping the control. */
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void OnStandaloneOverlapBegin(UInteractiveTargetComponent* TargetComponent,
		UInteractiveControlComponent* ControlComponent,
		FVector Origin,
		FVector Location,
		FVector Normal);

	/** Server event, called when interaction target starts overlapping the control. */
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void OnServerOverlapBegin(UInteractiveTargetComponent* TargetComponent,
		UInteractiveControlComponent* ControlComponent,
		FVector Origin,
		FVector Location,
		FVector Normal);

	/** Client event, called when interaction target starts overlapping the control. */
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void OnClientOverlapBegin(UInteractiveTargetComponent* TargetComponent,
		UInteractiveControlComponent* ControlComponent,
		FVector Origin,
		FVector Location,
		FVector Normal);

	/** Server event, called when interaction target stops overlapping the control. */
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void OnStandaloneOverlapEnd(UInteractiveTargetComponent* TargetComponent,
		UInteractiveControlComponent* ControlComponent);

	/** Server event, called when interaction target stops overlapping the control. */
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void OnServerOverlapEnd(UInteractiveTargetComponent* TargetComponent,
		UInteractiveControlComponent* ControlComponent);

	/** Client event, called when interaction target stops overlapping the control. */
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void OnClientOverlapEnd(UInteractiveTargetComponent* TargetComponent,
		UInteractiveControlComponent* ControlComponent);
#pragma endregion

#pragma region Activate
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void OnStandaloneActivateBegin(UInteractiveTargetComponent* TargetComponent,
		UInteractiveControlComponent* ControlComponent,
		FVector Origin,
		FVector Location,
		FVector Normal);

	/** Server event, called when server receives an input action begin. */
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void OnServerActivateBegin(UInteractiveTargetComponent* TargetComponent,
		UInteractiveControlComponent* ControlComponent,
		FVector Origin,
		FVector Location,
		FVector Normal);

	/** Client event, called when client receives an input action begin. */
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void OnClientActivateBegin(UInteractiveTargetComponent* TargetComponent,
		UInteractiveControlComponent* ControlComponent,
		FVector Origin,
		FVector Location,
		FVector Normal);

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void OnStandaloneActivateEnd(UInteractiveTargetComponent* TargetComponent,
		UInteractiveControlComponent* ControlComponent);

	/** Server event, called when server receives an input action end. */
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void OnServerActivateEnd(UInteractiveTargetComponent* TargetComponent,
		UInteractiveControlComponent* ControlComponent);

	/** Client event, called when client receives an input action end. */
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void OnClientActivateEnd(UInteractiveTargetComponent* TargetComponent,
		UInteractiveControlComponent* ControlComponent);
#pragma endregion

#pragma region UserInput
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void OnStandaloneUserInputBegin(UInteractiveTargetComponent* TargetComponent,
		UInteractiveControlComponent* ControlComponent,
		const FName& InInputName,
		const FKey& InKey,
		FVector Origin,
		FVector Location,
		FVector Normal);

	/** Server event, called when server receives an input action begin. */
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void OnServerUserInputBegin(UInteractiveTargetComponent* TargetComponent,
		UInteractiveControlComponent* ControlComponent, const FName& InInputName, const FKey& InKey,
		FVector Origin,
		FVector Location,
		FVector Normal);

	/** Client event, called when client receives an input action begin. */
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void OnClientUserInputBegin(UInteractiveTargetComponent* TargetComponent,
		UInteractiveControlComponent* ControlComponent,
		const FName& InInputName,
		const FKey& InKey,
		FVector Origin,
		FVector Location,
		FVector Normal);

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void OnStandaloneUserInputEnd(UInteractiveTargetComponent* TargetComponent,
		UInteractiveControlComponent* ControlComponent,
		const FName& InInputName,
		const FKey& InKey);

	/** Server event, called when server receives an input action end. */
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void OnServerUserInputEnd(UInteractiveTargetComponent* TargetComponent,
		UInteractiveControlComponent* ControlComponent,
		const FName& InInputName,
		const FKey& InKey);

	/** Client event, called when client receives an input action end. */
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void OnClientUserInputEnd(UInteractiveTargetComponent* TargetComponent,
		UInteractiveControlComponent* ControlComponent,
		const FName& InInputName,
		const FKey& InKey);
#pragma endregion

#pragma region Grab
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void OnStandaloneGrabBegin(UInteractiveTargetComponent* TargetComponent,
		UInteractiveControlComponent* ControlComponent,
		FVector Origin,
		FVector Location,
		FVector Normal);

	/** Server event, called when server receives an input grab begin. */
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void OnServerGrabBegin(UInteractiveTargetComponent* TargetComponent,
		UInteractiveControlComponent* ControlComponent,
		FVector Origin,
		FVector Location,
		FVector Normal);

	/** Client event, called when client receives an input grab begin. */
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void OnClientGrabBegin(UInteractiveTargetComponent* TargetComponent,
		UInteractiveControlComponent* ControlComponent,
		FVector Origin,
		FVector Location,
		FVector Normal);

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void OnStandaloneGrabEnd(UInteractiveTargetComponent* TargetComponent,
		UInteractiveControlComponent* ControlComponent);

	/** Server event, called when server receives an input grab end. */
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void OnServerGrabEnd(UInteractiveTargetComponent* TargetComponent,
		UInteractiveControlComponent* ControlComponent);

	/** Client event, called when client receives an input grab end. */
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void OnClientGrabEnd(UInteractiveTargetComponent* TargetComponent,
		UInteractiveControlComponent* ControlComponent);

#pragma endregion

#pragma region Attach/Detach

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void OnStandaloneAttach(UInteractiveTargetComponent* TargetComponent,
		UInteractiveControlComponent* ControlComponent);

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void OnServerAttach(UInteractiveTargetComponent* TargetComponent,
		UInteractiveControlComponent* ControlComponent);

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void OnClientAttach(UInteractiveTargetComponent* TargetComponent,
		UInteractiveControlComponent* ControlComponent);

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void OnStandaloneDetach(UInteractiveTargetComponent* TargetComponent,
		UInteractiveControlComponent* ControlComponent);

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void OnServerDetach(UInteractiveTargetComponent* TargetComponent,
		UInteractiveControlComponent* ControlComponent);

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void OnClientDetach(UInteractiveTargetComponent* TargetComponent,
		UInteractiveControlComponent* ControlComponent);

#pragma endregion

#pragma region Input Axis
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void OnStandaloneActivateAxis(UInteractiveTargetComponent* TargetComponent,
		UInteractiveControlComponent* ControlComponent,
		EInteractionInputAxisType Axis,
		float Value);

	/** Server event, called when server receives input on axis.  */
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void OnServerActivateAxis(UInteractiveTargetComponent* TargetComponent,
		UInteractiveControlComponent* ControlComponent,
		EInteractionInputAxisType Axis,
		float Value);

	/** Client event, called when client receives input on axis. */
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void OnClientActivateAxis(UInteractiveTargetComponent* TargetComponent,
		UInteractiveControlComponent* ControlComponent,
		EInteractionInputAxisType Axis,
		float Value);
#pragma endregion

#pragma region Use
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void OnStandaloneUseBegin(UInteractiveTargetComponent* TargetComponent,
		UInteractiveControlComponent* ControlComponent,
		uint8 UseKeyIndex = 0);

	/** Server event, called when server receives an input action begin. */
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void OnServerUseBegin(UInteractiveTargetComponent* TargetComponent,
		UInteractiveControlComponent* ControlComponent,
		uint8 UseKeyIndex = 0);

	/** Client event, called when client receives an input action begin. */
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void OnClientUseBegin(UInteractiveTargetComponent* TargetComponent,
		UInteractiveControlComponent* ControlComponent,
		uint8 UseKeyIndex = 0);

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void OnStandaloneUseEnd(UInteractiveTargetComponent* TargetComponent,
		UInteractiveControlComponent* ControlComponent,
		uint8 UseKeyIndex = 0);

	/** Server event, called when server receives an input action end. */
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void OnServerUseEnd(UInteractiveTargetComponent* TargetComponent,
		UInteractiveControlComponent* ControlComponent,
		uint8 UseKeyIndex = 0);

	/** Client event, called when client receives an input action end. */
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void OnClientUseEnd(UInteractiveTargetComponent* TargetComponent,
		UInteractiveControlComponent* ControlComponent,
		uint8 UseKeyIndex = 0);

#pragma endregion

#pragma region Use Axis
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void OnStandaloneUseAxis(UInteractiveTargetComponent* TargetComponent,
		UInteractiveControlComponent* ControlComponent,
		EInteractionInputAxisType Axis,
		float Value = 0.f,
		uint8 UseKeyIndex = 0);

	/** Server event, called when server receives input on axis.  */
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void OnServerUseAxis(UInteractiveTargetComponent* TargetComponent,
		UInteractiveControlComponent* ControlComponent,
		EInteractionInputAxisType Axis,
		float Value = 0.f,
		uint8 UseKeyIndex = 0);

	/** Client event, called when client receives input on axis. */
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction", meta=(DeprecatedFunction))
	void OnClientUseAxis(UInteractiveTargetComponent* TargetComponent,
		UInteractiveControlComponent* ControlComponent,
		EInteractionInputAxisType Axis,
		float Value = 0.f,
		uint8 UseKeyIndex = 0);

#pragma endregion
};
