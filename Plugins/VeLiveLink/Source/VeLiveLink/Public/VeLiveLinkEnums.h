// Author: Nikolay Bulatov, Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "VeLiveLinkEnums.generated.h"

UENUM(BlueprintType)
enum class EVeLiveLinkFaceBlendshapeIndex : uint8 {
	// Left eye blend shapes
	EyeBlinkLeft = 0,
	EyeLookDownLeft = 1,
	EyeLookInLeft = 2,
	EyeLookOutLeft = 3,
	EyeLookUpLeft = 4,
	EyeSquintLeft = 5,
	EyeWideLeft = 6,
	// Right eye blend shapes
	EyeBlinkRight = 7,
	EyeLookDownRight = 8,
	EyeLookInRight = 9,
	EyeLookOutRight = 10,
	EyeLookUpRight = 11,
	EyeSquintRight = 12,
	EyeWideRight = 13,
	// Jaw blend shapes
	JawForward = 14,
	JawLeft = 15,
	JawRight = 16,
	JawOpen = 17,
	// Mouth blend shapes
	MouthClose = 18,
	MouthFunnel = 19,
	MouthPucker = 20,
	MouthLeft = 21,
	MouthRight = 22,
	MouthSmileLeft = 23,
	MouthSmileRight = 24,
	MouthFrownLeft = 25,
	MouthFrownRight = 26,
	MouthDimpleLeft = 27,
	MouthDimpleRight = 28,
	MouthStretchLeft = 29,
	MouthStretchRight = 30,
	MouthRollLower = 31,
	MouthRollUpper = 32,
	MouthShrugLower = 33,
	MouthShrugUpper = 34,
	MouthPressLeft = 35,
	MouthPressRight = 36,
	MouthLowerDownLeft = 37,
	MouthLowerDownRight = 38,
	MouthUpperUpLeft = 39,
	MouthUpperUpRight = 40,
	// Brow blend shapes
	BrowDownLeft = 41,
	BrowDownRight = 42,
	BrowInnerUp = 43,
	BrowOuterUpLeft = 44,
	BrowOuterUpRight = 45,
	// Cheek blend shapes
	CheekPuff = 46,
	CheekSquintLeft = 47,
	CheekSquintRight = 48,
	// Nose blend shapes
	NoseSneerLeft = 49,
	NoseSneerRight = 50,
	TongueOut = 51,
	// Treat the head rotation as curves for LiveLink support
	HeadYaw = 52,
	HeadPitch = 53,
	HeadRoll = 54,
	// Treat eye rotation as curves for LiveLink support
	LeftEyeYaw = 55,
	LeftEyePitch = 56,
	LeftEyeRoll = 57,
	RightEyeYaw = 58,
	RightEyePitch = 59,
	RightEyeRoll = 60,
	MAX
};

UENUM(BlueprintType)
enum class EVeLiveLinkPoseLandmarkIndex : uint8 {
	Nose = 0,
	LeftEyeInner = 1,
	LeftEye = 2,
	LeftEyeOuter = 3,
	RightEyeInner = 4,
	RightEye = 5,
	RightEyeOuter = 6,
	LeftEar = 7,
	RightEar = 8,
	MouthLeft = 9,
	MouthRight = 10,
	LeftShoulder = 11,
	RightShoulder = 12,
	LeftElbow = 13,
	RightElbow = 14,
	LeftWrist = 15,
	RightWrist = 16,
	LeftPinky = 17,
	RightPinky = 18,
	LeftIndex = 19,
	RightIndex = 20,
	LeftThumb = 21,
	RightThumb = 22,
	LeftHip = 23,
	RightHip = 24,
	LeftKnee = 25,
	RightKnee = 26,
	LeftAnkle = 27,
	RightAnkle = 28,
	LeftHeel = 29,
	RightHeel = 30,
	LeftFootIndex = 31,
	RightFootIndex = 32,
	MAX
};

UENUM(BlueprintType)
enum class EVeLiveLinkHandLandmarkIndex : uint8 {
	Wrist = 0,
	ThumbCMC = 1,
	ThumbMCP = 2,
	ThumbIP = 3,
	ThumbTip = 4,
	IndexFingerMCP = 5,
	IndexFingerPIP = 6,
	IndexFingerDIP = 7,
	IndexFingerTip = 8,
	MiddleFingerMCP = 9,
	MiddleFingerPIP = 10,
	MiddleFingerDIP = 11,
	MiddleFingerTip = 12,
	RingFingerMCP = 13,
	RingFingerPIP = 14,
	RingFingerDIP = 15,
	RingFingerTip = 16,
	PinkyMCP = 17,
	PinkyPIP = 18,
	PinkyDIP = 19,
	PinkyTip = 20,
	MAX
};
