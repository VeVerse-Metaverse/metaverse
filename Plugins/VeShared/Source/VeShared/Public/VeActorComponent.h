// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "VeActorComponent.generated.h"


UCLASS(DefaultToInstanced, BlueprintType, abstract)
class VESHARED_API UVeActorComponent : public UActorComponent {
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UVeActorComponent();

	/** Checks if this component belongs to the locally controlled pawn or local controller. */
	bool IsLocallyControlled() const;

	/** Gets the game instance for the component */
	UGameInstance* GetGameInstance() const;
};
