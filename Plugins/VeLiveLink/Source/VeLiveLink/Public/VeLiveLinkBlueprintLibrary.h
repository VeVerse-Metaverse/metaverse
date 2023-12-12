// Author: Nikolay Bulatov, Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "CoreMinimal.h"
#include "VeLiveLinkMessageThread.h"
#include "VeLiveLinkEnums.h"
#include "VeLiveLinkBlueprintLibrary.generated.h"

USTRUCT(BlueprintType)
struct FVeLiveLinkArmData {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FVector RightArmPosition = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite)
	FVector RightForearmPosition = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite)
	FVector RightHandPosition = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite)
	FVector LeftArmPosition = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite)
	FVector LeftForearmPosition = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite)
	FVector LeftHandPosition = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite)
	FRotator RightArmRotation = FRotator::ZeroRotator;

	UPROPERTY(BlueprintReadWrite)
	FRotator RightForearmRotation = FRotator::ZeroRotator;

	UPROPERTY(BlueprintReadWrite)
	FRotator RightHandRotation = FRotator::ZeroRotator;

	UPROPERTY(BlueprintReadWrite)
	FRotator LeftArmRotation = FRotator::ZeroRotator;

	UPROPERTY(BlueprintReadWrite)
	FRotator LeftForearmRotation = FRotator::ZeroRotator;

	UPROPERTY(BlueprintReadWrite)
	FRotator LeftHandRotation = FRotator::ZeroRotator;
};

/**
 * 
 */
UCLASS()
class VELIVELINK_API UVeLiveLinkBlueprintLibrary : public UBlueprintFunctionLibrary {
public:
	GENERATED_BODY()

	/**
	 * @brief Returns the angle between two points in 2D space.
	 */
	static float Get2dAngleInRadians(const FVector2D A, const FVector2D B);
	static float Get2dAngleInDegrees(const FVector2D A, const FVector2D B);

	/**
	 * @brief Get the angle between three points in 3D space.
	 */
	static float Get3dAngleInRadians(FVector A, FVector B, FVector C);
	static float Get3dAngleInDegrees(FVector A, FVector B, FVector C);

	/**
	 * @brief Returns the rotation between two points in 3D space.
	 */
	static FVector FindRotationInDegrees(FVector A, FVector B);

	/**
	 * @brief Converts normalized rotation values into radians clamped by human limits.
	 */
	static FVeLiveLinkArmData RigArmData(const FVeLiveLinkArmData& ArmData);

	UFUNCTION(BlueprintCallable, Category = "VeLiveLink")
	static void GetRiggedArmData(const TMap<EVeLiveLinkPoseLandmarkIndex, FVeLiveLinkPoseLandmarks>& InData, FVeLiveLinkArmData& OutArmData);

	UFUNCTION(BlueprintCallable, Category = "VeLiveLink")
	static void GetArmData(const TMap<EVeLiveLinkPoseLandmarkIndex, FVeLiveLinkPoseLandmarks>& InData, FVeLiveLinkArmData& OutArmData);
};
