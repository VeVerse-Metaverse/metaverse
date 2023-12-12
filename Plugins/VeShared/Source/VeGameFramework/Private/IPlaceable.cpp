// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "IPlaceable.h"

void IPlaceable::SetPlaceableStateHelper(EPlaceableState State, float Ratio) {
	if (PlaceableState != State || PlaceableRatio != Ratio) {
		PlaceableState = State;
		PlaceableRatio = Ratio;
		OnPlaceableStateChanged.Broadcast(State, Ratio);
	}
}
