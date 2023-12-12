// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "CogAiNpcSettingsComponent.h"

UCogAiNpcSettingsComponent::UCogAiNpcSettingsComponent() {
	PrimaryComponentTick.bCanEverTick = false;
}

void UCogAiNpcSettingsComponent::OnAiUserMetadataUpdated(const FVeUserMetadata& InUserMetadata) {
	if (InUserMetadata.DefaultPersonaMetadata.Name.IsEmpty()) {
		// Use the user name if the default persona name is not set.
		PerceptibleProperties.Add(TEXT("name"), InUserMetadata.Name);
	} else {
		// Use the default persona name if it is set.
		PerceptibleProperties.Add(TEXT("name"), InUserMetadata.DefaultPersonaMetadata.Name);
	}

	if (InUserMetadata.DefaultPersonaMetadata.Description.IsEmpty()) {
		// Use the user description if the default persona description is not set.
		PerceptibleProperties.Add(TEXT("description"), InUserMetadata.Description);
	} else {
		// Use the default persona description if it is set.
		PerceptibleProperties.Add(TEXT("description"), InUserMetadata.DefaultPersonaMetadata.Description);
	}
}
