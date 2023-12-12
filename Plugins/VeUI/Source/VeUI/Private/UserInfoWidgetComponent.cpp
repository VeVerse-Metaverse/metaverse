// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UserInfoWidgetComponent.h"
#include "GameFramework/Pawn.h"
#include "UIUserInfoWidget.h"
#include "VePlayerStateBase.h"
#include "UObject/ConstructorHelpers.h"


// Sets default values for this component's properties
UUserInfoWidgetComponent::UUserInfoWidgetComponent()
	: Super() {
	PrimaryComponentTick.bCanEverTick = true;
	SetOwnerNoSee(true);

#if !UE_SERVER
	static ConstructorHelpers::FClassFinder<UUserWidget> UserInfoClassFinder(TEXT("/VeUI/UI/World/WBP_UIUserInfo"));
	SetWidgetClass(UserInfoClassFinder.Class);
	SetDrawSize(FVector2D(1200, 1200));
#endif

	SetRelativeScale3D(FVector(0.1f, 0.1f, 0.1f));
}

// Called when the game starts
void UUserInfoWidgetComponent::BeginPlay() {
	Super::BeginPlay();

	if (GetNetMode() != NM_DedicatedServer) {
		const auto* OwningPawn = Cast<APawn>(GetOwner());
		if (IsValid(OwningPawn)) {
			SetPlayerState(OwningPawn->GetPlayerState());
		}
	}
}

void UUserInfoWidgetComponent::SetPlayerState(APlayerState* InPlayerState) const {
	if (GetNetMode() != NM_DedicatedServer) {
		auto* UserInfoWidget = Cast<UUIUserInfoWidget>(GetWidget());
		if (IsValid(UserInfoWidget)) {
			auto* VePlayerState = Cast<AVePlayerStateBase>(InPlayerState);
			UserInfoWidget->SetPlayerState(VePlayerState);
		}
	}
}
