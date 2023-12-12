// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.


#include "VeVRCharacter.h"


AVeVRCharacter::AVeVRCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	PrimaryActorTick.bCanEverTick = true;

	VRCharacterIKComponent = CreateDefaultSubobject<UVeVRCharacterIKReplicationComponent>(FName("CharacterIKReplication"));
}

void AVeVRCharacter::RegisterCallbacks() {
}

void AVeVRCharacter::UnregisterCallbacks() {
}

void AVeVRCharacter::BeginPlay() {
	Super::BeginPlay();

	RegisterCallbacks();
}

void AVeVRCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);

	UnregisterCallbacks();
}

void AVeVRCharacter::Tick(const float DeltaTime) {
	Super::Tick(DeltaTime);

}

void AVeVRCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AVeVRCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}
