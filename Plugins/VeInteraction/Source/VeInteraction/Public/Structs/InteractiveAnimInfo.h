// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "InteractiveAnimInfo.generated.h"

USTRUCT(BlueprintType)
struct FVeInteractiveAnimFingerInfo {
	GENERATED_BODY()

	FVeInteractiveAnimFingerInfo(): Grab(0), Spread(0), Twist(0) {
	}

	/** Finger grab amount. */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	float Grab;

	/** Finger spread amount. */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	float Spread;

	/** Finger twist amount. */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	float Twist;
};

USTRUCT(BlueprintType)
struct FVeInteractiveAnimHandInfo {
	GENERATED_BODY()

	FVeInteractiveAnimHandInfo(): Stretch(0), Compensate(0) {
	}

	/** Stretch the hand out. */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	float Stretch;

	/** Compensate bone stretching. */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	float Compensate;

	/** Thumb finger data. */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FVeInteractiveAnimFingerInfo ThumbFinger;

	/** Index finger data. */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FVeInteractiveAnimFingerInfo IndexFinger;

	/** Middle finger data. */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FVeInteractiveAnimFingerInfo MiddleFinger;

	/** Ring finger data. */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FVeInteractiveAnimFingerInfo RingFinger;

	/** Pinky finger data. */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FVeInteractiveAnimFingerInfo PinkyFinger;
};
