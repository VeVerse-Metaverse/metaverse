// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "AiInfoWidgetComponent.h"

#include "UIAIInfoWidget.h"
#include "UIUserInfoWidget.h"
#include "VeAiPlayerState.h"
#include "VeShared.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"


// Sets default values for this component's properties
UAiInfoWidgetComponent::UAiInfoWidgetComponent()
	: Super() {
	PrimaryComponentTick.bCanEverTick = true;

#if !UE_SERVER
	static ConstructorHelpers::FClassFinder<UUserWidget> UserInfoClassFinder(TEXT("/VeAI/Shared/UI/World/WBP_AiUserInfo"));
	SetWidgetClass(UserInfoClassFinder.Class);
	SetDrawSize(FVector2D(1200, 1200));
#endif

	SetRelativeScale3D(FVector(0.1f, 0.1f, 0.1f));
}

void UAiInfoWidgetComponent::OnSay(const FText& InText) {
	if (auto* AiInfoWidget = Cast<UUIAIInfoWidget>(GetWidget()); IsValid(AiInfoWidget)) {
		AiInfoWidget->OnSay(InText);
	}
}

// Called when the game starts
void UAiInfoWidgetComponent::BeginPlay() {
	Super::BeginPlay();

	if (GetNetMode() != NM_DedicatedServer) {
		FTimerDelegate PlayerStateTimerDelegate;
		PlayerStateTimerDelegate.BindWeakLambda(this, [&] {
			if (const auto* const World = GetWorld(); IsValid(World)) {
				if (const auto* OwningPawn = Cast<AVeAiCharacter>(GetOwner()); IsValid(OwningPawn)) {
					if (auto* AiInfoWidget = Cast<UUIAIInfoWidget>(GetWidget()); IsValid(AiInfoWidget)) {
						if (auto* VeAiPlayerState = OwningPawn->GetAiPlayerState<AVeAiPlayerState>()) {
							AiInfoWidget->SetAIPlayerState(VeAiPlayerState);
							World->GetTimerManager().ClearTimer(PlayerStateTimerHandle);
						} else {
							LogWarningF("Pawn %s has no AiPlayerState", *OwningPawn->GetName());
						}
					}
				}
			}
		});

		if (const auto* const World = GetWorld(); IsValid(World)) {
			World->GetTimerManager().SetTimer(PlayerStateTimerHandle, PlayerStateTimerDelegate, 1.0f, true);
		}
	}
}

void UAiInfoWidgetComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	const auto* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!IsValid(PC)) {
		return;
	}

	const auto* Pawn = PC->GetPawn();
	if (!IsValid(Pawn)) {
		return;
	}

	const auto Distance = FVector::Dist(Pawn->GetActorLocation(), GetOwner()->GetActorLocation());
	if (Distance > MaxVisibilityDistance) {
		SetVisibility(false);
		return;
	}

	SetVisibility(true);

	float Alpha;
	if (Distance < MinVisibilityDistance) {
		Alpha = 1.0f;
	} else {
		const float DistanceInRange = FMath::Clamp(Distance - MinVisibilityDistance, 0.0f, MaxVisibilityDistance - MinVisibilityDistance);
		Alpha = 1.0f - FMath::SmoothStep(0.0f, 1.0f, DistanceInRange / (MaxVisibilityDistance - MinVisibilityDistance));
	}

	if (auto* InfoWidget = Cast<UUIAIInfoWidget>(GetWidget())) {
		InfoWidget->SetOpacity(Alpha);
	}
}
