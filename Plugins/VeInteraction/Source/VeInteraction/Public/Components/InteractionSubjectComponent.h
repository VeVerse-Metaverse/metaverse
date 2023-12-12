// Author: Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "PlayerComponent.h"
#include "Enums/InteractionType.h"
#include "InputAction.h"
#include "InteractionSubjectComponent.generated.h"

class UInputAction;
class UInputMappingContext;
class UInteractionObjectComponent;
class UInteractiveControlComponent;
class UPhysicsHandleComponent;

UENUM()
enum class EEventReplicateToClients : uint8 {
	None UMETA(DisplayName="None"),
	Owning UMETA(DisplayName="Owning Client"),
	All UMETA(DisplayName="All Clients")
};

USTRUCT(BlueprintType)
struct FInteractionObjectEventInfo {
	GENERATED_BODY()

	UPROPERTY()
	TWeakObjectPtr<UInteractionObjectComponent> Object{nullptr};

	UPROPERTY()
	FVector Origin{0};

	UPROPERTY()
	FVector Location{0};

	UPROPERTY()
	FVector Normal{0};
};

USTRUCT(BlueprintType, Blueprintable)
struct VEINTERACTION_API FInteractionEventPayload {
	GENERATED_BODY()

	virtual ~FInteractionEventPayload() = default;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Method;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Data;

	virtual TArray<uint8> Serialize();
	virtual void Deserialize(TArray<uint8> InData);
};

/** Added to player controller, pawn and motion controllers. */
UCLASS(BlueprintType, meta=(BlueprintSpawnableComponent))
class VEINTERACTION_API UInteractionSubjectComponent final : public UPlayerComponent {
	GENERATED_BODY()

public:
	explicit UInteractionSubjectComponent();
	virtual void InitializeComponent() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category="MappingContext")
	void RegisterMappingContext(UInputMappingContext* MappingContext);

	UFUNCTION(BlueprintCallable, Category="MappingContext")
	void UnregisterMappingContext(UInputMappingContext* MappingContext);

protected:
	virtual void NativeOnPlayerControllerChanged(APlayerController* NewPlayerController, APlayerController* OldPlayerController) override;

	void BindInputActions(UInputComponent* InInputComponent, UInputMappingContext* MappingContext);
	void UnbindInputActions(UInputComponent* InInputComponent, UInputMappingContext* MappingContext);

private:
	UPROPERTY()
	TSet<TObjectPtr<UInputMappingContext>> CurrentMappingContexts;

	/** Handles of bindings for remove */
	UPROPERTY()
	TMap<const UInputAction*, uint32> ActionBindingHandles;

	UPROPERTY()
	TObjectPtr<UPrimitiveComponent> TriggerComponent;

	UPROPERTY()
	FInteractionObjectEventInfo CurrentFocusedObject;

	UPROPERTY()
	FInteractionObjectEventInfo CurrentTargetedObject;

	UPROPERTY()
	TArray<TObjectPtr<UInteractionObjectComponent>> OverlappingObjects{};

	UPrimitiveComponent* FindTriggerComponent();
	FInteractionObjectEventInfo GetPawnFocusedTarget() const;
	FInteractionObjectEventInfo GetMotionControllerFocusedTarget() const;
	FInteractionObjectEventInfo GetPawnOverlappedTarget();
	FInteractionObjectEventInfo GetMotionControllerOverlappedTarget();

public:
	UInteractionObjectComponent* GetTargetObject() const {
		return CurrentTargetedObject.Object.Get();
	}

	UPROPERTY(EditAnywhere, Category = "Interaction")
	EInteractionControlMode ControlMode = EInteractionControlMode::Unknown;

	FORCEINLINE void SetControlMode(const EInteractionControlMode NewControlMode) {
		ControlMode = NewControlMode;
	}

protected:
	/** Called from the TickComponent. */
	void UpdateTarget();

	FRotator GetOwnerRotation() const;

#pragma region Owner Getters

public:
	UFUNCTION(BlueprintPure)
	AActor* GetMotionController() const;

	virtual APawn* GetPawn() const override;

	UFUNCTION(BlueprintPure)
	bool IsMotionController() const;

#pragma endregion

#pragma region Focus

