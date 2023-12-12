// Author: Nikolay Bulatov, Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#include "VeLiveLinkServerConnection.h"

// #include "VeShared.h"
#include "VeLiveLink.h"
#include "ILiveLinkClient.h"
#include "VeLiveLinkSource.h"
#include "VeLiveLinkTypes.h"
#include "VeLiveLinkRole.h"
#include "Sockets.h"
#include "IPAddress.h"


UVeLiveLinkServerConnection::UVeLiveLinkServerConnection(FVeLiveLinkSource* InSource, FSocket* InSocket)
	: Guid(FGuid::NewGuid())
	  , Source(InSource)
	  , Socket(InSocket)
	  , LastTimeDataReceived(0.0) {

	TSharedPtr<FInternetAddr> Addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	Socket->GetPeerAddress(*Addr);
	PeerAddressStr = Addr->ToString(true);

	// VeLog("New client connected: %s", *PeerAddressStr);

	MessageThread = MakeUnique<FVeLiveLinkMessageThread>(InSocket, Guid);

	MessageThread->OnFrameDataReady().BindRaw(this, &UVeLiveLinkServerConnection::OnFrameDataReady);
	MessageThread->OnStatusChanged().BindRaw(this, &UVeLiveLinkServerConnection::OnStatusChanged);
	MessageThread->OnConnectionLost().BindRaw(this, &UVeLiveLinkServerConnection::OnConnectionLost);
	MessageThread->OnConnectionFailed().BindRaw(this, &UVeLiveLinkServerConnection::OnConnectionFailed);

	MessageThread->Start();
}

void UVeLiveLinkServerConnection::Update() { }

bool UVeLiveLinkServerConnection::RequestDestroy() {
	FScopeLock Lock(&ThreadMutex);

	bPendingDestroy = true;

	if (MessageThread.IsValid()) {
		// Instruct the message thread to stop its message loop
		if (MessageThread->IsThreadRunning()) {
			MessageThreadShutdownTime = FPlatformTime::Seconds();
			MessageThread->Stop();
		}

		if ((FPlatformTime::Seconds() - MessageThreadShutdownTime) > MessageThreadShutdownTimeout) {
			MessageThread->ForceKill();
		}

		// If the message thread has stopped executing its message loop, then it is safe to reset it and destroy the socket
		if (!MessageThread->IsFinished()) {
			return false;
		}

		MessageThread.Reset();
	}

	if (Socket) {
		Socket->Close();
		Socket = nullptr;
		// VeLog("Connection closed: %s", *PeerAddressStr);
	}

	return true;
}

void UVeLiveLinkServerConnection::OnConnectionLost() {
	// VeLog("Connection lost: %s", *PeerAddressStr);
	RequestDestroy();
}

void UVeLiveLinkServerConnection::OnConnectionFailed() {
	// VeLog("Connection failed: %s", *PeerAddressStr);
	RequestDestroy();
}

void UVeLiveLinkServerConnection::OnStatusChanged() { }

void UVeLiveLinkServerConnection::UpdateStaticData(FName SubjectName) {
	FLiveLinkStaticDataStruct VeLiveLinkStaticDataStruct(FVeLiveLinkStaticData::StaticStruct());
	// FVeLiveLinkStaticData* VeLiveLinkStaticData = VeLiveLinkStaticDataStruct.Cast<FVeLiveLinkStaticData>();

	FLiveLinkSubjectKey SubjectKey(Source->GetGuid(), SubjectName);

	Source->GetClient()->PushSubjectStaticData_AnyThread(SubjectKey, UVeLiveLinkRole::StaticClass(), MoveTemp(VeLiveLinkStaticDataStruct));
}

