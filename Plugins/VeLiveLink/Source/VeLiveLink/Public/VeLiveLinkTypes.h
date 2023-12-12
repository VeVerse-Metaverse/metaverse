// Author: Nikolay Bulatov, Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "Roles/LiveLinkCameraTypes.h"
#include "VeLiveLinkEnums.h"
#include "VeNboSerializer.h"
#include "VeLiveLinkTypes.generated.h"


/**
 * Struct for static VeVerse LiveLink data
 */
USTRUCT(BlueprintType)
struct VELIVELINK_API FVeLiveLinkStaticData : public FLiveLinkBaseStaticData {
	GENERATED_BODY()
};

USTRUCT(BlueprintType)
struct VELIVELINK_API FVeLiveLinkPoseLandmarks {
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "VeVerse LiveLink")
	FVector Location = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "VeVerse LiveLink")
	float Visibility = 0;

	friend inline FVeNboSerializeToBuffer& operator <<(FVeNboSerializeToBuffer& Ar, const FVeLiveLinkPoseLandmarks& Value) {
		Ar << Value.Location;
		Ar << Value.Visibility;
		return Ar;
	}

	friend inline FVeNboSerializeFromBuffer& operator >>(FVeNboSerializeFromBuffer& Ar, FVeLiveLinkPoseLandmarks& Value) {
		Ar >> Value.Location;
		Ar >> Value.Visibility;
		return Ar;
	}
};

/**
 * Struct for dynamic (per-frame) VeVerse Live Link data
 */
USTRUCT(BlueprintType)
struct VELIVELINK_API FVeLiveLinkFrameData : public FLiveLinkBaseFrameData {
	GENERATED_BODY()

	/** Face blendshape values */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "VeVerse LiveLink")
	TMap<EVeLiveLinkFaceBlendshapeIndex, float> FaceBlendshapes;

	/** Pose world landmarks, 4th component is visibility */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "VeVerse LiveLink")
	TMap<EVeLiveLinkPoseLandmarkIndex, FVeLiveLinkPoseLandmarks> PoseLandmarks;

	/** Left hand landmarks */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="VeVerse LiveLink")
	TMap<EVeLiveLinkHandLandmarkIndex, FVector> LeftHandLandmarks;

	/** Left hand landmarks */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="VeVerse LiveLink")
	TMap<EVeLiveLinkHandLandmarkIndex, FVector> RightHandLandmarks;
};

/**
 * Facility structure to handle VeVerse Live Link data in blueprint
 */
USTRUCT(BlueprintType)
struct VELIVELINK_API FVeLiveLinkBlueprintData : public FLiveLinkBaseBlueprintData {
	GENERATED_BODY()

	/** Dynamic data that can change every frame  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeVerse LiveLink")
	FVeLiveLinkFrameData FrameData;
};
