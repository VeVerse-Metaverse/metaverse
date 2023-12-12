// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "VeCharacter.h"
#include "VeUserMetadata.h"
#include "Components/BoxComponent.h"
#include "Components/SpotLightComponent.h"
#include "GameFramework/Character.h"
#include "VeMainCharacter.generated.h"

class UEditorComponent;
enum class EOpenSeaAssetType : uint8;
class UUIPlaceableBrowserWidget;
class FApiPersonaMetadata;

UCLASS()
class VEGAME_API AVeMainCharacter final : public AVeCharacter {
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AVeMainCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void RegisterCallbacks() override;
	virtual void UnregisterCallbacks() override;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, meta=(DisplayName="Update Persona Full Preview"))
	void BP_UpdatePersonaFullPreview();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, meta=(DisplayName="Update Persona Face Preview"))
	void BP_UpdatePersonaFacePreview();

	UFUNCTION()
	void OnLookTargetTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnLookTargetTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	FDelegateHandle OnPersonaCreateRequestDelegateHandle;
	FDelegateHandle OnPersonaSelectedDelegateHandle;
	FDelegateHandle OnUpdatePersonaFullPreviewDelegateHandle;
	FDelegateHandle OnUpdatePersonaFacePreviewDelegateHandle;

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** List of potential look targets. */
	UPROPERTY()
	TArray<AActor*> RegisteredLookTargets;

	/** Add a new look target entered range. */
	void RegisterLookTarget(AActor* InLookTarget);

	/** Remove look target that left range. */
	void UnregisterLookTarget(AActor* InLookTarget);

	/** Look target update timer handle. */
	FTimerHandle LookTargetTimerHandle;

	/** Point that character looks at. */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	AActor* LookTarget = nullptr;

	/** Component used for 3d widget interactions. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UWidgetInteractionComponent* WidgetInteractionComponent;

	/** Scene capture component for capturing persona face preview. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneCaptureComponent2D* PersonaFacePreviewCaptureComponent;

	/** Scene capture component for capturing full-body persona preview. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneCaptureComponent2D* PersonaFullPreviewCaptureComponent;

	/** Persona preview light component. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USpotLightComponent* PersonaPreviewLightComponent;

	/** Trigger to detect look targets which may attract character's attention. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBoxComponent* LookTargetTrigger;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UVeLiveLinkComponent* VeLiveLinkComponent;

	/** Used to find a new look target. */
	AActor* FindLookTarget();

	/** Update a look target */
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateLookTargetBP(AActor* InLookAtTarget);

	/** Current persona ID. */
	UPROPERTY(VisibleAnywhere, Replicated)
	FGuid CurrentPersonaId = {};

	/** Current persona file URL, replicated to clients to request a new persona avatar mesh. */
	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_CurrentPersonaFileUrl)
	FString CurrentPersonaFileUrl = {};

	/** Set current persona ID at the owning client. Called when the user selects the persona in UI. */
	UFUNCTION()
	void Owner_SetCurrentPersonaId(const FGuid& InPersonaId);

	/** Notify server from the owner that the persona ID has changed. */
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_SetCurrentPersonaId(const FGuid& InPersonaId);

	/** Update current persona information. */
	UFUNCTION(BlueprintAuthorityOnly)
	void Authority_SetCurrentPersonaId(const FGuid& InPersonaId);

	/** Update current persona file URL. */
	UFUNCTION(BlueprintAuthorityOnly)
	void Authority_SetCurrentPersonaFileUrl(const FString& InPersonaFileUrl);

	/** Handle current persona file URL replication. */
	UFUNCTION()
	void OnRep_CurrentPersonaFileUrl();

	/** Current persona URL changed event, called for any remote character. */
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="On Remote Character Persona URL Changed"))
	void Remote_OnPersonaFileUrlChangedBP(const FString& InPersonaFileUrl);

	/** Current persona URL changed event, called only for the owning character. */
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="On Owner Character Persona URL Changed"))
	void Owner_OnPersonaFileUrlChangedBP(const FString& InPersonaFileUrl);

	/** Get current persona id. */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FGuid GetCurrentPersonaId();

	/** Notify UI that the avatar has been loaded to close the loading screen in the UI. */
	UFUNCTION(BlueprintCallable)
	void NotifyPersonaLoaded(const FGuid& PersonaId);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
