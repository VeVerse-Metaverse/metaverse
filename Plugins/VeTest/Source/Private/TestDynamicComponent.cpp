// 


#include "TestDynamicComponent.h"

#include "PlaceableLib.h"


// Sets default values for this component's properties
UTestDynamicComponent::UTestDynamicComponent() {
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UTestDynamicComponent::OnRegister() {
	Super::OnRegister();

	DynamicMeshComponent = Cast<UStaticMeshComponent>(UPlaceableLib::CreateRuntimeComponent(this, UStaticMeshComponent::StaticClass(), "DynamicMeshComponent", CreationMethod));
	if (DynamicMeshComponent) {
		DynamicMeshComponent->bAutoActivate = false;
		DynamicMeshComponent->SetMobility(EComponentMobility::Movable);

		if (DynamicMeshComponent) {
			for (int i = 0; i < 3; i++) {
				const FName NewChildComponentName = *FString::Printf(TEXT("RuntimeChildComponent_%d"), i);
				const auto NewChildComp = Cast<UStaticMeshComponent>(UPlaceableLib::CreateRuntimeComponent(DynamicMeshComponent, UStaticMeshComponent::StaticClass(), NewChildComponentName, EComponentCreationMethod::Instance));
				for (int j = 0; j < 3; j++) {
					const FName NewGrandChildComponentName = *FString::Printf(TEXT("RuntimeGrandChildComponent_%d_%d"), i, j);
					Cast<UStaticMeshComponent>(UPlaceableLib::CreateRuntimeComponent(NewChildComp, USceneComponent::StaticClass(), NewGrandChildComponentName, EComponentCreationMethod::Instance));
				}
			}
		}
	}
}

void UTestDynamicComponent::OnUnregister() {
	Super::OnUnregister();

	UPlaceableLib::DestroyRuntimeComponent(this, DynamicMeshComponent);
}


// Called every frame
void UTestDynamicComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}
