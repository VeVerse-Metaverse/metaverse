// 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyColorMesh.generated.h"

UCLASS()
class VETEST_API AMyColorMesh : public AActor {
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMyColorMesh();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyColorMesh")
	UStaticMeshComponent* MyColorMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyColorMesh")
	UMaterialInterface* MyColorMaterial = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyColorMesh")
	UMaterialInstanceDynamic* MyColorMaterialInstance = nullptr;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


};
