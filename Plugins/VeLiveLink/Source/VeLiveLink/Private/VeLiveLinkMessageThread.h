// Author: Nikolay Bulatov, Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "CoreMinimal.h"
#include "VeLiveLinkSourceSettings.h"


class FRunnable;
class FSocket;
class FVeNboSerializeFromBuffer;
class FNboSerializeFromBuffer;

struct FVeLiveLinkPacket;

DECLARE_DELEGATE_TwoParams(FOnFrameDataReady, FVeLiveLinkPacket InData, FName SubjectName);
DECLARE_DELEGATE(FOnStatusChanged);
DECLARE_DELEGATE(FOnConnectionLost)
DECLARE_DELEGATE(FOnConnectionFailed)

/** 
 * @brief VeVerse LiveLink Protocol Constants
 */
struct FVeLiveLinkProtocol {
	static constexpr uint16 Header = 0x564C;
	static constexpr uint16 Version = 0x01;
};

/**
 * @brief VeVerse LiveLink Protocol Packet Data
 */
struct FVeLiveLinkPacketBlendshapeData {
	// Blendshape Index
	// uint32 Index = 0;
	// Blendshape Value
	float Value = 0;
};

struct FVeLiveLinkPacketPoseWorldLandmarkData {
	uint32 Index = 0;
	float X = 0;
	float Y = 0;
	float Z = 0;
	float Visibility = 0;

	FVector ToVector() const {
		return FVector(X, Y, Z);
	}
};

struct FVeLiveLinkPacketHandLandmarkData {
	uint32 Index = 0;
	float X = 0;
	float Y = 0;
	float Z = 0;
};

/**
 * @brief VeVerse LiveLink Protocol Packet
 */
struct FVeLiveLinkPacket {
	struct FVeLiveLinkPacketHeader {
		/**
		 * @brief Packet Header (should be 0x564C)
		 */
		uint16 Header = 0;
		/**
		 * @brief Packet Version (should be 0x0100)
		 */
		uint16 Version = 0;
		/**
		 * @brief SubjectName string length
		 */
		FName SubjectName;
		/**
		 * @brief FrameTime
		 */
		FQualifiedFrameTime FrameTime;
		/**
		 * @brief Packet blendshape payload length
		 */
		uint16 BlendshapePayloadLength = 0;
		/**
		 * @brief Packet pose world landmark payload length
		 */
		uint16 PoseWorldLandmarkPayloadLength = 0;
		/**
		 * @brief Packet left hand landmark payload length
		 */
		uint16 LeftHandLandmarkPayloadLength = 0;
		/**
		 * @brief Packet right hand landmark payload length
		 */
		uint16 RightHandLandmarkPayloadLength = 0;

		bool ReadHeader(FVeNboSerializeFromBuffer& Ar);

		/** Header size (bytes) */
		int32 Size() const;

	private:
		int32 SubjectNameSize = 0;
	};

	/**
	 * Packet header
	 */
	FVeLiveLinkPacketHeader Header;
	/**
	 * @brief Packet blendshape payload, vector of blendshape data
	 */
	TArray<FVeLiveLinkPacketBlendshapeData> BlendshapePayload;
	/**
	 * @brief Packet pose world landmark payload, vector of pose world landmark data
	 */
	TArray<FVeLiveLinkPacketPoseWorldLandmarkData> PoseWorldLandmarkPayload;
	/**
	 * @brief Packet left hand landmark payload, vector of left hand landmark data
	 */
	TArray<FVeLiveLinkPacketHandLandmarkData> LeftHandLandmarkPayload;
	/**
	 * @brief Packet right hand landmark payload, vector of right hand landmark data
	 */
	TArray<FVeLiveLinkPacketHandLandmarkData> RightHandLandmarkPayload;

	/** Packet size (bytes) */
	int32 Size() const;

	int32 BlendshapePayloadSize() const {
		return Header.BlendshapePayloadLength * BlendshapePayload.GetTypeSize();
	}

	int32 PoseWorldLandmarkPayloadSize() const {
		return Header.PoseWorldLandmarkPayloadLength * PoseWorldLandmarkPayload.GetTypeSize();
	}

	int32 LeftHandLandmarkPayloadSize() const {
		return Header.LeftHandLandmarkPayloadLength * LeftHandLandmarkPayload.GetTypeSize();
	}

	int32 RightHandLandmarkPayloadSize() const {
		return Header.RightHandLandmarkPayloadLength * RightHandLandmarkPayload.GetTypeSize();
	}
};

class FVeLiveLinkMessageThread : public FRunnable {
public:
	FVeLiveLinkMessageThread(FSocket* InSocket, const FGuid& InConnectionId);
	~FVeLiveLinkMessageThread();

	FGuid ConnectionId;

	void Start();

	bool IsThreadRunning() const {
		return bIsThreadRunning;
	}

	bool IsFinished() const {
		return bIsFinished;
	}

	/**
	 * Returns a delegate that is executed when frame data is ready.
	 *
	 * @return The delegate.
	 */
	FOnFrameDataReady& OnFrameDataReady() {
		return FrameDataReadyDelegate;
	}

	FOnStatusChanged& OnStatusChanged() {
		return StatusChangedDelegate;
	}

	FOnConnectionLost& OnConnectionLost() {
		return ConnectionLostDelegate;
	}

	FOnConnectionFailed& OnConnectionFailed() {
		return ConnectionFailedDelegate;
	}

public:
	//~ FRunnable Interface
	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	// End FRunnable Interface

	void SetSocket(FSocket* InSocket);

	void SoftReset();

	void ForceKill();

private:
	void HardReset();

private:
	FSocket* Socket;

	TUniquePtr<FRunnableThread> Thread;
	std::atomic<bool> bIsThreadRunning = false;

	FOnFrameDataReady FrameDataReadyDelegate;
	FOnStatusChanged StatusChangedDelegate;
	FOnConnectionLost ConnectionLostDelegate;
	FOnConnectionFailed ConnectionFailedDelegate;

	// FCriticalSection CriticalSection;

	double LastTimeDataReceived = 0.0;

	bool bSoftResetTriggered = false;
	bool bHardResetTriggered = false;

	std::atomic<bool> bIsFinished = true;
	std::atomic<bool> bForceKillThread = false;

private:
	static constexpr uint32 MaximumMessageLength = 4096;

	static constexpr uint32 ThreadStackSize = 1024 * 128;

	static constexpr float ConnectionWaitInterval = 0.02f; // 20ms
	static constexpr double ConnectionTimeout = 5.0;       // 3sec
	static constexpr double DataReceivedTimeout = 30.0;    // 2sec
};
