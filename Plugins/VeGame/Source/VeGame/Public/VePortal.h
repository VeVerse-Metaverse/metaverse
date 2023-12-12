// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "GameFramework/PlayerStart.h"
#include "DownloadChunkSubsystem.h"
#include "IPlaceable.h"
#include "ManagedTextureRequester.h"
#include "NiagaraComponent.h"
#include "ApiPortalMetadata.h"
#include "VePortalTypes.h"
#include "Actors/InteractiveActor.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "VePortal.generated.h"

class FApiPortalMetadata;
class FApiUpdatePortalMetadata;
class FApiServerMetadata;
class UUIPortalInfoWidget;

enum class EVeMountPakState : uint8 {
	Success,
	Failure,
	NotExist
};

USTRUCT(BlueprintType)
struct FVePortalListInfo {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGuid Id;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Name;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGuid DestinationId;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString DestinationName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString DestinationSpaceName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString DestinationMetaverseName;
};

UCLASS(Blueprintable, BlueprintType)
class VEGAME_API AVePortal : public APlayerStart, public IInteractive, public IPlaceable, public IManagedTextureRequester {
	GENERATED_BODY()

	virtual void NativeOnTextureLoaded(const FTextureLoadResult& InResult) override;

public:
	// Sets default values for this actor's properties
	AVePortal(const FObjectInitializer& ObjectInitializer);

	/** Request portal registration. */
	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void PostInitializeComponents() override;

	void SetPortalMetadata(const FApiPortalMetadata& InMetadata);

	UFUNCTION(BlueprintCallable)
	void PortalInfoWidget_OnDownloadClicked();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Portal")
	UInteractiveTargetComponent* InteractiveTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction")
	UInteractionObjectComponent* InteractionObjectComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Portal")
	USceneComponent* PortalRootComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Portal")
	USceneComponent* PortalScaleComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Placeable")
	class UPlaceableComponent* PlaceableComponent;

	// DECLARE_EVENT_OneParam(AVePortal, FOnMetadataChanged, FApiPortalMetadata Metadata);
	// FOnMetadataChanged OnPortalMetadataChanged;

#pragma region Triggers
	/** Outer trigger component to request server matching. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Portal")
	USphereComponent* OuterTriggerComponent;

	/** Inner trigger component to request player teleportation. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Portal")
	UBoxComponent* InnerTriggerComponent;
#pragma endregion

#pragma region Preview
	/** Front side destination preview. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Portal")
	UStaticMeshComponent* PreviewFrontSideComponent;

	/** Back side destination preview. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Portal")
	UStaticMeshComponent* PreviewBackSideComponent;
#pragma endregion

#pragma region Effects
	/** Widget to indicate the portal state. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Portal|Effects")
	UWidgetComponent* WidgetComponent;

	/** Class for the widget component. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Portal|Effects")
	TSubclassOf<UUIPortalInfoWidget> WidgetClass;

	/** Illuminated track to indicate the portal state. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Portal|Effects")
	UStaticMeshComponent* TrackComponent;

	/** Top inner ring. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Portal|Effects")
	UStaticMeshComponent* TopInnerRingComponent;

	/** Bottom inner ring. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Portal|Effects")
	UStaticMeshComponent* BottomInnerRingComponent;

	/** Top middle ring. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Portal|Effects")
	UStaticMeshComponent* TopMiddleRingComponent;

	/** Bottom middle ring. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Portal|Effects")
	UStaticMeshComponent* BottomMiddleRingComponent;

	/** Top outer ring. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Portal|Effects")
	UStaticMeshComponent* TopOuterRingComponent;

	/** Bottom outer ring. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Portal|Effects")
	UStaticMeshComponent* BottomOuterRingComponent;

	/** Steam particles. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Portal|Effects")
	UParticleSystemComponent* BackgroundParticleSystemComponent;

	/** Audio component to play background cue. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Portal|Effects")
	UAudioComponent* BackgroundAudioComponent;

	/** Audio component for status change effects. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Portal|Effects")
	UAudioComponent* StatusAudioComponent;

	/** Audio component for loading status change effects. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Portal|Effects")
	UAudioComponent* LoadingStatusAudioComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Portal|Effects")
	UMaterialInterface* TopInnerRingMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Portal|Effects")
	UMaterialInterface* TopMiddleRingMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Portal|Effects")
	UMaterialInterface* TopOuterRingMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Portal|Effects")
	UMaterialInterface* BottomInnerRingMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Portal|Effects")
	UMaterialInterface* BottomMiddleRingMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Portal|Effects")
	UMaterialInterface* BottomOuterRingMaterial;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Portal|Effects")
	UMaterialInstanceDynamic* TopInnerRingMID;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Portal|Effects")
	UMaterialInstanceDynamic* TopMiddleRingMID;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Portal|Effects")
	UMaterialInstanceDynamic* TopOuterRingMID;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Portal|Effects")
	UMaterialInstanceDynamic* BottomInnerRingMID;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Portal|Effects")
	UMaterialInstanceDynamic* BottomMiddleRingMID;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Portal|Effects")
	UMaterialInstanceDynamic* BottomOuterRingMID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Portal|Effects")
	FLinearColor NameTextColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Portal|Effects")
	FLinearColor DefaultStateTextColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Portal|Effects")
	FLinearColor OnlineStateTextColor = FLinearColor(0.7f, 1.0f, 0.7f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Portal|Effects")
	FLinearColor LoadingStateTextColor = FLinearColor(1.0f, 1.0f, 0.7f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Portal|Effects")
	FLinearColor WaitServerStateTextColor = FLinearColor(1.0f, 1.0f, 0.9f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Portal|Effects")
	FLinearColor ConfirmationStateTextColor = FLinearColor(0.25f, 0.25f, 1.0f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Portal|Effects")
	FLinearColor OfflineStateTextColor = FLinearColor(1.0f, 0.7f, 0.7f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Portal|Effects", meta = (ClampMin = "50.0", ClampMax = "500.0", UIMin = "50.0", UIMax = "500.0"))
	float RingBrightness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Portal|Effects")
	FLinearColor TopInnerRingColor = FLinearColor(0.0f, 0.0f, 1.0f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Portal|Effects")
	FLinearColor TopMiddleRingColor = FLinearColor(0.0f, 1.0f, 1.0f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Portal|Effects")
	FLinearColor TopOuterRingColor = FLinearColor(1.0f, 0.0f, 1.0f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Portal|Effects")
	FLinearColor BottomInnerRingColor = FLinearColor(1.0f, 0.0f, 1.0f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Portal|Effects")
	FLinearColor BottomMiddleRingColor = FLinearColor(0.0f, 0.0f, 1.0f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Portal|Effects")
	FLinearColor BottomOuterRingColor = FLinearColor(0.0f, 1.0f, 1.0f, 1.0f);

#pragma endregion

#pragma region Placeable
public:
	UFUNCTION(BlueprintPure, Category="Placeable")
	const FVePlaceableMetadata& GetPlaceableMetadata() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Placeable")
	TSubclassOf<class UUIPortalPropertiesWidget> PlaceablePropertiesClass;

	virtual UUIPlaceablePropertiesWidget* GetPlaceablePropertiesWidget_Implementation(APlayerController* PlayerController) override;

	void Properties_OnSave_Create(class UUIPlaceablePropertiesWidget* PropertiesWidget, const FApiUpdatePortalMetadata& InUpdateMetadata);
	void Properties_OnSave_Update(class UUIPlaceablePropertiesWidget* PropertiesWidget, const FApiUpdatePortalMetadata& InUpdateMetadata);

protected:
	UFUNCTION(BlueprintCallable, Category="Placeable")
	void UpdatePlaceableOnServer();

#pragma endregion

protected:
	// void RequestDownloadConfirmation();

	void RequestPreviewCubemap();

	UFUNCTION()
	void OnOuterTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnInnerTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOuterTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnInnerTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Creating and uploading a render preview*/
	void OnCubeMapRenderPreview(const FGuid& InPortalId);

