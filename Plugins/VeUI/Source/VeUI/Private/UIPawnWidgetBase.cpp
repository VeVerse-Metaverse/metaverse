// Authors: Khusan T.Yadgarov, Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UIPawnWidgetBase.h"

void UUIPawnWidgetBase::SetPawn(APawn* InPawn) {
	Pawn = InPawn;
	OnPawnSet(InPawn);
}

APawn* UUIPawnWidgetBase::GetPawn() const {
	return Pawn;
}
