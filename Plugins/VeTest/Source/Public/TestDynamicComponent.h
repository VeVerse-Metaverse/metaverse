// 

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TestDynamicComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class VETEST_API UTestDynamicComponent : public USceneComponent {
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UTestDynamicComponent();

protected:
	// Called when the game starts
	virtual void OnRegister() override;
	virtual void OnUnregister() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> DynamicMeshComponent;
};
