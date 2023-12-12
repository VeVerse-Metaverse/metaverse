// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "InteractiveFingerSettings.generated.h"

USTRUCT(BlueprintType)
struct FInteractiveFingerSettings {

	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fingers")
	float ThumbGrab = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fingers")
	float ThumbSpread = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fingers")
	float ThumbTwist = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fingers")
	float IndexGrab = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fingers")
	float IndexSpread = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fingers")
	float IndexTwist = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fingers")
	float MiddleGrab = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fingers")
	float MiddleSpread = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fingers")
	float MiddleTwist = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fingers")
	float RingGrab = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fingers")
	float RingSpread = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fingers")
	float RingTwist = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fingers")
	float PinkyGrab = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fingers")
	float PinkySpread = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fingers")
	float PinkyTwist = 0.0f;
};
