// 


#include "VeAiSimpleFsmLocationComponent.h"

UVeAiSimpleFsmLocationComponent::UVeAiSimpleFsmLocationComponent() {
	PrimaryComponentTick.bCanEverTick = true;
}

void UVeAiSimpleFsmLocationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
