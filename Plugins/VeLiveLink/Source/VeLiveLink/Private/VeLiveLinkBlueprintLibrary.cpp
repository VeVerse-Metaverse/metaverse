// Author: Nikolay Bulatov, Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#include "VeLiveLinkBlueprintLibrary.h"

#include "VeLiveLinkTypes.h"
#include "VeLiveLinkEnums.h"
#include "VeLiveLinkMessageThread.h"

float UVeLiveLinkBlueprintLibrary::Get2dAngleInRadians(const FVector2D A, const FVector2D B) {
	const float DeltaX = B.X - A.X;
	const float DeltaY = B.Y - A.Y;
	const float Angle = FMath::Atan2(DeltaY, DeltaX);
	return Angle;
}

float UVeLiveLinkBlueprintLibrary::Get2dAngleInDegrees(const FVector2D A, const FVector2D B) {
	return FMath::RadiansToDegrees(Get2dAngleInRadians(A, B));
}

float UVeLiveLinkBlueprintLibrary::Get3dAngleInRadians(const FVector A, const FVector B, const FVector C) {
	// Calculate vector between points 1 and 2.
	const FVector V1 = A - B;
	// Calculate vector between points 2 and 3.
	const FVector V2 = C - B;

	// Normalize the vectors.
	const auto N1 = V1.GetSafeNormal();
	const auto N2 = V2.GetSafeNormal();

	// Dot product of the normalized vectors is a function of the cosine of the angle between the vectors scaled by the length of the vectors.
	const auto Dot = N1.Dot(N2);

	// Extract the angle from the dot product.
	const auto Angle = FMath::Acos(Dot);

	return Angle;
}

float UVeLiveLinkBlueprintLibrary::Get3dAngleInDegrees(const FVector A, const FVector B, const FVector C) {
	return FMath::RadiansToDegrees(Get3dAngleInRadians(A, B, C));
}

FVector UVeLiveLinkBlueprintLibrary::FindRotationInDegrees(const FVector A, const FVector B) {
	const float X = Get2dAngleInDegrees(FVector2D(A.Z, A.X), FVector2D(B.Z, B.X));
	const float Y = Get2dAngleInDegrees(FVector2D(A.Z, A.Y), FVector2D(B.Z, B.Y));
	const float Z = Get2dAngleInDegrees(FVector2D(A.X, A.Y), FVector2D(B.X, B.Y));
	return FVector(X, Y, Z);
}

FVeLiveLinkArmData UVeLiveLinkBlueprintLibrary::RigArmData(const FVeLiveLinkArmData& ArmData) {
	float invert = -1;

	FVeLiveLinkArmData RiggedArmData = ArmData;

	// todo: implement
	RiggedArmData.LeftArmRotation.Yaw *= -2.3f * invert;
	RiggedArmData.LeftArmRotation.Pitch *= PI * invert;
	RiggedArmData.LeftArmRotation.Pitch -= ArmData.LeftForearmRotation.Roll;
	RiggedArmData.LeftArmRotation.Pitch -= -invert * FMath::Max(ArmData.LeftForearmRotation.Yaw, 0);
	RiggedArmData.LeftArmRotation.Roll -= 0.3f * invert;

	RiggedArmData.LeftForearmRotation.Yaw *= -2.14f * invert;
	RiggedArmData.LeftForearmRotation.Pitch *= 2.14 * invert;
	RiggedArmData.LeftForearmRotation.Roll *= 2.14 * invert;

	RiggedArmData.LeftArmRotation.Roll = FMath::Clamp(RiggedArmData.LeftArmRotation.Roll, -0.5f, UE_PI);
	RiggedArmData.LeftForearmRotation.Roll = FMath::Clamp(RiggedArmData.LeftForearmRotation.Roll, -0.3f, 0.3f);

	RiggedArmData.LeftHandRotation.Pitch = FMath::Clamp(RiggedArmData.LeftHandRotation.Pitch * 2, -0.6f, 0.6f); // side to side
	RiggedArmData.LeftHandRotation.Roll = RiggedArmData.LeftHandRotation.Roll * -2.3f * invert;                 // up down

	// Right arm
	RiggedArmData.RightArmRotation.Yaw = FMath::Clamp(RiggedArmData.RightArmRotation.Yaw, -90.0f, 90.0f);     // Not used
	RiggedArmData.RightArmRotation.Pitch = FMath::Clamp(RiggedArmData.RightArmRotation.Pitch, -90.0f, 90.0f); // Limit shoulder forward-backward bend
	RiggedArmData.RightArmRotation.Roll = FMath::Clamp(RiggedArmData.RightArmRotation.Roll, -1.0f, 1.0f);     // Limit shoulder up-down bend

	RiggedArmData.RightForearmRotation.Yaw = FMath::Clamp(RiggedArmData.RightForearmRotation.Yaw, -1.0f, 1.0f);        // Not used
	RiggedArmData.RightForearmRotation.Pitch = FMath::Clamp(RiggedArmData.RightForearmRotation.Pitch, -145.0f, -5.0f); // Limit elbow bend
	RiggedArmData.RightForearmRotation.Roll = FMath::Clamp(RiggedArmData.RightForearmRotation.Roll, -1.0f, 1.0f);      // Not used

	RiggedArmData.RightHandRotation.Pitch = FMath::Clamp(RiggedArmData.RightHandRotation.Pitch, -28.0f, 28.0f); // Limit side to side hand movement
	RiggedArmData.RightHandRotation.Roll = FMath::Clamp(RiggedArmData.RightHandRotation.Roll, -25.0f, 65.0f);   // Limit up down hand movement
	RiggedArmData.RightHandRotation.Yaw = FMath::Clamp(RiggedArmData.RightHandRotation.Yaw, -25.0f, 65.0f);     // Limit hand rotation 

	return RiggedArmData;
}

