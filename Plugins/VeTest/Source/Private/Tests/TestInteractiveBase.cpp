#include "Tests/TestInteractiveBase.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InteractionObjectComponent.h"

ATestInteractiveBase::ATestInteractiveBase() {
	PrimaryActorTick.bCanEverTick = true;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	check(BoxComponent);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	check(MeshComponent);

	InteractionObjectComponent = CreateDefaultSubobject<UInteractionObjectComponent>(TEXT("InteractionObject"));
	check(InteractionObjectComponent);

	BoxComponent->SetupAttachment(RootComponent);
	MeshComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ATestInteractiveBase::BeginPlay() {
	Super::BeginPlay();
}

// Called every frame
void ATestInteractiveBase::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}

void ATestInteractiveBase::ServerFocusBegin_Implementation(UInteractionSubjectComponent* Subject, UInteractionObjectComponent* Object,
														   const FVector& Origin,
														   const FVector& Location,
														   const FVector& Normal) {
	IInteractive::ServerFocusBegin_Implementation(Subject, Object, Origin, Location, Normal);

	bServerFocused = true;
	ServerFocusedBy = Subject;
}

void ATestInteractiveBase::ServerFocusEnd_Implementation(UInteractionSubjectComponent* Subject, UInteractionObjectComponent* Object) {
	IInteractive::ServerFocusEnd_Implementation(Subject, Object);

	bServerFocused = false;
	ServerFocusedBy = nullptr;
}

void ATestInteractiveBase::ClientFocusBegin_Implementation(UInteractionSubjectComponent* Subject, UInteractionObjectComponent* Object,
														   const FVector& Origin,
														   const FVector& Location,
														   const FVector& Normal) {
	IInteractive::ClientFocusBegin_Implementation(Subject, Object, Origin, Location, Normal);

	bClientFocused = true;
	ClientFocusedBy = Subject;
}

void ATestInteractiveBase::ClientFocusEnd_Implementation(UInteractionSubjectComponent* Subject, UInteractionObjectComponent* Object) {
	IInteractive::ClientFocusEnd_Implementation(Subject, Object);

	bClientFocused = false;
	ClientFocusedBy = nullptr;
}

void ATestInteractiveBase::ServerOverlapBegin_Implementation(UInteractionSubjectComponent* Subject, UInteractionObjectComponent* Object,
															 const FVector& Origin,
															 const FVector& Location,
															 const FVector& Normal) {
	IInteractive::ServerOverlapBegin_Implementation(Subject, Object, Origin, Location, Normal);

	bServerOverlapped = true;
	ServerOverlappedBy = Subject;
}

void ATestInteractiveBase::ServerOverlapEnd_Implementation(UInteractionSubjectComponent* Subject, UInteractionObjectComponent* Object) {
	IInteractive::ServerOverlapEnd_Implementation(Subject, Object);

	bServerOverlapped = false;
	ServerOverlappedBy = nullptr;
}

void ATestInteractiveBase::ClientOverlapBegin_Implementation(UInteractionSubjectComponent* Subject, UInteractionObjectComponent* Object,
															 const FVector& Origin,
															 const FVector& Location,
															 const FVector& Normal) {
	IInteractive::ClientOverlapBegin_Implementation(Subject, Object, Origin, Location, Normal);

	bClientOverlapped = true;
	ClientOverlappedBy = Subject;
}

void ATestInteractiveBase::ClientOverlapEnd_Implementation(UInteractionSubjectComponent* Subject, UInteractionObjectComponent* Object) {
	IInteractive::ClientOverlapEnd_Implementation(Subject, Object);

	bClientOverlapped = false;
	ClientOverlappedBy = nullptr;
}

void ATestInteractiveBase::ServerInputBegin_Implementation(UInteractionSubjectComponent* Subject, UInteractionObjectComponent* Object, const FName& Name,
														   const FVector& Origin,
														   const FVector& Location,
														   const FVector& Normal) {
	IInteractive::ServerInputBegin_Implementation(Subject, Object, Name, Origin, Location, Normal);

	bServerInputActive = true;
	ServerInputActivatedBy = Subject;
}

void ATestInteractiveBase::ServerInputEnd_Implementation(UInteractionSubjectComponent* Subject, UInteractionObjectComponent* Object, const FName& Name,
														 const FVector& Origin,
														 const FVector& Location,
														 const FVector& Normal) {
	IInteractive::ServerInputEnd_Implementation(Subject, Object, Name, Origin, Location, Normal);

	bServerInputActive = false;
	ServerInputActivatedBy = nullptr;
}

void ATestInteractiveBase::ClientInputBegin_Implementation(UInteractionSubjectComponent* Subject, UInteractionObjectComponent* Object, const FName& Name,
														   const FVector& Origin,
														   const FVector& Location,
														   const FVector& Normal) {
	IInteractive::ClientInputBegin_Implementation(Subject, Object, Name, Origin, Location, Normal);

	bClientInputActive = true;
	ClientInputActivatedBy = Subject;
}

void ATestInteractiveBase::ClientInputEnd_Implementation(UInteractionSubjectComponent* Subject, UInteractionObjectComponent* Object, const FName& Name,
														 const FVector& Origin,
														 const FVector& Location,
														 const FVector& Normal) {
	IInteractive::ClientInputEnd_Implementation(Subject, Object, Name, Origin, Location, Normal);

	bClientInputActive = false;
	ClientInputActivatedBy = nullptr;
}
