// Author: Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "InputTriggers.h"
#include "Enums/InteractionType.h"
#include "Components/ActorComponent.h"
#include "InteractionObjectComponent.generated.h"

class UInteractionSubjectComponent;
class UInteractionObjectComponent;
class UInputAction;

/**
 * 
 */
UCLASS(BlueprintType, meta=(BlueprintSpawnableComponent))
class VEINTERACTION_API UInteractionObjectComponent : public UActorComponent {
	GENERATED_BODY()

	friend UInteractionSubjectComponent;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnInteractionFocusBegin,
		UInteractionSubjectComponent*, Subject,
		UInteractionObjectComponent*, Object,
		const FVector&, Origin,
		const FVector&, Location,
		const FVector&, Normal
		);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInteractionFocusEnd,
		UInteractionSubjectComponent*, Subject,
		UInteractionObjectComponent*, Object
		);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInteractionOverlapBegin,
		UInteractionSubjectComponent*, Subject,
		UInteractionObjectComponent*, Object
		);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInteractionOverlapEnd,
		UInteractionSubjectComponent*, Subject,
		UInteractionObjectComponent*, Object
		);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTargetEvent,
		UInteractionSubjectComponent*, Subject,
		UInteractionObjectComponent*, Object
		);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_SevenParams(FInteractionActionTriggered,
		UInteractionSubjectComponent*, Subject,
		UInteractionObjectComponent*, Object,
		ETriggerEvent, TriggerEvent,
		FVector, ActionValue,
		float, ElapsedSeconds,
		float, TriggeredSeconds,
		const UInputAction*, InputAction
		);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnInteractionObjectEvent,
		UInteractionSubjectComponent*, Subject,
		UInteractionObjectComponent*, Object,
		const FName&, Name,
		UObject*, Payload
		);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnInteractionBinaryEvent,
		UInteractionSubjectComponent*, Subject,
		UInteractionObjectComponent*, Object,
		const FName&, Name,
		const TArray<uint8>&, Payload
		);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnInteractionStringEvent,
		UInteractionSubjectComponent*, Subject,
		UInteractionObjectComponent*, Object,
		const FName&, Name,
		const FString&, Payload
		);

#pragma region Focus

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=" Interaction")
	EInteractionReplication FocusReplication = EInteractionReplication::Server;

	/** Description */
	UPROPERTY(BlueprintAssignable)
	FOnInteractionFocusBegin OnClientFocusBegin;

	/** Description */
	UPROPERTY(BlueprintAssignable)
	FOnInteractionFocusEnd OnClientFocusEnd;

	/** Description */
	UPROPERTY(BlueprintAssignable)
	FOnInteractionFocusBegin OnServerFocusBegin;

	/** Description */
	UPROPERTY(BlueprintAssignable)
	FOnInteractionFocusEnd OnServerFocusEnd;

private:
	void Server_FocusBegin(
		UInteractionSubjectComponent* Subject,
		const FVector_NetQuantize& Origin,
		const FVector_NetQuantize& Location,
		const FVector_NetQuantizeNormal& Normal
		);

	void Client_FocusBegin(
		UInteractionSubjectComponent* Subject,
		const FVector_NetQuantize& Origin,
		const FVector_NetQuantize& Location,
		const FVector_NetQuantizeNormal& Normal
		);

	void Server_FocusEnd(UInteractionSubjectComponent* Subject);

	void Client_FocusEnd(UInteractionSubjectComponent* Subject);

#pragma endregion Focus

#pragma region Overlap

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=" Interaction")
	EInteractionReplication OverlapReplication = EInteractionReplication::Server;

	UPROPERTY(BlueprintAssignable)
	FOnInteractionOverlapBegin OnClientOverlapBegin;

	UPROPERTY(BlueprintAssignable)
	FOnInteractionOverlapEnd OnClientOverlapEnd;

	UPROPERTY(BlueprintAssignable)
	FOnInteractionOverlapBegin OnServerOverlapBegin;

	UPROPERTY(BlueprintAssignable)
	FOnInteractionOverlapEnd OnServerOverlapEnd;

private:
	void Server_OverlapBegin(UInteractionSubjectComponent* Subject);

	void Client_OverlapBegin(UInteractionSubjectComponent* Subject);

	void Server_OverlapEnd(UInteractionSubjectComponent* Subject);

	void Client_OverlapEnd(UInteractionSubjectComponent* Subject);

#pragma endregion Overlap

#pragma region Target

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=" Interaction")
	EInteractionReplication TargetReplication = EInteractionReplication::Server;

	UPROPERTY(BlueprintAssignable)
	FOnTargetEvent OnClientTargetBegin;

	UPROPERTY(BlueprintAssignable)
	FOnTargetEvent OnClientTargetEnd;

	UPROPERTY(BlueprintAssignable)
	FOnTargetEvent OnServerTargetBegin;

	UPROPERTY(BlueprintAssignable)
	FOnTargetEvent OnServerTargetEnd;

private:
	void Server_TargetBegin(UInteractionSubjectComponent* Subject);

	void Client_TargetBegin(UInteractionSubjectComponent* Subject);

	void Server_TargetEnd(UInteractionSubjectComponent* Subject);

	void Client_TargetEnd(UInteractionSubjectComponent* Subject);

#pragma endregion Target

#pragma region EnhancedInput

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=" Interaction")
	EInteractionReplication InputActionReplication = EInteractionReplication::Server;

	UPROPERTY(BlueprintAssignable)
	FInteractionActionTriggered OnClientInputAction;

	UPROPERTY(BlueprintAssignable)
	FInteractionActionTriggered OnServerInputAction;

private:
	void Client_InputAction(
		UInteractionSubjectComponent* Subject,
		ETriggerEvent TriggerEvent,
		FVector ActionValue,
		float ElapsedSeconds,
		float TriggeredSeconds,
		const UInputAction* InputAction
		);

	void Server_InputAction(
		UInteractionSubjectComponent* Subject,
		ETriggerEvent TriggerEvent,
		FVector ActionValue,
		float ElapsedSeconds,
		float TriggeredSeconds,
		const UInputAction* InputAction
		);

#pragma endregion EnhancedInput

#pragma region Events

public:
	UPROPERTY(BlueprintAssignable)
	FOnInteractionBinaryEvent OnClientBinaryEvent;

	UPROPERTY(BlueprintAssignable)
	FOnInteractionObjectEvent OnClientObjectEvent;

	UPROPERTY(BlueprintAssignable)
	FOnInteractionStringEvent OnClientStringEvent;

	UPROPERTY(BlueprintAssignable)
	FOnInteractionBinaryEvent OnServerBinaryEvent;

	UPROPERTY(BlueprintAssignable)
	FOnInteractionObjectEvent OnServerObjectEvent;

	UPROPERTY(BlueprintAssignable)
	FOnInteractionStringEvent OnServerStringEvent;

private:
	void Client_Event_Binary(UInteractionSubjectComponent* Subject, const FName& Name, const TArray<uint8>& Payload);
	void Server_Event_Binary(UInteractionSubjectComponent* Subject, const FName& Name, const TArray<uint8>& Payload);

	void Client_Event_Object(UInteractionSubjectComponent* Subject, const FName& Name, UObject* Payload);
	void Server_Event_Object(UInteractionSubjectComponent* Subject, const FName& Name, UObject* Payload);

	void Server_Event_String(UInteractionSubjectComponent* Subject, const FName& Name, const FString& Payload);
	void Client_Event_String(UInteractionSubjectComponent* Subject, const FName& Name, const FString& Payload);

#pragma endregion

};