void UVeLiveLinkServerConnection::OnFrameDataReady(FVeLiveLinkPacket InData, FName SubjectName) {
	FLiveLinkSubjectKey SubjectKey(Source->GetGuid(), SubjectName);

	{
		FScopeLock Lock(&ThreadMutex);
		if (SubjectName != CurrentSubjectName) {
			CurrentSubjectName = SubjectName;
			UpdateStaticData(SubjectName);
		}
	}

	FLiveLinkFrameDataStruct FrameDataStruct(FVeLiveLinkFrameData::StaticStruct());
	FVeLiveLinkFrameData* FrameData = FrameDataStruct.Cast<FVeLiveLinkFrameData>();

	LastTimeDataReceived = FPlatformTime::Seconds();
	FrameData->WorldTime = LastTimeDataReceived.load();
	FrameData->MetaData.SceneTime = InData.Header.FrameTime;

	// Face
	uint8 i(0);
	FVeLiveLinkFaceBlendshapeMap FaceBlendShapes;
	FaceBlendShapes.Reserve(InData.BlendshapePayload.Num());
	for (auto Payload : InData.BlendshapePayload) {
		FrameData->FaceBlendshapes.Emplace(static_cast<EVeLiveLinkFaceBlendshapeIndex>(i), Payload.Value);
		FaceBlendShapes.Emplace(static_cast<EVeLiveLinkFaceBlendshapeIndex>(i), Payload.Value);
		++i;
	}

	// Pose
	for (auto Payload : InData.PoseWorldLandmarkPayload) {
		FVeLiveLinkPoseLandmarks PoseWorldLandmarks{LocationFromMediapipe(FVector(Payload.X, Payload.Y, Payload.Z)), Payload.Visibility};
		FrameData->PoseLandmarks.Emplace(static_cast<EVeLiveLinkPoseLandmarkIndex>(Payload.Index), MoveTemp(PoseWorldLandmarks));
	}

	// Left Hand
	for (auto Payload : InData.LeftHandLandmarkPayload) {
		FrameData->LeftHandLandmarks.Emplace(static_cast<EVeLiveLinkHandLandmarkIndex>(Payload.Index), LocationFromMediapipe(FVector(Payload.X, Payload.Y, Payload.Z)));
	}

	// Right Hand
	for (auto Payload : InData.RightHandLandmarkPayload) {
		FrameData->RightHandLandmarks.Emplace(static_cast<EVeLiveLinkHandLandmarkIndex>(Payload.Index), LocationFromMediapipe(FVector(Payload.X, Payload.Y, Payload.Z)));
	}

	ARKitPublishBlendShapes(FName(TEXT("iPhone")), InData.Header.FrameTime, FaceBlendShapes);
	Source->GetClient()->PushSubjectFrameData_AnyThread(SubjectKey, MoveTemp(FrameDataStruct));
}

void UVeLiveLinkServerConnection::ARKitPublishBlendShapes(FName SubjectName, const FQualifiedFrameTime& FrameTime, const FVeLiveLinkFaceBlendshapeMap& FaceBlendShapes, FName DeviceId) {
	auto* Client = Source->GetClient();
	auto& SourceGuid = Source->GetGuid();

	// PrintLog(TEXT("VeVerse"), FrameTime.Time.FrameNumber.Value, FaceBlendShapes);

	// SCOPE_CYCLE_COUNTER(STAT_FaceAR_Local_PublishLiveLink);
	//
	// check(Client != nullptr);

	// This code touches UObjects so needs to be run only in the game thread
	// check(IsInGameThread());

	//If we can't retrieve blend shape enum, nothing we can do
	const UEnum* EnumPtr = StaticEnum<EVeLiveLinkFaceBlendshapeIndex>();
	if (EnumPtr == nullptr) {
		return;
	}

	const FLiveLinkSubjectKey SubjectKey(SourceGuid, SubjectName);


	// Is this a new device and subject pair?
	FBlendShapeStaticData* BlendShapeDataPtr = BlendShapePerDeviceMap.Find(DeviceId);
	if (BlendShapeDataPtr == nullptr) {
		ARKitUpdateStaticData(SubjectName, FaceBlendShapes, DeviceId);
	}
	// Did the subject name change for the device?
	else if (SubjectKey != BlendShapeDataPtr->SubjectKey) {
		// The remote device changed subject names, so remove the old subject
		Client->RemoveSubject_AnyThread(BlendShapeDataPtr->SubjectKey);

		ARKitUpdateStaticData(SubjectName, FaceBlendShapes, DeviceId);
	}

	FLiveLinkFrameDataStruct FrameDataStruct(FLiveLinkBaseFrameData::StaticStruct());
	FLiveLinkBaseFrameData* FrameData = FrameDataStruct.Cast<FLiveLinkBaseFrameData>();
	FrameData->WorldTime = FPlatformTime::Seconds();
	FrameData->MetaData.SceneTime = FrameTime;
	FrameData->PropertyValues.Reserve((int32)EVeLiveLinkFaceBlendshapeIndex::MAX);

	// Iterate through all of the blend shapes copying them into the LiveLink data type
	for (int32 Shape = 0; Shape < (int32)EVeLiveLinkFaceBlendshapeIndex::MAX; Shape++) {
		if (FaceBlendShapes.Contains((EVeLiveLinkFaceBlendshapeIndex)Shape)) {
			const float CurveValue = FaceBlendShapes.FindChecked((EVeLiveLinkFaceBlendshapeIndex)Shape);
			FrameData->PropertyValues.Add(CurveValue);
		}
	}

	//Blendshapes don't change over time. If they were to change, we would need to keep track
	//of previous values to always have frame data matching static data.
	check(FrameData->PropertyValues.Num() == BlendShapePerDeviceMap.FindChecked(DeviceId).StaticData.PropertyNames.Num());

	// Share the data locally with the LiveLink client
	Client->PushSubjectFrameData_AnyThread(SubjectKey, MoveTemp(FrameDataStruct));
}


