// Author: Nikolay Bulatov, Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#include "VeLiveLinkMessageThread.h"

#include "Sockets.h"

#include "HAL/RunnableThread.h"
#include "VeNboSerializer.h"


DEFINE_LOG_CATEGORY_STATIC(LogVeVerseMessageThread, Log, All);

bool FVeLiveLinkPacket::FVeLiveLinkPacketHeader::ReadHeader(FVeNboSerializeFromBuffer& Ar) {
	{
		const uint16 VersionSize = sizeof(Header) + sizeof(Version);
		if (Ar.GetBufferSize() < VersionSize + sizeof(SubjectNameSize)) {
			return false;
		}

		Ar >> Header;
		Ar >> Version;
		if (Header != 0x564c || Version != 0x0001) {
			return false;
		}

		Ar >> SubjectNameSize;

		if (Ar.GetBufferSize() < Size()) {
			return false;
		}

		Ar.Seek(VersionSize);
		Ar >> SubjectName;
		Ar >> FrameTime;

		Ar >> BlendshapePayloadLength;
		Ar >> PoseWorldLandmarkPayloadLength;
		Ar >> LeftHandLandmarkPayloadLength;
		Ar >> RightHandLandmarkPayloadLength;

		return true;
	}
}

int32 FVeLiveLinkPacket::FVeLiveLinkPacketHeader::Size() const {
	return sizeof(Header) + sizeof(Version)
		+ sizeof(SubjectNameSize) + SubjectNameSize
		+ sizeof(FQualifiedFrameTime)
		+ sizeof(BlendshapePayloadLength)
		+ sizeof(PoseWorldLandmarkPayloadLength)
		+ sizeof(LeftHandLandmarkPayloadLength)
		+ sizeof(RightHandLandmarkPayloadLength);
}

int32 FVeLiveLinkPacket::Size() const {
	return Header.Size()
		+ BlendshapePayloadSize()
		+ PoseWorldLandmarkPayloadSize()
		+ LeftHandLandmarkPayloadSize()
		+ RightHandLandmarkPayloadSize();
}

FVeLiveLinkMessageThread::FVeLiveLinkMessageThread(FSocket* InSocket, const FGuid& InConnectionId)
	: ConnectionId(InConnectionId), Socket(InSocket) {}

FVeLiveLinkMessageThread::~FVeLiveLinkMessageThread() {
	if (Thread != nullptr) {
		Thread->Kill(true);
	}
}

void FVeLiveLinkMessageThread::SetSocket(FSocket* InSocket) {
	Socket = InSocket;
	bSoftResetTriggered = false;
	bHardResetTriggered = false;
	LastTimeDataReceived = FPlatformTime::Seconds();
}

void FVeLiveLinkMessageThread::Start() {
	bIsFinished = false;

	Thread.Reset(FRunnableThread::Create(this, TEXT("VeVerse Live Link Message Thread"), ThreadStackSize, TPri_AboveNormal));
}

bool FVeLiveLinkMessageThread::Init() {
	bIsThreadRunning = true;
	return true;
}

void FVeLiveLinkMessageThread::Stop() {
	bIsThreadRunning = false;
}

