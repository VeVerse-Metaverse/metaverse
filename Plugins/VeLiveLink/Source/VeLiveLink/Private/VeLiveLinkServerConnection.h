// Author: Nikolay Bulatov, Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "CoreMinimal.h"
#include "VeLiveLinkEnums.h"
#include "Roles/LiveLinkAnimationTypes.h"
#include "VeLiveLinkMessageThread.h"


class FVeLiveLinkSource;

using FVeLiveLinkFaceBlendshapeMap = TMap<EVeLiveLinkFaceBlendshapeIndex, float>;


class UVeLiveLinkServerConnection : public TSharedFromThis<UVeLiveLinkServerConnection> {
public:
	UVeLiveLinkServerConnection(FVeLiveLinkSource* InSource, FSocket* InSocket);

	const FGuid& GetGuid() const { return Guid; }

	void Update();

	bool IsPendingDestroy() const { return bPendingDestroy; }
	bool RequestDestroy();

private:
	FGuid Guid;
	FVeLiveLinkSource* Source;
	FSocket* Socket;
	bool bPendingDestroy = false;
	FName CurrentSubjectName = NAME_None;
	FString PeerAddressStr;

	TUniquePtr<FVeLiveLinkMessageThread> MessageThread;
	FCriticalSection ThreadMutex;

	double MessageThreadShutdownTime;
	const float MessageThreadShutdownTimeout = 2.0f;

	std::atomic<double> LastTimeDataReceived;

	void OnConnectionLost();
	void OnConnectionFailed();
	void OnStatusChanged();

	void UpdateStaticData(FName SubjectName);
	void OnFrameDataReady(FVeLiveLinkPacket InData, FName SubjectName);

	void ARKitPublishBlendShapes(FName SubjectName, const FQualifiedFrameTime& FrameTime, const FVeLiveLinkFaceBlendshapeMap& FaceBlendShapes, FName DeviceId = NAME_None);
	void ARKitUpdateStaticData(FName SubjectName, const FVeLiveLinkFaceBlendshapeMap& FaceBlendShapes, FName DeviceId);

	/** Helper struct to contain BlendShape data per deviceId */
	struct FBlendShapeStaticData
	{
		FLiveLinkSubjectKey SubjectKey;
		FLiveLinkSkeletonStaticData StaticData;
	};

	/** Used to track names changes for a given device and keep track of property names contained in subject */
	TMap<FName, FBlendShapeStaticData> BlendShapePerDeviceMap;

	static FName ParseEnumName(FName EnumName)
	{
		const int32 BlendShapeEnumNameLength = 28; // Length of 'EVeLiveLinkBlendshapeIndex::'
		FString EnumString = EnumName.ToString();
		return FName(*EnumString.Right(EnumString.Len() - BlendShapeEnumNameLength));
	}

	FVector LocationFromMediapipe(const FVector& InLocation);

	void PrintLog(const FString& Name, int32 Frame, const FVeLiveLinkFaceBlendshapeMap& FaceBlendShapes);
};
