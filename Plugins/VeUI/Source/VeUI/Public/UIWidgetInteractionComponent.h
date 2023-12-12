// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once


#include "UIPlayerController.h"
#include "Components/WidgetInteractionComponent.h"
#include "UIWidgetInteractionComponent.generated.h"

/**
 * 
 */
UCLASS()
class VEUI_API UUIWidgetInteractionComponent : public UWidgetInteractionComponent {
	GENERATED_BODY()

	FDelegateHandle OnInputTouchDelegateHandle;

public:
	UUIWidgetInteractionComponent(const FObjectInitializer& InObjectInitializer);

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Helper to get an owning UI player controller. */
	AUIPlayerController* GetOwningPlayerController() const;

	/** Returns touch hit result. */
	FHitResult GetTouchHitResult(uint32 InIndex, ETouchType::Type InType, const FVector2D& InLocation, const float InForce) const;

	/** Interaction mode to be used for the interaction component. */
	EUIInteractionMode GetInteractionMode() const;

protected:
	virtual FWidgetPath FindHoveredWidgetPath(const FWidgetTraceResult& TraceResult) const override;
};