void UVeLiveLinkServerConnection::ARKitUpdateStaticData(FName SubjectName, const FVeLiveLinkFaceBlendshapeMap& FaceBlendShapes, FName DeviceId /*= NAME_None*/) {
	auto* Client = Source->GetClient();
	auto& SourceGuid = Source->GetGuid();

	//Update the subject key to match latest one
	FBlendShapeStaticData& BlendShapeData = BlendShapePerDeviceMap.FindOrAdd(DeviceId);
	BlendShapeData.SubjectKey = FLiveLinkSubjectKey(SourceGuid, SubjectName);

	//Update property names array
	BlendShapeData.StaticData.PropertyNames.Reset((int32)EVeLiveLinkFaceBlendshapeIndex::MAX);

	//Iterate through all valid blend shapes to extract names
	const UEnum* EnumPtr = StaticEnum<EVeLiveLinkFaceBlendshapeIndex>();
	for (int32 Shape = 0; Shape < (int32)EVeLiveLinkFaceBlendshapeIndex::MAX; Shape++) {
		if (FaceBlendShapes.Contains((EVeLiveLinkFaceBlendshapeIndex)Shape)) {
			//Blendshapes don't change over time. If they were to change, we would need to keep track
			//of previous values to always have frame data matching static data
			const FName ShapeName = ParseEnumName(EnumPtr->GetNameByValue(Shape));
			BlendShapeData.StaticData.PropertyNames.Add(ShapeName);
		}
	}

	//Push the associated static data
	FLiveLinkStaticDataStruct StaticDataStruct(FLiveLinkBaseStaticData::StaticStruct());
	FLiveLinkBaseStaticData* BaseStaticData = StaticDataStruct.Cast<FLiveLinkBaseStaticData>();
	BaseStaticData->PropertyNames = BlendShapeData.StaticData.PropertyNames;

	UE_LOG(LogVeLiveLink, Verbose, TEXT("Pushing AppleARKit Subject '%s' with %d blend shapes"), *SubjectName.ToString(), BaseStaticData->PropertyNames.Num());
	Client->PushSubjectStaticData_AnyThread(BlendShapeData.SubjectKey, ULiveLinkBasicRole::StaticClass(), MoveTemp(StaticDataStruct));
}

FVector UVeLiveLinkServerConnection::LocationFromMediapipe(const FVector& InLocation) {
	// const FVector Location(InLocation * FVector(100.0f, 100.0f, -100.0f));
	// const FRotator Rotation(0.0f, -90.0f, 90.0f);
	// return Rotation.RotateVector(Location);

	// return FVector(InLocation.Z * -100.0f, InLocation.X * -100.0, InLocation.Y * -100.0f);

	return InLocation;
}

#include "Misc/Paths.h"

void UVeLiveLinkServerConnection::PrintLog(const FString& Name, int32 Frame, const FVeLiveLinkFaceBlendshapeMap& FaceBlendShapes) {
	TArray<FString> Cols;
	Cols.Reserve(FaceBlendShapes.Num() + 1);
	Cols.Emplace(Name);
	Cols.Emplace(FString::FromInt(Frame));

	for (auto& Elem : FaceBlendShapes) {
		if (Elem.Value) {
			Cols.Emplace(FString::SanitizeFloat(Elem.Value, 6));
		} else {
			Cols.Emplace(TEXT(""));
		}
	}

	const FString String(FString::Join(Cols, TEXT(";")) + TEXT("\n"));

	FString Filename = FPaths::ProjectLogDir() + TEXT("LiveLink.csv");
	FFileHelper::SaveStringToFile(String, *Filename, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);
}