void UVeLiveLinkBlueprintLibrary::GetRiggedArmData(const TMap<EVeLiveLinkPoseLandmarkIndex, FVeLiveLinkPoseLandmarks>& InData, FVeLiveLinkArmData& OutArmData) {
	GetArmData(InData, OutArmData);
	OutArmData = RigArmData(OutArmData);
}

void UVeLiveLinkBlueprintLibrary::GetArmData(const TMap<EVeLiveLinkPoseLandmarkIndex, FVeLiveLinkPoseLandmarks>& InData, FVeLiveLinkArmData& OutArmData) {
	auto* LeftShoulderRef = InData.Find(EVeLiveLinkPoseLandmarkIndex::LeftShoulder);
	auto* RightShoulderRef = InData.Find(EVeLiveLinkPoseLandmarkIndex::RightShoulder);
	auto* LeftElbowRef = InData.Find(EVeLiveLinkPoseLandmarkIndex::LeftElbow);
	auto* RightElbowRef = InData.Find(EVeLiveLinkPoseLandmarkIndex::RightElbow);
	auto* LeftWristRef = InData.Find(EVeLiveLinkPoseLandmarkIndex::LeftWrist);
	auto* RightWristRef = InData.Find(EVeLiveLinkPoseLandmarkIndex::RightWrist);
	auto* LeftIndexRef = InData.Find(EVeLiveLinkPoseLandmarkIndex::LeftIndex);
	auto* RightIndexRef = InData.Find(EVeLiveLinkPoseLandmarkIndex::RightIndex);
	auto* LeftPinkyRef = InData.Find(EVeLiveLinkPoseLandmarkIndex::LeftPinky);
	auto* RightPinkyRef = InData.Find(EVeLiveLinkPoseLandmarkIndex::RightPinky);

	// Note: the left and right arms are swapped.

	FVector RightShoulder{57.335400f, 3.560128f, 7.514849f}; // (Pitch=3.560128,Yaw=7.514849,Roll=57.335400)
	if (RightShoulderRef) {
		RightShoulder = (*RightShoulderRef).Location;
	}

	FVector RightElbow{-6.794147f, -25.284803f, 4.265117f}; // (Pitch=-25.284803,Yaw=4.265117,Roll=-6.794147)
	if (RightElbowRef) {
		RightElbow = (*RightElbowRef).Location;
	}

	FVector RightWrist{5.380046f, 1.912979f, 4.532884f}; // (Pitch=1.912979,Yaw=4.532884,Roll=5.380046)
	if (RightWristRef) {
		RightWrist = (*RightWristRef).Location;
	}

	FVector RightIndex{10.892458f, -7.564259f, -6.664494f}; // (Pitch=-7.564259,Yaw=-6.664494,Roll=10.892458)
	if (RightIndexRef) {
		RightIndex = (*RightIndexRef).Location;
	}

	FVector RightPinky{10.461678f, 18.570060f, -6.571176f}; // (Pitch=18.570060,Yaw=-6.571176,Roll=10.461678)
	if (RightPinkyRef) {
		RightPinky = (*RightPinkyRef).Location;
	}

	FVector LeftShoulder{57.335567f, -3.560175f, -7.514699f}; // (Pitch=-3.560175,Yaw=-7.514699,Roll=57.335567)
	if (LeftShoulderRef) {
		LeftShoulder = (*LeftShoulderRef).Location;
	}

	FVector LeftElbow{-6.794358f, 25.284788f, -4.265412f}; // (Pitch=25.284788,Yaw=-4.265412,Roll=-6.794358)
	if (LeftElbowRef) {
		LeftElbow = (*LeftElbowRef).Location;
	}

	FVector LeftWrist{5.379777f, -1.912985f, -4.533254f}; // (Pitch=-1.912985,Yaw=-4.533254,Roll=5.379777)
	if (LeftWristRef) {
		LeftWrist = (*LeftWristRef).Location;
	}

	FVector LeftIndex{10.892813f, 7.564153f, 6.664984f}; // (Pitch=7.564153,Yaw=6.664984,Roll=10.892813)
	if (LeftIndexRef) {
		LeftIndex = (*LeftIndexRef).Location;
	}

	FVector LeftPinky{10.461994f, -18.570038f, 6.571603f}; // (Pitch=-18.570038,Yaw=6.571603,Roll=10.461994)
	if (LeftPinkyRef) {
		LeftPinky = (*LeftPinkyRef).Location;
	}

	// Arm rotation.
	auto LeftArmRotation = FQuat::FindBetweenVectors(LeftShoulder, LeftElbow).Euler();
	auto RightArmRotation = FQuat::FindBetweenVectors(RightShoulder, RightElbow).Euler();
	LeftArmRotation.Y = Get3dAngleInDegrees(RightShoulder, LeftShoulder, LeftElbow);
	RightArmRotation.Y = Get3dAngleInDegrees(LeftShoulder, RightShoulder, RightElbow);

	// Forearm rotation.
	auto LeftForearmRotation = FQuat::FindBetweenVectors(LeftElbow, LeftWrist).Euler();
	auto RightForearmRotation = FQuat::FindBetweenVectors(RightElbow, RightWrist).Euler();
	LeftForearmRotation.Y = Get3dAngleInDegrees(LeftShoulder, LeftElbow, LeftWrist);
	RightForearmRotation.Y = Get3dAngleInDegrees(RightShoulder, RightElbow, RightWrist);
	LeftForearmRotation.Z = FMath::Clamp(LeftForearmRotation.Z, FMath::RadiansToDegrees(-2.14f), 0.0f);
	RightForearmRotation.Z = FMath::Clamp(RightForearmRotation.Z, FMath::RadiansToDegrees(-2.14f), 0.0f);

	// Right hand rotation.
	auto LeftHandRotation = FQuat::FindBetweenVectors(LeftWrist, FMath::Lerp(LeftPinky, LeftIndex, 0.5f)).Euler();

	// Left hand rotation.
	auto RightHandRotation = FQuat::FindBetweenVectors(RightWrist, FMath::Lerp(RightPinky, RightIndex, 0.5f)).Euler();

	OutArmData.LeftArmPosition = LeftShoulder;
	OutArmData.RightArmPosition = RightShoulder;

	OutArmData.LeftForearmPosition = LeftElbow;
	OutArmData.RightForearmPosition = RightElbow;

	OutArmData.LeftHandPosition = LeftWrist;
	OutArmData.RightHandPosition = RightWrist;

	OutArmData.LeftArmRotation = FRotator::MakeFromEuler(LeftArmRotation);
	OutArmData.RightArmRotation = FRotator::MakeFromEuler(RightArmRotation);

	OutArmData.LeftForearmRotation = FRotator::MakeFromEuler(LeftForearmRotation);
	OutArmData.RightForearmRotation = FRotator::MakeFromEuler(RightForearmRotation);

	OutArmData.LeftHandRotation = FRotator::MakeFromEuler(LeftHandRotation);
	OutArmData.LeftHandRotation = FRotator::MakeFromEuler(RightHandRotation);
}
