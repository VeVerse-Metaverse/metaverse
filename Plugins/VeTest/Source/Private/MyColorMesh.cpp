// 


#include "MyColorMesh.h"


// Sets default values
AMyColorMesh::AMyColorMesh() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.0f;

	MyColorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MyColorMesh"));
	RootComponent = MyColorMesh;
}

// Called when the game starts or when spawned
void AMyColorMesh::BeginPlay() {
	Super::BeginPlay();

	if (IsValid(MyColorMesh) && IsValid(MyColorMaterial)) {
		MyColorMaterialInstance = UMaterialInstanceDynamic::Create(MyColorMaterial, this);
		MyColorMesh->SetMaterial(0, MyColorMaterialInstance);
	}
}

// Called every frame
void AMyColorMesh::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (IsValid(MyColorMesh)) {
		auto* MaterialInstance = Cast<UMaterialInstanceDynamic>(MyColorMesh->GetMaterial(0));
		if (IsValid(MaterialInstance)) {
			MaterialInstance->SetVectorParameterValue("Color", FLinearColor::MakeRandomColor());
		}
	}
}
