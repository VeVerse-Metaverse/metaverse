// Copyright Epic Games, Inc. All Rights Reserved.

#include "RpcTcpMessageThread.h"

#include "Sockets.h"

#include "HAL/RunnableThread.h"
#include "VeNboSerializer.h"

bool FVeRpcMessage::FVeRpcPacketHeader::ReadHeader(FVeNboSerializeFromBuffer& Ar) {
	if (Ar.GetBufferSize() < sizeof Header + sizeof Version + sizeof PayloadLength) {
		return false;
	}

	Ar >> Header;
	Ar >> Version;
	if (Header != 0x5652 || Version != 0x0100) {
		return false;
	}

	Ar >> PayloadLength;

	return true;
}

uint32 FVeRpcMessage::FVeRpcPacketHeader::Size() const {
	return sizeof Header + sizeof Version + sizeof PayloadLength;
}

uint32 FVeRpcMessage::Size() const {
	return Header.Size() + Header.PayloadLength;
}

FVeRpcTcpMessageThread::FVeRpcTcpMessageThread(FSocket* InSocket)
	: Socket(InSocket) { }

FVeRpcTcpMessageThread::~FVeRpcTcpMessageThread() { }

void FVeRpcTcpMessageThread::Start() {
	bIsFinished = false;
	Thread.Reset(FRunnableThread::Create(this, TEXT("FVeRpcTcpMessageThread"), 0, TPri_Normal));
}

bool FVeRpcTcpMessageThread::Init() {
	bIsThreadRunning = true;
	return true;
}

uint32 FVeRpcTcpMessageThread::Run() {
	TArray<uint8> ReceiveBuffer;

	// Reserve space for the maximum amount of data that can be received in a single message on the socket.
	ReceiveBuffer.SetNumZeroed(MaxMessageLength);

	// Initialize the timer used to check for connection timeout.
	LastTimeDataReceived = FPlatformTime::Seconds();

	// Wait for connection to be established before starting to receive data from the socket.
	while (Socket->GetConnectionState() != SCS_Connected) {
		if (FPlatformTime::Seconds() - LastTimeDataReceived > ConnectionTimeout) {
			ConnectionFailedDelegate.ExecuteIfBound();
			bIsFinished = true;
			return 0;
		}

		// Yield the thread while waiting for the connection to be established.
		FPlatformProcess::Sleep(ConnectionWaitInterval);
	}

	// Update the last time data was received from the socket.
	LastTimeDataReceived = FPlatformTime::Seconds();

	// Total number of bytes read from the socket. This is used to keep track of how much data is in the buffer.
	int32 TotalBytesRead = 0;

	// Main thread loop.
	while (bIsThreadRunning && !bForceKillThread) {
		// Number of bytes read from the socket in the current iteration of the loop.
		int32 NumBytesRead = 0;

		// Read data from the socket into the buffer. Start reading from the end of the buffer to avoid overwriting data that hasn't been processed yet.
		if (Socket->Recv(ReceiveBuffer.GetData() + TotalBytesRead, MaxMessageLength - TotalBytesRead, NumBytesRead)) {
			// Update the total number of bytes read from the socket.
			TotalBytesRead += NumBytesRead;

			// Attempt to break up packets from the server until there are no full packets remaining in the buffer.
			// We assume that we can receive a packet in multiple chunks or a multiple packets in a single chunk.
			// We read full packets until there are no more full packets in the buffer, so we break out of the loop when we can't read a full packet.
			while (true) {
				FVeNboSerializeFromBuffer FromBuffer(ReceiveBuffer.GetData(), TotalBytesRead);

				FVeRpcMessage Message;

				// Read the header from the buffer.
				if (!Message.Header.ReadHeader(FromBuffer)) {
					break;
				}

				// Ensure that we received at least the minimum amount of data to constitute a valid packet before trying to parse it.
				const int32 MessageSize = Message.Size();
				if (TotalBytesRead < MessageSize) {
					break;
				}

				// Parse the packet.
				const int32 Offset = Message.Header.Size();

				// Read the payload from the buffer.
				if (const int32 PayloadLength = Message.Header.PayloadLength; PayloadLength > 0) {
					Message.Payload.SetNumZeroed(Message.Header.PayloadLength);
					const auto* Data = &ReceiveBuffer[Offset];
					FMemory::Memcpy(Message.Payload.GetData(), Data, PayloadLength);
				}

				// Update the last time data was received from the socket.
				LastTimeDataReceived = FPlatformTime::Seconds();

				// Execute the delegate to handle the connection status change.
				StatusChangedDelegate.ExecuteIfBound();

				// Execute the delegate to handle the received message.
				MessageReceivedDelegate.ExecuteIfBound(Message);

				// Remove the message from the buffer.
				ReceiveBuffer.RemoveAt(0, MessageSize, false);

				// Re-add the removed data to the end of the buffer to avoid overwriting data that hasn't been processed yet and maintain the buffer size.
				ReceiveBuffer.AddZeroed(MessageSize);

				// Update the total number of bytes read from the socket.
				TotalBytesRead -= MessageSize;
			}
		} else {
			// If the socket is not connected, the connection has been lost.
			if (Socket->GetConnectionState() != SCS_Connected) {
				ConnectionLostDelegate.ExecuteIfBound();
				break;
			}
		}

		const double CurrentTime = FPlatformTime::Seconds();

		// If the connection has been idle for longer than the timeout, the connection has been lost.
		if (const double TimeDelta = CurrentTime - LastTimeDataReceived; TimeDelta > ConnectionTimeout) {
			if (bSoftResetTriggered == false) {
				// First try to soft reset the connection.
				bSoftResetTriggered = true;
				SoftReset();
			} else if (bHardResetTriggered == false) {
				// If the soft reset fails, try to hard reset the connection.
				bHardResetTriggered = true;
				HardReset();
			}
		}
	}

	bIsFinished = true;

	return 0;
}

void FVeRpcTcpMessageThread::Stop() {
	bIsThreadRunning = false;
}

void FVeRpcTcpMessageThread::SetSocket(FSocket* InSocket) {
	Socket = InSocket;
}

void FVeRpcTcpMessageThread::SoftReset() {
	// Reset the last time data was received from the socket so that another reset does not occur immediately.
	LastTimeDataReceived = FPlatformTime::Seconds();
}

void FVeRpcTcpMessageThread::HardReset() {
	// Update this flag so that the main message loop stops executing and the thread can be marked as finished.
	bForceKillThread = true;

	// Execute the delegate to handle the connection status change and connection lost (to trigger a reconnect).
	ConnectionLostDelegate.ExecuteIfBound();
}

void FVeRpcTcpMessageThread::ForceKill() {
	// Update this flag so that the main message loop stops executing and the thread can be marked as finished.
	bForceKillThread = true;
}
