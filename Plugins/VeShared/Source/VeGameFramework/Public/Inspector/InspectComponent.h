// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


#pragma once

#include "PlayerComponent.h"
#include "InspectComponent.generated.h"


#ifndef ECC_InteractionTraceChannel
#define ECC_InteractionTraceChannel ECC_GameTraceChannel1
#endif

const static FName NAME_TraceInteractionFocus2 = TEXT("TraceInteractionFocus");


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class VEGAMEFRAMEWORK_API UInspectComponent : public UPlayerComponent {
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInspectComponent();

	UPROPERTY(BlueprintReadOnly)
	AActor* FindActor = nullptr;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AInspector> InspectorClass = nullptr;

	/** Current inspector spawned for the inspected object */
	UPROPERTY()
	TWeakObjectPtr<AInspector> Inspector;

	DECLARE_EVENT_OneParam(FInspectSubsystem, FOnBeginInspect, AActor* /*InspectedActor*/)

	FOnBeginInspect OnBeginInspect;

	DECLARE_EVENT(FInspectSubsystem, FOnEndInspect)

	FOnEndInspect OnEndInspect;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	AActor* GetPawnInspectTarget() const;

	AActor* BeginInspect();

	UFUNCTION(BlueprintCallable)
	void OnOrbitBegin();

	UFUNCTION(BlueprintCallable)
	void OrbitUpdate(float DeltaX, float DeltaY);

	UFUNCTION(BlueprintCallable)
	void OnOrbitEnd();

	UFUNCTION(BlueprintCallable)
	void OnPanBegin();

	UFUNCTION(BlueprintCallable)
	void PanUpdate(float DeltaX, float DeltaY);

	UFUNCTION(BlueprintCallable)
	void OnPanEnd();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bPanning = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bOrbiting = false;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction|Options")
	float MaxFocusDistance = 500.0f;

	/** Begin inspecting selected actor by spawning and attaching an inspector actor to it. */
	UFUNCTION(BlueprintCallable)
	void BeginInspectActor(AActor* InInspectTarget);

	/** End inspecting, deleting the inspector actor. */
	UFUNCTION(BlueprintCallable)
	void EndInspect();

	// void Pan(FVector PanValue);
	void Zoom(float ZoomValue) const;
	void ResetView() const;

	AActor* GetInspectTarget() const { return InspectTarget.Get(); }
	
private:
	TWeakObjectPtr<AActor> InspectTarget;

};
