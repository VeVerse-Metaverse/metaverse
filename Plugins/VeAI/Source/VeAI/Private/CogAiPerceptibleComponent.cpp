// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


#include "CogAiPerceptibleComponent.h"


UCogAiPerceptibleComponent::UCogAiPerceptibleComponent() {
	PrimaryComponentTick.bCanEverTick = false;
}

void UCogAiPerceptibleComponent::SetName(const FString& Name) {
	PerceptibleProperties.Add(TEXT("name"), Name);
}

void UCogAiPerceptibleComponent::SetDescription(const FString& Description) {
	PerceptibleProperties.Add(TEXT("description"), Description);
}

void UCogAiPerceptibleComponent::SetInspectDescription(const FString& InspectDescription) {
	PerceptibleProperties.Add(TEXT("inspect"), InspectDescription);
}

FString UCogAiPerceptibleComponent::GetName_Implementation() {
	if (auto* Name = PerceptibleProperties.Find(TEXT("name"))) {
		return *Name;
	}

	return "";
}

FString UCogAiPerceptibleComponent::GetDescription_Implementation() {
	if (auto* Description = PerceptibleProperties.Find(TEXT("description"))) {
		return *Description;
	}

	return "";
}

FString UCogAiPerceptibleComponent::GetInspectDescription_Implementation() {
	if (auto* InspectDescription = PerceptibleProperties.Find(TEXT("inspect"))) {
		return *InspectDescription;
	}

	return "";
}