protected:
	void OnFocusChanged(const FInteractionObjectEventInfo& NewObjectInfo, const FInteractionObjectEventInfo& OldObjectInfo);


	/** Called from server, received as an RPC at owning client. */
	UFUNCTION(Client, Reliable)
	void RemoteOwner_FocusBegin(
		UInteractionObjectComponent* Object,
		const FVector_NetQuantize& Origin = {},
		const FVector_NetQuantize& Location = {},
		const FVector_NetQuantizeNormal& Normal = {}
		);

	/** Called from server, received as an RPC at any client. */
	UFUNCTION(NetMulticast, Reliable)
	void RemoteMulticast_FocusBegin(
		UInteractionObjectComponent* Object,
		const FVector_NetQuantize& Origin = {},
		const FVector_NetQuantize& Location = {},
		const FVector_NetQuantizeNormal& Normal = {}
		);

	/** Called from TickComponent at server. */
	void LocalServer_FocusBegin(
		UInteractionObjectComponent* Object,
		const FVector& Origin = {},
		const FVector& Location = {},
		const FVector& Normal = {}
		);

	/** Called from TickComponent at client if focus events aren't replicated or from Client RPC focus calls. */
	void LocalClient_FocusBegin(
		UInteractionObjectComponent* Object,
		const FVector& Origin = {},
		const FVector& Location = {},
		const FVector& Normal = {}
		);


	/** Called from server, received as an RPC at owning client. */
	UFUNCTION(Client, Reliable)
	void RemoteOwner_FocusEnd(UInteractionObjectComponent* Object);

	/** Called from server, received as an RPC at any client. */
	UFUNCTION(NetMulticast, Reliable)
	void RemoteMulticast_FocusEnd(UInteractionObjectComponent* Object);

	/** Called from the TickComponent. */
	void LocalServer_FocusEnd(UInteractionObjectComponent* Object);

	/** Called from TickComponent at client if focus events aren't replicated or from Client RPC focus calls. */
	void LocalClient_FocusEnd(UInteractionObjectComponent* Object);

#pragma endregion Focus

#pragma region Overlap

protected:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


	/** Called from server, received as an RPC at owning client. */
	UFUNCTION(Client, Reliable)
	void RemoteOwner_OverlapBegin(UInteractionObjectComponent* Object);

	/** Called from server, received as an RPC at any client. */
	UFUNCTION(NetMulticast, Reliable)
	void RemoteMulticast_OverlapBegin(UInteractionObjectComponent* Object);

	/** Called from TickComponent at server. */
	void LocalServer_OverlapBegin(UInteractionObjectComponent* Object);

	/** Called from TickComponent at client if focus events aren't replicated or from Client RPC focus calls. */
	void LocalClient_OverlapBegin(UInteractionObjectComponent* Object);


	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/** Called from server, received as an RPC at owning client. */
	UFUNCTION(Client, Reliable)
	void RemoteOwner_OverlapEnd(UInteractionObjectComponent* Object);

	/** Called from server, received as an RPC at any client. */
	UFUNCTION(NetMulticast, Reliable)
	void RemoteMulticast_OverlapEnd(UInteractionObjectComponent* Object);

	void LocalServer_OverlapEnd(UInteractionObjectComponent* Object);

	void LocalClient_OverlapEnd(UInteractionObjectComponent* Object);

#pragma endregion Overlap

#pragma region Target

protected:
	void OnTargetChanged(const FInteractionObjectEventInfo& NewObjectInfo, const FInteractionObjectEventInfo& OldObjectInfo);


	/** Called from server, received as an RPC at owning client. */
	UFUNCTION(Client, Reliable)
	void RemoteOwner_TargetBegin(UInteractionObjectComponent* Object);

	/** Called from server, received as an RPC at any client. */
	UFUNCTION(NetMulticast, Reliable)
	void RemoteMulticast_TargetBegin(UInteractionObjectComponent* Object);

	void LocalServer_TargetBegin(UInteractionObjectComponent* Object);

	void LocalClient_TargetBegin(UInteractionObjectComponent* Object);


	/** Called from server, received as an RPC at owning client. */
	UFUNCTION(Client, Reliable)
	void RemoteOwner_TargetEnd(UInteractionObjectComponent* Object);

	/** Called from server, received as an RPC at any client. */
	UFUNCTION(NetMulticast, Reliable)
	void RemoteMulticast_TargetEnd(UInteractionObjectComponent* Object);

	void LocalServer_TargetEnd(UInteractionObjectComponent* Object);

	void LocalClient_TargetEnd(UInteractionObjectComponent* Object);


#pragma endregion Target

#pragma region EnhancedInput

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_SevenParams(FInteractionActionTriggered,
		UInteractionSubjectComponent*, Subject,
		UInteractionObjectComponent*, Object,
		ETriggerEvent, TriggerEvent,
		FVector, ActionValue,
		float, ElapsedSeconds,
		float, TriggeredSeconds,
		const UInputAction*, InputAction
		);

	DECLARE_MULTICAST_DELEGATE_SevenParams(FInteractionActionTriggeredNative,
		UInteractionSubjectComponent* Subject,
		UInteractionObjectComponent* Object,
		ETriggerEvent TriggerEvent,
		FVector ActionValue,
		float ElapsedSeconds,
		float TriggeredSeconds,
		const UInputAction* InputAction
	);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MappingContext")
	TArray<TObjectPtr<UInputMappingContext>> GameMappingContexts;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction")
	EInteractionReplication InputActionReplication = EInteractionReplication::Server;

	UPROPERTY(BlueprintAssignable)
	FInteractionActionTriggered OnClientInputAction;

	FInteractionActionTriggeredNative OnClientInputActionNative;

	UPROPERTY(BlueprintAssignable)
	FInteractionActionTriggered OnServerInputAction;

	FInteractionActionTriggeredNative OnServerInputActionNative;

