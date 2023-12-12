// 

#pragma once

#include "GameFramework/Actor.h"
#include "Interfaces/Interactive.h"
#include "TestInteractiveBase.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class UInteractionObjectComponent;
class UInteractionSubjectComponent;

UCLASS()
class VETEST_API ATestInteractiveBase : public AActor, public IInteractive {
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATestInteractiveBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UBoxComponent> BoxComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UInteractionObjectComponent> InteractionObjectComponent;

	virtual void ServerFocusBegin_Implementation(UInteractionSubjectComponent* Subject, UInteractionObjectComponent* Object, const FVector& Origin, const FVector& Location,
												 const FVector& Normal) override;
	virtual void ServerFocusEnd_Implementation(UInteractionSubjectComponent* Subject, UInteractionObjectComponent* Object) override;

	virtual void ClientFocusBegin_Implementation(UInteractionSubjectComponent* Subject, UInteractionObjectComponent* Object, const FVector& Origin, const FVector& Location,
												 const FVector& Normal) override;
	virtual void ClientFocusEnd_Implementation(UInteractionSubjectComponent* Subject, UInteractionObjectComponent* Object) override;

	virtual void ServerOverlapBegin_Implementation(UInteractionSubjectComponent* Subject, UInteractionObjectComponent* Object, const FVector& Origin, const FVector& Location,
												   const FVector& Normal) override;
	virtual void ServerOverlapEnd_Implementation(UInteractionSubjectComponent* Subject, UInteractionObjectComponent* Object) override;

	virtual void ClientOverlapBegin_Implementation(UInteractionSubjectComponent* Subject, UInteractionObjectComponent* Object, const FVector& Origin, const FVector& Location,
												   const FVector& Normal) override;
	virtual void ClientOverlapEnd_Implementation(UInteractionSubjectComponent* Subject, UInteractionObjectComponent* Object) override;

	virtual void ServerInputBegin_Implementation(UInteractionSubjectComponent* Subject, UInteractionObjectComponent* Object, const FName& Name, const FVector& Origin, const FVector& Location,
												   const FVector& Normal) override;

	virtual void ServerInputEnd_Implementation(UInteractionSubjectComponent* Subject, UInteractionObjectComponent* Object, const FName& Name, const FVector& Origin, const FVector& Location,
												   const FVector& Normal) override;

	virtual void ClientInputBegin_Implementation(UInteractionSubjectComponent* Subject, UInteractionObjectComponent* Object, const FName& Name, const FVector& Origin, const FVector& Location,
												   const FVector& Normal) override;

	virtual void ClientInputEnd_Implementation(UInteractionSubjectComponent* Subject, UInteractionObjectComponent* Object, const FName& Name, const FVector& Origin, const FVector& Location,
												   const FVector& Normal) override;

#pragma region
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bServerFocused;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInteractionSubjectComponent* ServerFocusedBy = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bServerOverlapped;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInteractionSubjectComponent* ServerOverlappedBy = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bServerInputActive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInteractionSubjectComponent* ServerInputActivatedBy = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bClientFocused;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInteractionSubjectComponent* ClientFocusedBy = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bClientOverlapped;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInteractionSubjectComponent* ClientOverlappedBy = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bClientInputActive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInteractionSubjectComponent* ClientInputActivatedBy = nullptr;

#pragma endregion
};
