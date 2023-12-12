// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#include "Pawns/InteractiveCharacterBase.h"

#include "VeShared.h"
#include "Components/ALSDebugComponent.h"
#include "Components/InteractionSubjectComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"

AInteractiveCharacterBase::AInteractiveCharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {
	PrimaryActorTick.bCanEverTick = true;

	SetReplicates(true);
	ACharacter::SetReplicateMovement(true);

	InteractionSubjectComponent = CreateDefaultSubobject<UInteractionSubjectComponent>("InteractionSubject");

	PhysicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>("PhysicsHandle");
}
