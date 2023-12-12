// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#include "Actors/InteractiveActor.h"
#include <InteractionMacros.h>
#include "Interaction.h"
#include "TimerManager.h"
#include "Components/InteractionObjectComponent.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"
#include "Serialization/BufferArchive.h"

// Sets default values
AInteractiveActor::AInteractiveActor(): Super() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bAlwaysRelevant = true;

	InteractionObjectComponent = CreateDefaultSubobject<UInteractionObjectComponent>("Interaction Object");

	bUseReset = false;
	bUseResetTimer = false;
	ResetTimerDelay = 3.f;
	bUseResetZ = false;
	ResetZ = -1000.f;
	bUseResetDistance = false;
	ResetDistance = 1000.f;
	bUseResetTriggers = false;

	bReplicates = true;
	SetReplicatingMovement(true);
	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
}

void AInteractiveActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AInteractiveActor::OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor) {
	if (!ACTOR_IS_AUTHORITY(this)) {
		return;
	}

	if (OtherActor == OverlappedActor) {
		return;
	}

	if (!bUseReset) {
		return;
	}

	if (ResetBeginOverlapTriggers.Contains(OtherActor)) {
		ResetState();
	}
}

void AInteractiveActor::OnEndOverlap(AActor* OverlappedActor, AActor* OtherActor) {
	if (!ACTOR_IS_AUTHORITY(this)) {
		return;
	}

	if (OtherActor == OverlappedActor) {
		return;
	}

	if (!bUseReset) {
		return;
	}

	if (ResetEndOverlapTriggers.Contains(OtherActor)) {
		ResetState();
	}
}

void AInteractiveActor::OnDetached_Implementation(UInteractiveControlComponent* ControlComponent) {
	if (bUseResetOnDetach) {
		ResetState();
	}
}

void AInteractiveActor::RestartResetTimer() {
	if (!bUseResetTimer) {
		return;
	}

	const auto World = GetWorld();
	if (World == nullptr) {
		UE_LOG(LogInteraction, Error, TEXT("No world, cant restart reset timer"));
		return;
	}

	if (World->GetTimerManager().TimerExists(ResetTimer)) {
		World->GetTimerManager().ClearTimer(ResetTimer);
	}

	World->GetTimerManager().SetTimer(ResetTimer, this, &AInteractiveActor::ResetState, ResetTimerDelay);
}

void AInteractiveActor::ServerResetState_Implementation(const FTransform Transform) {
	SetActorTransform(Transform);
	if (IsValid(this)) {
		if (const UWorld* const World = GetWorld()) {
			FTimerHandle __tempTimerHandle;
			World->GetTimerManager().SetTimer(__tempTimerHandle, FTimerDelegate().CreateWeakLambda(this, [=] {
				AnimateFadeIn();
			}), ResetFadeInDelay, false);
		}
	}
}

void AInteractiveActor::ClientResetState_Implementation(const FTransform Transform) {
	SetActorTransform(Transform);
	if (IsValid(this)) {
		if (const UWorld* const World = GetWorld()) {
			FTimerHandle __tempTimerHandle;
			World->GetTimerManager().SetTimer(__tempTimerHandle, FTimerDelegate().CreateWeakLambda(this, [=] {
				AnimateFadeIn();
			}), ResetFadeInDelay, false);
		}
	}
}

void AInteractiveActor::Multicast_ResetState_Implementation(const FTransform Transform) {
	if (ACTOR_IS_AUTHORITY(this)) {
		ServerResetState(Transform);
	} else {
		ClientResetState(Transform);
	}
}

void AInteractiveActor::ResetState_Implementation() {
	if (!ACTOR_IS_AUTHORITY(this)) {
		return;
	}

	AnimateFadeOut();

	if (IsValid(this)) {
		if (const UWorld* const World = GetWorld()) {
			FTimerHandle __tempTimerHandle;
			World->GetTimerManager().SetTimer(__tempTimerHandle, FTimerDelegate().CreateWeakLambda(this, [=] {
				Multicast_ResetState(OriginalTransform);
			}), ResetFadeOutDelay, false);
		}
	};
}

void AInteractiveActor::BeginPlay() {
	Super::BeginPlay();

	if (HasAuthority()) {
		OriginalTransform = FTransform();
		OriginalTransform.SetLocation(GetActorLocation());
		OriginalTransform.SetRotation(GetActorQuat());
		OriginalTransform.SetScale3D(GetActorScale3D());

		OnActorBeginOverlap.AddDynamic(this, &AInteractiveActor::OnBeginOverlap);
		OnActorEndOverlap.AddDynamic(this, &AInteractiveActor::OnEndOverlap);
	}

#if 0
	if (IsValid(InteractiveTarget)) {
		if (!InteractiveTarget->OnDetached.IsAlreadyBound(this, &AInteractiveActor::OnDetached)) {
			InteractiveTarget->OnDetached.AddDynamic(this, &AInteractiveActor::OnDetached);
		}
	}
#endif

	SetReplicates(true);
	SetReplicatingMovement(true);
}

void AInteractiveActor::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	OnActorBeginOverlap.RemoveDynamic(this, &AInteractiveActor::OnBeginOverlap);
	OnActorEndOverlap.RemoveDynamic(this, &AInteractiveActor::OnEndOverlap);
	Super::EndPlay(EndPlayReason);
}

void AInteractiveActor::Tick(const float DeltaSeconds) {
	Super::Tick(DeltaSeconds);

	if (GIsClient) {
		return;
	}

	if (!bUseReset) {
		return;
	}

	if (bUseResetZ) {
		if (GetActorLocation().Z <= ResetZ) {
			ResetState();
			return;
		}
	}

	if (bUseResetDistance) {
		if (FVector::Distance(GetActorLocation(), OriginalTransform.GetLocation()) >= ResetDistance) {
			ResetState();
		}
	}
}

