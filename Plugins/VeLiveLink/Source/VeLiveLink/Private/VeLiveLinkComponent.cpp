// Author: Nikolay Bulatov, Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#include "VeLiveLinkComponent.h"

#include "Net/UnrealNetwork.h"
#include "ILiveLinkClient.h"
#include "Roles/LiveLinkBasicRole.h"
#include "VeLiveLinkEnums.h"
#include "VeNboSerializer.h"
#include "GameFramework/Character.h"


UVeLiveLinkComponent::UVeLiveLinkComponent() {
	// PrimaryComponentTick.TickGroup = TG_DuringPhysics;
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.0f;
	// SetNetAddressable();
	SetIsReplicatedByDefault(true);

	bWantsInitializeComponent = true;

	LiveLinkSubjectName = TEXT("VeVerse_LiveLink");
}

void UVeLiveLinkComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UVeLiveLinkComponent, RawFrameData, COND_SkipOwner);
}

void UVeLiveLinkComponent::InitializeComponent() {
	Super::InitializeComponent();

	ILiveLinkClient* ThisFrameClient = nullptr;
	IModularFeatures& ModularFeatures = IModularFeatures::Get();
	if (ModularFeatures.IsModularFeatureAvailable(ILiveLinkClient::ModularFeatureName)) {
		ThisFrameClient = &IModularFeatures::Get().GetModularFeature<ILiveLinkClient>(ILiveLinkClient::ModularFeatureName);
	}
	LiveLinkClient_AnyThread = ThisFrameClient;


	// Prepare FrameData.FaceBlendshapes
	if (EnableFaceBlendshapes) {
		if (ReplicationFaceBlendshapes.Num()) {
			const TSet<EVeLiveLinkFaceBlendshapeIndex> Keys(ReplicationFaceBlendshapes);
			FrameData.FaceBlendshapes.Reserve(Keys.Num());
			for (const auto& Key : Keys) {
				FrameData.FaceBlendshapes.Emplace(Key, 0.0f);
			}
		} else {
			const uint8 Num(static_cast<int32>(EVeLiveLinkFaceBlendshapeIndex::MAX));
			FrameData.FaceBlendshapes.Reserve(Num);
			for (uint8 i = 0; i < Num; ++i) {
				FrameData.FaceBlendshapes.Emplace(static_cast<EVeLiveLinkFaceBlendshapeIndex>(i), 0.0f);
			}
		}
	}

	// Prepare FrameData.PoseLandmarks
	if (EnablePoseLandmarks) {
		if (ReplicationPoseLandmarks.Num()) {
			const TSet<EVeLiveLinkPoseLandmarkIndex> Keys(ReplicationPoseLandmarks);
			FrameData.PoseLandmarks.Reserve(Keys.Num());
			WorldPoseLandmarks.Reserve(Keys.Num());
			for (const auto& Key : Keys) {
				FrameData.PoseLandmarks.Emplace(Key, FVeLiveLinkPoseLandmarks());
				WorldPoseLandmarks.Emplace(Key, FVeLiveLinkPoseLandmarks());
			}
		} else {
			const uint8 Num(static_cast<int32>(EVeLiveLinkPoseLandmarkIndex::MAX));
			FrameData.PoseLandmarks.Reserve(Num);
			WorldPoseLandmarks.Reserve(Num);
			for (uint8 i = 0; i < Num; ++i) {
				FrameData.PoseLandmarks.Emplace(static_cast<EVeLiveLinkPoseLandmarkIndex>(i), FVeLiveLinkPoseLandmarks());
				WorldPoseLandmarks.Emplace(static_cast<EVeLiveLinkPoseLandmarkIndex>(i), FVeLiveLinkPoseLandmarks());
			}
		}
	}

	// Prepare FrameData.LeftHandLandmarks
	// Prepare FrameData.RightHandLandmarks
	if (EnableHandLandmarks) {
		if (ReplicationHandLandmarks.Num()) {
			const TSet<EVeLiveLinkHandLandmarkIndex> Keys(ReplicationHandLandmarks);
			FrameData.LeftHandLandmarks.Reserve(Keys.Num());
			FrameData.RightHandLandmarks.Reserve(Keys.Num());
			WorldLeftHandLandmarks.Reserve(Keys.Num());
			WorldRightHandLandmarks.Reserve(Keys.Num());
			for (const auto& Key : Keys) {
				FrameData.LeftHandLandmarks.Emplace(Key, FVector::ZeroVector);
				FrameData.RightHandLandmarks.Emplace(Key, FVector::ZeroVector);
				WorldLeftHandLandmarks.Emplace(Key, FVector::ZeroVector);
				WorldRightHandLandmarks.Emplace(Key, FVector::ZeroVector);
			}
		} else {
			const uint8 Num(static_cast<int32>(EVeLiveLinkHandLandmarkIndex::MAX));
			FrameData.LeftHandLandmarks.Reserve(Num);
			FrameData.RightHandLandmarks.Reserve(Num);
			WorldLeftHandLandmarks.Reserve(Num);
			WorldRightHandLandmarks.Reserve(Num);
			for (uint8 i = 0; i < Num; ++i) {
				FrameData.LeftHandLandmarks.Emplace(static_cast<EVeLiveLinkHandLandmarkIndex>(i), FVector::ZeroVector);
				FrameData.RightHandLandmarks.Emplace(static_cast<EVeLiveLinkHandLandmarkIndex>(i), FVector::ZeroVector);
				WorldLeftHandLandmarks.Emplace(static_cast<EVeLiveLinkHandLandmarkIndex>(i), FVector::ZeroVector);
				WorldRightHandLandmarks.Emplace(static_cast<EVeLiveLinkHandLandmarkIndex>(i), FVector::ZeroVector);
			}
		}
	}

	// Calculate size of RawFrameData
	const uint32 FaceBlandshapesSize = FrameData.FaceBlendshapes.Num() * sizeof(float);
	const uint32 PoseLandmarksSize = FrameData.PoseLandmarks.Num() * sizeof(FVeLiveLinkPoseLandmarks);
	const uint32 HandLandmarksSize = FrameData.LeftHandLandmarks.Num() * sizeof(FVector) * 2;
	RawFrameDataSize = FaceBlandshapesSize + PoseLandmarksSize + HandLandmarksSize;

#if !UE_SERVER
	FSlateApplication::Get().OnApplicationActivationStateChanged().AddUObject(this, &UVeLiveLinkComponent::OnWindowFocusChanged);
#endif
}

void UVeLiveLinkComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// UE_LOG(LogTemp, Log, TEXT(">>> [%p] Tick"), this);

	FLiveLinkSubjectFrameData SubjectFrameData;

	TSubclassOf<ULiveLinkRole> SubjectRole = LiveLinkClient_AnyThread->GetSubjectRole_AnyThread(LiveLinkSubjectName);
	if (SubjectRole) {
		if (LiveLinkClient_AnyThread->EvaluateFrame_AnyThread(LiveLinkSubjectName, ULiveLinkBasicRole::StaticClass(), SubjectFrameData)) {
			FLiveLinkFrameDataStruct FrameDataStruct(FVeLiveLinkFrameData::StaticStruct());
			auto* SourceFrameData = SubjectFrameData.FrameData.Cast<FVeLiveLinkFrameData>();
			UpdateFrameData(DeltaTime, SourceFrameData);
		}
	}
}

void UVeLiveLinkComponent::BeginPlay() {
	Super::BeginPlay();

	UpdateInput();
}

void UVeLiveLinkComponent::Server_LiveLinkFrameData_Implementation(const TArray<uint8>& InRawFrameData) {
	RawFrameData = InRawFrameData;
}

void UVeLiveLinkComponent::OnRep_RawFrameData() {
	// UE_LOG(LogTemp, Log, TEXT(">>> [%p] OnRep"), this);
	ReadFrameData(RawFrameData);
}

void UVeLiveLinkComponent::SetEnableInput(bool Value) {
	if (bEnableInput != Value) {
		bEnableInput = Value;
		UpdateInput();
	}
}