protected:
	void OnInputActionTriggered(const struct FInputActionInstance& ActionInstance);

	UFUNCTION(Server, Reliable)
	void RemoteServer_InputAction(
		ETriggerEvent TriggerEvent,
		FVector ActionValue,
		float ElapsedSeconds,
		float TriggeredSeconds,
		const UInputAction* InputAction
		);

	UFUNCTION(Client, Reliable)
	void RemoteOwner_InputAction(
		UInteractionObjectComponent* Object,
		ETriggerEvent TriggerEvent,
		FVector ActionValue,
		float ElapsedSeconds,
		float TriggeredSeconds,
		const UInputAction* InputAction
		);

	UFUNCTION(NetMulticast, Reliable)
	void RemoteMulticast_InputAction(
		UInteractionObjectComponent* Object,
		ETriggerEvent TriggerEvent,
		FVector ActionValue,
		float ElapsedSeconds,
		float TriggeredSeconds,
		const UInputAction* InputAction
		);

	void LocalServer_InputAction(
		UInteractionObjectComponent* Object,
		ETriggerEvent TriggerEvent,
		FVector ActionValue,
		float ElapsedSeconds,
		float TriggeredSeconds,
		const UInputAction* InputAction);

	void LocalClient_InputAction(
		UInteractionObjectComponent* Object,
		ETriggerEvent TriggerEvent,
		FVector ActionValue,
		float ElapsedSeconds,
		float TriggeredSeconds,
		const UInputAction* InputAction
		);

#pragma endregion EnhancedInput

#pragma region Events - Binary

public:
	/** Called from the owning client, received as an RPC on the server. */
	UFUNCTION(BlueprintCallable, Category="Interaction", DisplayName="Remote Server Event (Binary)", meta=(AutoCreateRefTerm="Payload", AdvancedDisplay="Proxy"))
	void RemoteServer_Event_Binary_BP(UInteractionObjectComponent* Object,
		const FName Name,
		const TArray<uint8>& Payload,
		EEventReplicateToClients ReplicationMode = EEventReplicateToClients::All,
		UInteractionSubjectComponent* Proxy = nullptr
		);

	/** Called from the server, received as an RPC on clients. */
	UFUNCTION(BlueprintCallable, Category="Interaction", DisplayName="Remote Client Event (Binary)", meta=(AutoCreateRefTerm="Payload"))
	void RemoteClient_Event_Binary_BP(UInteractionObjectComponent* Object,
		const FName Name,
		const TArray<uint8>& Payload,
		bool Multicast = false
		);

protected:
	/** Called from the owning client, received as an RPC on the server. */
	UFUNCTION(Server, Reliable)
	void RemoteServer_Event_Binary(
		UInteractionObjectComponent* Object,
		const FName Name,
		const TArray<uint8>& Payload,
		EEventReplicateToClients ReplicationMode = EEventReplicateToClients::All,
		UInteractionSubjectComponent* Proxy = nullptr
		);

	/** Called from server, received as an RPC at owning client. */
	UFUNCTION(Client, Reliable)
	void RemoteOwner_Event_Binary(
		UInteractionObjectComponent* Object,
		const FName Name,
		const TArray<uint8>& Payload = {}
		);

	/** Called from server, received as an RPC at any client. */
	UFUNCTION(NetMulticast, Reliable, Category="Interaction")
	void RemoteMulticast_Event_Binary(
		UInteractionObjectComponent* Object,
		const FName Name,
		const TArray<uint8>& Payload = {}
		);

	/** Called by Remote Server event. */
	void LocalServer_Event_Binary(
		UInteractionObjectComponent* Object,
		const FName Name,
		const TArray<uint8>& Payload = {},
		EEventReplicateToClients ReplicationMode = EEventReplicateToClients::All
		);

	/** Called by Local Client event. */
	void LocalClient_Event_Binary(
		UInteractionObjectComponent* Object,
		const FName Name,
		const TArray<uint8>& Payload = {}
		);

#pragma endregion Events - Binary

#pragma region Events - String

