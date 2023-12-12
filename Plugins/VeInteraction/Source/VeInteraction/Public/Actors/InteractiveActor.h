// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "Components/InteractiveTargetComponent.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Interactive.h"
#include "InteractiveActor.generated.h"

/** Base class for interactive actors that use subject/object interaction system. Includes optional reset functionality. */
UCLASS(Blueprintable, BlueprintType, Category="Metaverse Interaction")
class VEINTERACTION_API AInteractiveActor : public AActor {
	GENERATED_BODY()

public:
	explicit AInteractiveActor();

	/** Returns the properties used for network replication, this needs to be overridden by all actor classes with native replicated properties */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Provides interaction options and events. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Metaverse Interaction")
	UInteractionObjectComponent* InteractionObjectComponent;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaSeconds) override;

#pragma region Resettable
	/** The actor can reset. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Metaverse Interaction|Reset")
	bool bUseReset;

	/** Reset when player releases grip on the object. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Metaverse Interaction|Reset")
	bool bUseResetOnDetach;

	/** Should the reset timer be used. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Metaverse Interaction|Reset")
	bool bUseResetTimer;

	/** How long the object can idle before reset. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Metaverse Interaction|Reset")
	float ResetTimerDelay;

	FTimerHandle ResetTimer;

	/** Should object be reset if falling below the threshold. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Metaverse Interaction|Reset")
	bool bUseResetZ;

	/** The height threshold to reset the object. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Metaverse Interaction|Reset")
	float ResetZ;

	/** Should the object be reset if moved too far from the original position. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Metaverse Interaction|Reset")
	bool bUseResetDistance;

	/** The maximum distance the object allowed to move. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Metaverse Interaction|Reset")
	float ResetDistance;

	/** Transform to place object at reset. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Metaverse Interaction|Reset")
	FTransform OriginalTransform;

	/** Should the reset triggers be used. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Metaverse Interaction|Reset")
	bool bUseResetTriggers;

	/** Triggers that reset object when overlapped. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Metaverse Interaction|Reset")
	TArray<AActor*> ResetBeginOverlapTriggers;

	/** Triggers that reset object when object leaves. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Metaverse Interaction|Reset")
	TArray<AActor*> ResetEndOverlapTriggers;

	UFUNCTION(BlueprintCallable)
	virtual void OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION(BlueprintCallable)
	virtual void OnEndOverlap(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION(BlueprintNativeEvent, Category="Metaverse Interaction|Reset")
	void OnDetached(UInteractiveControlComponent* ControlComponent);

	/** How long to wait for fade in animation before resetting the object. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Metaverse Interaction|Reset")
	float ResetFadeInDelay = 0.0f;

	/** How long to wait for fade out animation before resetting the object. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Metaverse Interaction|Reset")
	float ResetFadeOutDelay = 1.0f;

	/** Play fade in animation. */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Metaverse Interaction|Reset")
	void FadeIn();

	/** Play fade in animation. */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Metaverse Interaction|Reset")
	void AnimateFadeIn();

	/** Play fade out animation. */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Metaverse Interaction|Reset")
	void FadeOut();

	/** Play fade out animation. */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Metaverse Interaction|Reset")
	void AnimateFadeOut();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Metaverse Interaction|Reset")
	void ResetState();

	UFUNCTION(BlueprintCallable, Category="Metaverse Interaction|Reset")
	void RestartResetTimer();

	UFUNCTION(NetMulticast, Reliable, Category="Metaverse Interaction|Reset")
	void Multicast_ResetState(FTransform Transform);

	UFUNCTION(BlueprintNativeEvent, Category="Metaverse Interaction|Reset")
	void ServerResetState(FTransform Transform);

	UFUNCTION(BlueprintNativeEvent, Category="Metaverse Interaction|Reset")
	void ClientResetState(FTransform Transform);
#pragma endregion
};
