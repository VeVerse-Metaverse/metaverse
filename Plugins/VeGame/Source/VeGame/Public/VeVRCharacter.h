// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "ICallbackInterface.h"
#include "VeVRCharacterIKReplicationComponent.h"
#include "GameFramework/Character.h"
#include "Pawns/InteractiveVRCharacter.h"
#include "VeVRCharacter.generated.h"

class UVeUserComponent;

UCLASS()
class VEGAME_API AVeVRCharacter final : public AInteractiveVRCharacter, public ICallbackInterface {
	GENERATED_BODY()

	AVeVRCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void RegisterCallbacks() override;
	virtual void UnregisterCallbacks() override;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(VisibleAnywhere)
	UVeVRCharacterIKReplicationComponent* VRCharacterIKComponent;
	
	/** User information component. */
	// UPROPERTY(VisibleAnywhere)
	// UVeUserComponent* UserComponent;

	FDelegateHandle OnUserMetadataUpdatedDelegateHandle;

	/** User information widget displayed over remote pawns. */
	UPROPERTY(VisibleAnywhere)
	class UWidgetComponent* UserInfoWidgetComponent;

	FTimerHandle UserInfoWidgetUpdateTimer;
};