public:
	/** Called from the owning client, received as an RPC on the server. */
	UFUNCTION(BlueprintCallable, Category="Interaction", DisplayName="Remote Server Event (String)", meta=(AdvancedDisplay="Proxy"))
	void RemoteServer_Event_String_BP(
		UInteractionObjectComponent* Object,
		const FName Name,
		const FString& Payload,
		EEventReplicateToClients ReplicationMode = EEventReplicateToClients::All,
		UInteractionSubjectComponent* Proxy = nullptr
		);

	/** Called from the server, received as an RPC on clients. */
	UFUNCTION(BlueprintCallable, Category="Interaction", DisplayName="Remote Client Event (String)")
	void RemoteClient_Event_String_BP(
		UInteractionObjectComponent* Object,
		const FName Name,
		const FString& Payload,
		bool Multicast = false
		);

protected:
	/** Called from the owning client, received as an RPC on the server. */
	UFUNCTION(Server, Reliable)
	void RemoteServer_Event_String(
		UInteractionObjectComponent* Object,
		const FName Name,
		const FString& Payload,
		EEventReplicateToClients ReplicationMode = EEventReplicateToClients::All,
		UInteractionSubjectComponent* Proxy = nullptr
		);

	/** Called from server, received as an RPC at owning client. */
	UFUNCTION(Client, Reliable)
	void RemoteOwner_Event_String(
		UInteractionObjectComponent* Object,
		const FName Name,
		const FString& Payload = {}
		);

	/** Called from server, received as an RPC at any client. */
	UFUNCTION(NetMulticast, Reliable)
	void RemoteMulticast_Event_String(
		UInteractionObjectComponent* Object,
		const FName Name,
		const FString& Payload = {}
		);

	/** Called by Remote Server event. */
	void LocalServer_Event_String(
		UInteractionObjectComponent* Object,
		const FName Name,
		const FString& Payload = {},
		EEventReplicateToClients ReplicationMode = EEventReplicateToClients::All
		);

	/** Called by Local Client event. */
	void LocalClient_Event_String(UInteractionObjectComponent* Object,
		const FName Name,
		const FString& Payload = {}
		);

#pragma endregion Events - String

#pragma region Events - Object

public:
	// Object
	/** Called from the owning client, received as an RPC on the server. */
	UFUNCTION(BlueprintCallable, Category="Interaction", DisplayName="Remote Server Event (Object)", meta=(AdvancedDisplay="Proxy"))
	void RemoteServer_Event_Object_BP(
		UInteractionObjectComponent* Object,
		const FName Name,
		UObject* Payload,
		EEventReplicateToClients ReplicationMode = EEventReplicateToClients::All,
		UInteractionSubjectComponent* Proxy = nullptr
		);

	UFUNCTION(BlueprintCallable, Category="Interaction", DisplayName="Remote Client Event (Object)")
	void RemoteClient_Event_Object_BP(
		UInteractionObjectComponent* Object,
		const FName Name,
		UObject* Payload,
		bool Multicast = false
		);

protected:
	/** Called from the owning client, received as an RPC on the server. */
	UFUNCTION(Server, Reliable)
	void RemoteServer_Event_Object(
		UInteractionObjectComponent* Object,
		const FName Name,
		const FString& ClassName,
		const TArray<uint8>& Payload,
		EEventReplicateToClients ReplicationMode = EEventReplicateToClients::All,
		UInteractionSubjectComponent* Proxy = nullptr
		);

	/** Called from server, received as an RPC at owning client. */
	UFUNCTION(Client, Reliable)
	void RemoteOwner_Event_Object(
		UInteractionObjectComponent* Object,
		const FName Name,
		const FString& ClassName,
		const TArray<uint8>& Payload = {}
		);

	/** Called from server, received as an RPC at any client. */
	UFUNCTION(NetMulticast, Reliable)
	void RemoteMulticast_Event_Object(
		UInteractionObjectComponent* Object,
		const FName Name,
		const FString& ClassName,
		const TArray<uint8>& Payload = {}
		);

	/** Called by Remote Server event. */
	void LocalServer_Event_Object(
		UInteractionObjectComponent* Object,
		const FName Name,
		const FString& ClassName,
		const TArray<uint8>& Payload = {},
		EEventReplicateToClients ReplicationMode = EEventReplicateToClients::All
		);

	/** Called by Local Client event. */
	void LocalClient_Event_Object(
		UInteractionObjectComponent* Object,
		const FName Name,
		const FString& ClassName,
		const TArray<uint8>& Payload = {}
		);

#pragma endregion Events - Object

#pragma region Options

	UPROPERTY(EditDefaultsOnly, Category = "Interaction|Options")
	float MaxFocusDistance = 500.0f;

	/** Controls minimal direction sector to select triggerable objects from: -1 is 180deg, 0 is 90deg, etc. */
	UPROPERTY(EditAnywhere, Category = "Interaction|Options")
	float MinOverlapDirection = -1.f;

#pragma endregion
};
