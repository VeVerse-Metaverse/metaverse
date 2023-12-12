// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "VeAiSimpleFsmCharacter.h"

#include "VeAiPlayerState.h"
#include "VeAiSimpleFsmSubjectComponent.h"

AVeAiSimpleFsmCharacter::AVeAiSimpleFsmCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {

	SubjectComponent = CreateDefaultSubobject<UVeAiSimpleFsmSubjectComponent>("SubjectComponent");
}

void AVeAiSimpleFsmCharacter::OnAiPlayerStateChange() {
	Super::OnAiPlayerStateChange();

	if (HasAuthority()) {
		if (IsValid(AiPlayerState)) {
			AiPlayerState->GetOnAiUserMetadataUpdated().AddWeakLambda(this, [=](const FVeUserMetadata& InUserMetadata) {
				if (auto* Subject = FindComponentByClass<UVeAiSimpleFsmSubjectComponent>(); IsValid(Subject)) {
					Subject->SetId(InUserMetadata.Name);
					Subject->SetDescription(InUserMetadata.Description);
				}
			});
		}
	}
}
