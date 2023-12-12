// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "VeActorComponent.h"
#include "VeVRCharacterIKReplicationComponent.generated.h"

USTRUCT()
struct FVeVRCharacterIKTransform {
	GENERATED_BODY()

	FTransform HeadTransform;
	FTransform PelvisTransform;
	FTransform PelvisDirectionTransform;

	float RightHandStretch = 0.0f;
	float LeftHandStretch = 0.0f;
	float RightHandStretchCompensate = 0.0f;
	float LeftHandStretchCompensate = 0.0f;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class VEGAME_API UVeVRCharacterIKReplicationComponent final : public UVeActorComponent {
	GENERATED_BODY()

	UVeVRCharacterIKReplicationComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Request to update and send IK transforms to the server. */
	UFUNCTION()
	void Owner_SendIKTransforms();

	/** Client->Server RPC to update IK transforms. */
	UFUNCTION(Server, Reliable)
	void ServerRPC_ReceiveIKTransform(const FVeVRCharacterIKTransform& InTransforms);

	/**  */
	UPROPERTY()
	FVeVRCharacterIKTransform IKTransform;
};
