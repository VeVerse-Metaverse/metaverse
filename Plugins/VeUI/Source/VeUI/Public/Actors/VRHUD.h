// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "GameFramework/Actor.h"
#include "VRHUD.generated.h"

class UUIWidget;
class UWidgetComponent;

UCLASS(Blueprintable, BlueprintType)
class VEUI_API AVRHUD final : public AActor {
	GENERATED_BODY()

public:
	AVRHUD();

	/**
	 * Set owning pawn the widget should follow.
	 */
	void SetPawn(APawn* InPawn);

	/**
	 * Pin or unpin the menu. 
	 */
	void SetIsPinned(bool bInIsPinned);;

	/**
	 * Is menu pinned or not.
	 */
	bool GetIsPinned() const;

	/**
	 * Activate or deactivate the menu.
	 */
	void SetIsActive(bool bInIsActive);

	/**
	 * Activate or deactivate the menu.
	 */
	bool GetIsActive() const;

	class UVRRootWidget* GetRootWidget() const;

private:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	/**
	 * Menu is visible and is following the pawn when it is active.
	 */
	UPROPERTY(VisibleInstanceOnly)
	bool bIsActive = false;

	/**
	 * Allows to pin the menu so it stop moving after the player.
	 * @remark When player moves too far from the pinned menu, it gets disabled and unpinned
	 * so later player will be able to call it back.
	 */
	UPROPERTY(VisibleInstanceOnly)
	bool bIsPinned = false;

	/**
	 * Determines the distance the widget will maintain away from the pawn.
	 */
	UPROPERTY(EditDefaultsOnly)
	float Distance = 120.0f;

	/**
	 * Determines how fast the widget will move to the desired position.
	 */
	UPROPERTY(EditDefaultsOnly)
	float InterpolationSpeed = 1.2f;

	/**
	 * Offset to apply to the camera height.
	 */
	UPROPERTY(EditDefaultsOnly)
	float CameraVerticalOffset = 16.0f;

	/**
	 * Pawn that owns the menu.
	 */
	UPROPERTY(VisibleInstanceOnly)
	APawn* Pawn;

	/**
	 * Distance above which the menu is unpinned.
	 */
	UPROPERTY(EditDefaultsOnly)
	float PinDistanceLimit = 500.f;

	/**
	 * How often to check the pinned menu distance.
	 */
	UPROPERTY(EditDefaultsOnly)
	float PinDistanceCheckRate = 1.0f;

	/**
	 * Widget used to render the menu.
	 */
	UPROPERTY(EditDefaultsOnly, Category="UI")
	UWidgetComponent* MenuWidget;

	/** Time handle for checking the distance of pinned menu. */
	FTimerHandle DistanceCheckTimerHandle;
};