uint32 FVeLiveLinkMessageThread::Run() {
	// Reserve space for the maximum amount of data that we can receive on the socket at once
	TArray<uint8> ReceiveBuffer;
	ReceiveBuffer.SetNumZeroed(MaximumMessageLength);

	// Initialize the timer used to tell if the socket connection has gone idle
	LastTimeDataReceived = FPlatformTime::Seconds();

	// Wait for the Socket to be fully connected (in a readable and writable state)
	while (Socket->GetConnectionState() != ESocketConnectionState::SCS_Connected) {
		if ((FPlatformTime::Seconds() - LastTimeDataReceived) > ConnectionTimeout) {
			ConnectionFailedDelegate.ExecuteIfBound();
			bIsFinished = true;
			return 0;
		}

		// Yield execution of this thread for the specified interval
		FPlatformProcess::Sleep(ConnectionWaitInterval);
	}

	LastTimeDataReceived = FPlatformTime::Seconds();

	// Total number of bytes read from the socket. This is used to keep track of how much data is in the buffer.
	int32 TotalBytesRead = 0;

	// Main thread loop.
	while ((bIsThreadRunning) && !bForceKillThread) {
		// Number of bytes read from the socket in the current iteration of the loop. 
		int32 NumBytesRead = 0;

		// Read data from the socket into the buffer. Start reading from the end of the buffer to avoid overwriting data that hasn't been processed yet.
		if (Socket->Recv(ReceiveBuffer.GetData() + TotalBytesRead, MaximumMessageLength - TotalBytesRead, NumBytesRead)) {
			// Update the total number of bytes read from the socket. 
			TotalBytesRead += NumBytesRead;

			// Attempt to break up packets from the server until there are no full packets remaining in the buffer.
			// We assume that we can receive a packet in multiple chunks or a multiple packets in a single chunk.
			// We read full packets until there are no more full packets in the buffer, so we break out of the loop when we can't read a full packet.
			while (true) {
				FVeNboSerializeFromBuffer FromBuffer(ReceiveBuffer.GetData(), TotalBytesRead);

				FVeLiveLinkPacket Packet;

				if (!Packet.Header.ReadHeader(FromBuffer)) {
					break;
				}

				// Ensure that we received at least the minimum amount of data to constitute a valid packet before trying to parse it
				const int32 PacketSize = Packet.Size();
				if (TotalBytesRead < PacketSize) {
					break;
				}

				// Parse BlendshapePayload data
				if (const int32 Size = Packet.BlendshapePayloadSize()) {
					Packet.BlendshapePayload.SetNum(Packet.Header.BlendshapePayloadLength);
					FromBuffer.ReadBinary(reinterpret_cast<uint8*>(Packet.BlendshapePayload.GetData()), Size);
				}

				// Parse PoseWorldLandmarkPayload data
				if (const int32 Size = Packet.PoseWorldLandmarkPayloadSize()) {
					Packet.PoseWorldLandmarkPayload.SetNum(Packet.Header.PoseWorldLandmarkPayloadLength);
					FromBuffer.ReadBinary(reinterpret_cast<uint8*>(Packet.PoseWorldLandmarkPayload.GetData()), Size);
				}

				// Parse LeftHandLandmarkPayload data
				if (const int32 Size = Packet.LeftHandLandmarkPayloadSize()) {
					Packet.LeftHandLandmarkPayload.SetNum(Packet.Header.LeftHandLandmarkPayloadLength);
					FromBuffer.ReadBinary(reinterpret_cast<uint8*>(Packet.LeftHandLandmarkPayload.GetData()), Size);
				}

				// Parse RightHandLandmarkPayload data
				if (const int32 Size = Packet.RightHandLandmarkPayloadSize()) {
					Packet.RightHandLandmarkPayload.SetNum(Packet.Header.RightHandLandmarkPayloadLength);
					FromBuffer.ReadBinary(reinterpret_cast<uint8*>(Packet.RightHandLandmarkPayload.GetData()), Size);
				}

				// Update the timer.
				LastTimeDataReceived = FPlatformTime::Seconds();

				StatusChangedDelegate.ExecuteIfBound();

				// Execute the delegate that will process the packet.
				FrameDataReadyDelegate.ExecuteIfBound(Packet, Packet.Header.SubjectName);

				ReceiveBuffer.RemoveAt(0, PacketSize, false);
				ReceiveBuffer.AddZeroed(PacketSize);

				TotalBytesRead -= PacketSize;
			}

		} else if (Socket->GetConnectionState() != ESocketConnectionState::SCS_Connected) {
			ConnectionLostDelegate.ExecuteIfBound();
			break;
		}

		const double CurrentTime = FPlatformTime::Seconds();
		const double TimeDelta = CurrentTime - LastTimeDataReceived;

		// If the amount of time since the last packet was received from the server exceeds the timeout, assume that the communication flow is broken and attempt to reset it
		if (TimeDelta > DataReceivedTimeout) {
			// First, try a soft reset, which will re-initialize the command queue
			if (bSoftResetTriggered == false) {
				bSoftResetTriggered = true;
				SoftReset();
			}
			// If a soft reset does not work, assume that the connection to the socket is broken and trigger a hard reset to establish a new connection
			else if (bHardResetTriggered == false) {
				bHardResetTriggered = true;
				HardReset();
			}
		}
	}

	bIsFinished = true;

	return 0;
}

void FVeLiveLinkMessageThread::SoftReset() {
	// Reset the timer so that another reset does not immediately trigger
	LastTimeDataReceived = FPlatformTime::Seconds();
}

void FVeLiveLinkMessageThread::HardReset() {
	// Update this flag so that the main message loop stops executing and the thread can be marked as finished
	bForceKillThread = true;

	// Notify the Source that the connection has been lost so that it can attempt to re-establish it.
	ConnectionLostDelegate.ExecuteIfBound();
}

void FVeLiveLinkMessageThread::ForceKill() {
	// Update this flag so that the main message loop stops executing and the thread can be marked as finished
	bForceKillThread = true;
}