void UVeLiveLinkComponent::OnWindowFocusChanged(const bool bInIsFocused) {
	if (bIsFocused != bInIsFocused) {
		bIsFocused = bInIsFocused;
		UpdateInput();
	}
}

void UVeLiveLinkComponent::UpdateInput() {
	bool IsActiveInput = bEnableInput && bIsFocused && (EnableFaceBlendshapes || EnablePoseLandmarks || EnableHandLandmarks);
	// UE_LOG(LogTemp, Log, TEXT(">>> LIVELINK ACTIVE = %d"), IsActiveInput);
	SetComponentTickEnabled(IsActiveInput);
}

void UVeLiveLinkComponent::UpdateFrameData(float DeltaTime, FVeLiveLinkFrameData* InFrameData) {
	FVeNboSerializeToBuffer Buffer(RawFrameDataSize);

	// Face
	if (EnableFaceBlendshapes) {
		for (auto& Elem : FrameData.FaceBlendshapes) {
			if (auto* Payload = InFrameData->FaceBlendshapes.Find(Elem.Key)) {
				Elem.Value = *Payload;
			} else {
				Elem.Value = 0.0f;
			}
			Buffer << Elem.Value;
		}
	}

	// Pose
	if (EnablePoseLandmarks) {
		for (auto& Elem : FrameData.PoseLandmarks) {
			if (auto* Payload = InFrameData->PoseLandmarks.Find(Elem.Key)) {
				Elem.Value = *Payload;
			} else {
				Elem.Value = FVeLiveLinkPoseLandmarks();
			}
			UpdateLocationFromMediapipe(Elem.Value.Location);

			auto& WorldElem = WorldPoseLandmarks[Elem.Key];
			WorldElem.Location = GetComponentToWorld().TransformPosition(Elem.Value.Location);
			WorldElem.Visibility = Elem.Value.Visibility;

			Buffer << Elem.Value;
		}
	}

	if (EnableHandLandmarks) {
		// ResultHandLocation = HandLocation - WristFromHand + WristFromPose;
		// WristOffset = WristFromPose - WristFromHand;
		// ResultHandLocation = HandLocation + WristOffset

		FVector LeftWristOffset{};
		FVector RightWristOffset{};

		if (bBindHand) {
			FVector LeftPoseWrist{};
			FVector RightPoseWrist{};

			if (auto* Payload = FrameData.PoseLandmarks.Find(EVeLiveLinkPoseLandmarkIndex::LeftWrist)) {
				LeftPoseWrist = Payload->Location;
			}

			if (auto* Payload = FrameData.PoseLandmarks.Find(EVeLiveLinkPoseLandmarkIndex::RightWrist)) {
				RightPoseWrist = Payload->Location;
			}

			if (auto* Payload = InFrameData->LeftHandLandmarks.Find(EVeLiveLinkHandLandmarkIndex::Wrist)) {
				FVector LeftHandWrist = *Payload;
				UpdateLocationFromMediapipe(LeftHandWrist);
				LeftWristOffset = LeftPoseWrist - LeftHandWrist;
			}

			if (auto* Payload = InFrameData->RightHandLandmarks.Find(EVeLiveLinkHandLandmarkIndex::Wrist)) {
				FVector RightHandWrist = *Payload;
				UpdateLocationFromMediapipe(RightHandWrist);
				RightWristOffset = RightPoseWrist - RightHandWrist;
			}
		}

		// LeftHand
		for (auto& Elem : FrameData.LeftHandLandmarks) {
			if (auto* Payload = InFrameData->LeftHandLandmarks.Find(Elem.Key)) {
				Elem.Value = *Payload;
			} else {
				Elem.Value = FVector::ZeroVector;
			}

			UpdateLocationFromMediapipe(Elem.Value);
			if (bBindHand) {
				Elem.Value += LeftWristOffset;
			}

			WorldLeftHandLandmarks[Elem.Key] = GetComponentToWorld().TransformPosition(Elem.Value);

			Buffer << Elem.Value;
		}

		// RightHand
		for (auto& Elem : FrameData.RightHandLandmarks) {
			if (auto* Payload = InFrameData->RightHandLandmarks.Find(Elem.Key)) {
				Elem.Value = *Payload;
			} else {
				Elem.Value = FVector::ZeroVector;
			}

			UpdateLocationFromMediapipe(Elem.Value);
			if (bBindHand) {
				Elem.Value += RightWristOffset;
			}

			WorldRightHandLandmarks[Elem.Key] = GetComponentToWorld().TransformPosition(Elem.Value);

			Buffer << Elem.Value;
		}
	}

	// Replication data to server
	if (EnableReplication) {
		const double CurrentTime = FApp::GetCurrentTime();
		if (CurrentTime > ReplicationLastTime + ReplicationTime) {
			ReplicationLastTime = CurrentTime;
			Server_LiveLinkFrameData(Buffer.GetBuffer());
		}
	}

	// PrintLog(*InFrameData);
}

