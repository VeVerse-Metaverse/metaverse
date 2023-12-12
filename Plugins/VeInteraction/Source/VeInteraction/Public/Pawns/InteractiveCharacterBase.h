// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "Character/ALSCharacter.h"
#include "InteractiveCharacterBase.generated.h"

class UInputAction;
class UInputMappingContext;

/**
 * Base class for all interactive characters. Has interactive control component.
 */
UCLASS(Blueprintable)
class VEINTERACTION_API AInteractiveCharacterBase : public AALSCharacter {
	GENERATED_BODY()

public:
	AInteractiveCharacterBase(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"), Category="Metaverse Interaction")
	class UInteractionSubjectComponent* InteractionSubjectComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Metaverse Interaction (Deprecated)")
	class UPhysicsHandleComponent* PhysicsHandle;

};
