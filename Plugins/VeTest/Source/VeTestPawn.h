// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "GameFramework/Pawn.h"
#include "VeTestPawn.generated.h"

class UVeUserComponent;
class UWidgetComponent;

UCLASS()
class VETEST_API AVeTestPawn : public APawn {
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AVeTestPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Handles setup of replicated properties. */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// UPROPERTY(VisibleAnywhere)
	// UVeUserComponent* UserComponent;

	UPROPERTY(EditAnywhere)
	UWidgetComponent* UserInfoWidgetComponent;

	FDelegateHandle OnUserMetadataUpdatedDelegateHandle;
};
