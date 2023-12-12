// 


#include "VeAiSimpleFsmSubjectComponent.h"

#include "VeAiCharacter.h"
#include "VeAiPlayerState.h"
#include "VeAiSimpleFsmBehaviorComponent.h"


UVeAiSimpleFsmSubjectComponent::UVeAiSimpleFsmSubjectComponent() {
	PrimaryComponentTick.bCanEverTick = true;
}

void UVeAiSimpleFsmSubjectComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

UVeAiSimpleFsmBehaviorComponent* UVeAiSimpleFsmSubjectComponent::GetBehaviorComponent() const {
	const auto* Owner = GetOwner();
	if (!IsValid(Owner)) {
		return nullptr;
	}

	return Owner->FindComponentByClass<UVeAiSimpleFsmBehaviorComponent>();
}

FString UVeAiSimpleFsmSubjectComponent::GetId() const {
	const auto* Owner = GetOwner();
	if (!IsValid(Owner)) {
		return {};
	}

	if (const auto* OwningCharacter = Cast<AVeAiCharacter>(Owner); IsValid(OwningCharacter)) {
		if (const auto* State = OwningCharacter->GetAiPlayerState(); IsValid(State)) {
			return State->GetUserMetadata().Name;
		}
	}

	return {};
}

FString UVeAiSimpleFsmSubjectComponent::GetDescription() const {
	const auto* Owner = GetOwner();
	if (!IsValid(Owner)) {
		return {};
	}

	if (const auto* OwningCharacter = Cast<AVeAiCharacter>(Owner); IsValid(OwningCharacter)) {
		if (const auto* State = OwningCharacter->GetAiPlayerState(); IsValid(State)) {
			return State->GetUserMetadata().Description;
		}
	}

	return {};
}