void UVeLiveLinkComponent::ReadFrameData(const TArray<uint8>& InRawFrameData) {
	FVeNboSerializeFromBuffer Buffer(InRawFrameData.GetData(), InRawFrameData.Num());

	// Face
	if (EnableFaceBlendshapes) {
		for (auto& Elem : FrameData.FaceBlendshapes) {
			Buffer >> Elem.Value;
		}
	}

	// Pose
	if (EnablePoseLandmarks) {
		for (auto& Elem : FrameData.PoseLandmarks) {
			Buffer >> Elem.Value;
			auto& WorldElem = WorldPoseLandmarks[Elem.Key];
			WorldElem.Location = GetComponentToWorld().TransformPosition(Elem.Value.Location);
			WorldElem.Visibility = Elem.Value.Visibility;
		}
	}

	// Left Hand
	if (EnableHandLandmarks) {
		for (auto& Elem : FrameData.LeftHandLandmarks) {
			Buffer >> Elem.Value;
			WorldLeftHandLandmarks[Elem.Key] = GetComponentToWorld().TransformPosition(Elem.Value);
		}

		// Right Hand
		for (auto& Elem : FrameData.RightHandLandmarks) {
			Buffer >> Elem.Value;
			WorldRightHandLandmarks[Elem.Key] = GetComponentToWorld().TransformPosition(Elem.Value);
		}
	}
}

void UVeLiveLinkComponent::UpdateLocationFromMediapipe(FVector& Location) {
	Location = FVector(
		Location.Z * -100.0f,
		Location.X * -100.0f,
		Location.Y * -100.0f
		);
}

void UVeLiveLinkComponent::PrintLog(const FVeLiveLinkFrameData& FrameData) {
	const int32 Frame = FrameData.MetaData.SceneTime.Time.FrameNumber.Value;

	TArray<FString> Cols;
	Cols.Reserve(7);
	Cols.Emplace(FString::FromInt(Frame));

	if (auto* Value = FrameData.PoseLandmarks.Find(EVeLiveLinkPoseLandmarkIndex::LeftPinky)) {
		Cols.Emplace(FString::SanitizeFloat(Value->Location.X, 6));
		Cols.Emplace(FString::SanitizeFloat(Value->Location.Y, 6));
		Cols.Emplace(FString::SanitizeFloat(Value->Location.Z, 6));
	}
	if (auto* Value = FrameData.PoseLandmarks.Find(EVeLiveLinkPoseLandmarkIndex::LeftIndex)) {
		Cols.Emplace(FString::SanitizeFloat(Value->Location.X, 6));
		Cols.Emplace(FString::SanitizeFloat(Value->Location.Y, 6));
		Cols.Emplace(FString::SanitizeFloat(Value->Location.Z, 6));
	}

	const FString String(FString::Join(Cols, TEXT(";")) + TEXT("\n"));

	FString Filename = FPaths::ProjectLogDir() + TEXT("LiveLink.csv");
	FFileHelper::SaveStringToFile(String, *Filename, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);
}
