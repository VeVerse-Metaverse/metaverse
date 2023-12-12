// 


#include "TestDynamicActor.h"


// Sets default values
ATestDynamicActor::ATestDynamicActor() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");

	TestDynamicComponent = CreateDefaultSubobject<UTestDynamicComponent>("TestDynamicComponent");
	TestDynamicComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ATestDynamicActor::BeginPlay() {
	Super::BeginPlay();
	
}

// Called every frame
void ATestDynamicActor::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}

