// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "VeAiSimpleFsmEntityComponent.h"

UVeAiSimpleFsmEntityComponent::UVeAiSimpleFsmEntityComponent() {
	// Disable ticking as we don't need it in the base class.
	PrimaryComponentTick.bCanEverTick = false;
}