	/** Creating and uploading a render preview*/
	UFUNCTION(BlueprintCallable, CallInEditor, Category="Refresh Render Target")
	void OnRefreshRenderTargetPreviewButtonClicked(const FGuid& InPortalId);

	UFUNCTION(BlueprintImplementableEvent, CallInEditor)
	UTextureRenderTargetCube* GetCubemapPreviewTarget();

	UFUNCTION(BlueprintImplementableEvent, CallInEditor)
	UTextureRenderTarget2D* GetRenderTargetPreview();

protected:
	/** Current portal state. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EVePortalState State = EVePortalState::Offline;

	UFUNCTION(BlueprintCallable, CallInEditor, Category="Portal")
	void UpdatePortalState() {
		SetPortalState(DebugPortalState);
	}

	UPROPERTY(EditInstanceOnly, Category="Portal")
	EVePortalState DebugPortalState;

	/** Portal Id for instance only */
	UPROPERTY(EditInstanceOnly)
	FGuid Id;

	FApiPortalMetadata ApiPortalMetadata;

public:
	UFUNCTION(BlueprintPure)
	const FGuid& GetId() const;

	DECLARE_EVENT_OneParam(AVePortal, FOnPortalStateChanged, EVePortalState /* InState */);

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName = "OnDownloadStateChanged", ScriptName = "OnDownloadStateChanged"), Category="Portal")
	void OnDownloadStateChanged_BP(float Ratio);

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName = "OnStateChanged", ScriptName = "OnStateChanged"), Category="Portal")
	void OnStateChanged_BP(EVePortalState InState);

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName = "OnTeleportRequested", ScriptName = "OnTeleportRequested"), Category="Portal")
	void OnTeleportRequested_BP(APawn* InPawnToTeleport);

protected:
	FOnPortalStateChanged OnPortalStateChanged;

	/** Update portal state. */
	void SetPortalState(EVePortalState InState);

	void NativeOnPortalStateChanged(EVePortalState InState);

	/** Request the portal metadata from the API. */
	void UpdatePortalMetadata();

	//==========================================================================

protected:
	void StartByStatus();

	void SetWorldItem(TSharedPtr<class FGameFrameworkWorldItem> InWorldItem);
	void WorldItem_OnStateChanged(const class FGameFrameworkWorldItem& InWorldItem);

	UFUNCTION(BlueprintCallable, Category="Placeable")
	void SetPlaceableState(EPlaceableState InState, float InRatio = 0.0f);

private:
	TWeakPtr<class FGameFrameworkWorldItem> WorldItem;
	bool bPawnOuterOverlap = false;
	bool bPawnInnerOverlap = false;
	TWeakObjectPtr<APawn> PawnToTeleport = nullptr;
	TWeakObjectPtr<class AVePlayerController> PlayerControllerToTeleport;

	void StartByStatus_DownloadPackage(const TSharedPtr<class FGameFrameworkWorldItem>& WorldItemPtr);
	void StartByStatus_MountPackage(const TSharedPtr<class FGameFrameworkWorldItem>& WorldItemPtr);
	void StartByStatus_MatchingServer(const TSharedPtr<class FGameFrameworkWorldItem>& WorldItemPtr);
	void StartByStatus_TeleportOnline(const TSharedPtr<class FGameFrameworkWorldItem>& WorldItemPtr);

};
