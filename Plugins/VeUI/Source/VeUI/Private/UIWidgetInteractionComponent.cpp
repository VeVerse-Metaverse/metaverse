// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "UIWidgetInteractionComponent.h"

#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"

UUIWidgetInteractionComponent::UUIWidgetInteractionComponent(const FObjectInitializer& InObjectInitializer): Super(InObjectInitializer) {
	PrimaryComponentTick.bCanEverTick = true;
}

void UUIWidgetInteractionComponent::BeginPlay() {
	Super::BeginPlay();

	switch (GetInteractionMode()) {
	case EUIInteractionMode::Desktop:
		InteractionSource = EWidgetInteractionSource::CenterScreen;
		break;
	case EUIInteractionMode::Mobile:
		{
			InteractionSource = EWidgetInteractionSource::Custom;

			auto* OwningPlayerController = GetOwningPlayerController();
			if (IsValid(OwningPlayerController)) {
				if (!OnInputTouchDelegateHandle.IsValid()) {
					OnInputTouchDelegateHandle = OwningPlayerController->GetOnInputTouch().AddWeakLambda(
						this, [=](const uint32 InIndex, const ETouchType::Type InType, const FVector2D& InLocation, const float InForce) {
							const auto TouchHitResult = GetTouchHitResult(InIndex, InType, InLocation, InForce);
							SetCustomHitResult(TouchHitResult);
							PointerIndex = InIndex;
						});
				}
			}

			break;
		}
	case EUIInteractionMode::VR:
		InteractionSource = EWidgetInteractionSource::World;
		break;
	default:
		break;
	}
}

void UUIWidgetInteractionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	auto* OwningPlayerController = GetOwningPlayerController();
	if (IsValid(OwningPlayerController)) {
		OwningPlayerController->GetOnInputTouch().Remove(OnInputTouchDelegateHandle);
		OnInputTouchDelegateHandle.Reset();
	}

	Super::EndPlay(EndPlayReason);
}

AUIPlayerController* UUIWidgetInteractionComponent::GetOwningPlayerController() const {
	auto* Owner = GetOwner();

	if (!IsValid(Owner)) {
		return nullptr;
	}

	auto* OwningPlayerController = Cast<AUIPlayerController>(Owner);
	if (IsValid(OwningPlayerController)) {
		return OwningPlayerController;
	}

	const auto* OwningPawn = Cast<APawn>(Owner);
	if (IsValid(OwningPawn)) {
		auto* Controller = OwningPawn->GetController();
		OwningPlayerController = Cast<AUIPlayerController>(Controller);
		if (IsValid(OwningPlayerController)) {
			return OwningPlayerController;
		}
	}

	return nullptr;
}

FHitResult UUIWidgetInteractionComponent::GetTouchHitResult(uint32 InIndex, ETouchType::Type InType, const FVector2D& InLocation, const float InForce) const {
	// Make list of sibling or child components to ignore during hit testing.
	TArray<UPrimitiveComponent*> PrimitiveChildren;
	GetRelatedComponentsToIgnoreInAutomaticHitTesting(PrimitiveChildren);
	FCollisionQueryParams Params = FCollisionQueryParams::DefaultQueryParam;
	Params.AddIgnoredComponents(PrimitiveChildren);

	const auto* OwningPlayerController = GetOwningPlayerController();

	// Resulting world origin.
	FVector ProjectedWorldOrigin;
	// Resulting world direction.
	FVector ProjectedWorldDirection;
	// Resulting hits.
	TArray<FHitResult> HitResults;

	if (UGameplayStatics::DeprojectScreenToWorld(OwningPlayerController, InLocation, ProjectedWorldOrigin, ProjectedWorldDirection)) {
		const FVector LineStartLocation = ProjectedWorldOrigin;
		const FVector LineEndLocation = ProjectedWorldOrigin + ProjectedWorldDirection * InteractionDistance;
		if (const auto* World = GetWorld()) {
			World->LineTraceMultiByChannel(HitResults, LineStartLocation, LineEndLocation, TraceChannel, Params);
		}
	}

	for (const FHitResult& HitResult : HitResults) {
		if (const auto* HitWidgetComponent = Cast<UWidgetComponent>(HitResult.GetComponent())) {
			if (HitWidgetComponent->IsVisible()) {
				return HitResult;
			}
		} else {
			// If we hit something that wasn't a widget component, we're done.
			break;
		}
	}

	return {};
}

EUIInteractionMode UUIWidgetInteractionComponent::GetInteractionMode() const {
	const auto* PlayerController = GetOwningPlayerController();
	if (IsValid(PlayerController)) {
		return PlayerController->GetInteractionMode();
	}
	return EUIInteractionMode::Unknown;
}

FWidgetPath UUIWidgetInteractionComponent::FindHoveredWidgetPath(const FWidgetTraceResult& TraceResult) const {
	if (IsValid(TraceResult.HitWidgetComponent)) {
		constexpr float WidgetCursorRadius = 5.0f;
		return FWidgetPath(TraceResult.HitWidgetComponent->GetHitWidgetPath(TraceResult.LocalHitLocation, false, WidgetCursorRadius));
	}
	return Super::FindHoveredWidgetPath(TraceResult);
}
