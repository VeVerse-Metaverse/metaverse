// 

#pragma once

#include "CoreMinimal.h"
#include "TestDynamicComponent.h"
#include "GameFramework/Actor.h"
#include "TestDynamicActor.generated.h"

UCLASS()
class VETEST_API ATestDynamicActor : public AActor {
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATestDynamicActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UTestDynamicComponent> TestDynamicComponent;
};
