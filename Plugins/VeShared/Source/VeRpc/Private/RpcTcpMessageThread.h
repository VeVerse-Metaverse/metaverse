// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class FRunnable;
class FSocket;
class FVeNboSerializeFromBuffer;

struct FVeRpcMessage;

typedef TDelegate<void(FVeRpcMessage InMessage)> FOnMessageReceived;
typedef TDelegate<void()> FOnStatusChanged;
typedef TDelegate<void()> FOnConnectionLost;
typedef TDelegate<void()> FOnConnectionFailed;

/** 
 * @brief LE7EL RPC Protocol Constants
 */
struct FVeRpcProtocolConstants {
	static constexpr uint16 Header = 0x5652;
	static constexpr uint16 Version = 0x01;
};

struct FVeRpcMessage {
	struct FVeRpcPacketHeader {
		/**
		 * @brief Packet Header (should be 0x5652)
		 */
		uint16 Header = 0;
		/**
		 * @brief Packet Version (should be 0x0100)
		 */
		uint16 Version = 0;
		/**
		 * @brief Packet payload length
		 */
		uint32 PayloadLength = 0;

		/**
		 * @brief Read packet header from buffer.
		 */
		bool ReadHeader(FVeNboSerializeFromBuffer& Ar);

		/**
		 * @brief Get packet header size (in bytes).
		 */
		uint32 Size() const;
	};

	/**
	 * @brief Packet header.
	 */
	FVeRpcPacketHeader Header;

	/**
	 * @brief Packet payload.
	 */
	TArray<uint8> Payload;

	/**
	 * @brief Get packet size (in bytes).
	 */
	uint32 Size() const;
};

/**
 * @brief LE7EL RPC TCP Message Thread
 */
class FVeRpcTcpMessageThread final : public FRunnable {
public:
	FVeRpcTcpMessageThread(FSocket* InSocket);
	virtual ~FVeRpcTcpMessageThread() override;

	void Start();

	bool IsThreadRunning() const {
		return bIsThreadRunning;
	}

	bool IsFinished() const {
		return bIsFinished;
	}

	FOnMessageReceived& OnPacketDataReceived() {
		return MessageReceivedDelegate;
	}

	//~ FRunnable interface
	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	//~ End of FRunnable interface

	/**
	 * @brief Set socket to use for communication.
	 */
	void SetSocket(FSocket* InSocket);

	/**
	 * @brief Perform a soft reset of the thread.
	 */
	void SoftReset();

	/**
	 * @brief Perform a hard reset of the thread.
	 */
	void HardReset();

	/**
	 * @brief Force kill the thread.
	 */
	void ForceKill();

private:
	/**
	 * @brief Socket used to communicate with LE7EL RPC helper.
	 */
	FSocket* Socket;

	/**
	 * A thread to run the worker FRunnable on.
	 */
	TUniquePtr<FRunnableThread> Thread;

	/**
	 * @brief Is thread running?
	 */
	std::atomic<bool> bIsThreadRunning = false;

	/**
	 * @brief Has a soft reset been triggered?
	 */
	bool bSoftResetTriggered = false;

	/**
	 * @brief Has a hard reset been triggered?
	 */
	bool bHardResetTriggered = false;

	/**
	 * @brief Is thread finished?
	 */
	std::atomic<bool> bIsFinished = true;

	/**
	 * @brief Should thread be force killed?
	 */
	std::atomic<bool> bForceKillThread = false;

	FOnMessageReceived MessageReceivedDelegate;
	FOnStatusChanged StatusChangedDelegate;
	FOnConnectionLost ConnectionLostDelegate;
	FOnConnectionFailed ConnectionFailedDelegate;

	/**
	 * @brief Last time data was received from the socket (in seconds) (used to detect connection timeout).
	 */
	double LastTimeDataReceived = 0.0;

	/**
	 * @brief Maximum message length (in bytes).
	 */
	static constexpr uint32 MaxMessageLength = 4096;

	/**
	 * @brief Connection timeout (in seconds).
	 */
	static constexpr double ConnectionTimeout = 5.0;

	/**
	 * @brief Connection wait interval (in seconds). Used to wait for connection to be established.
	 */
	static constexpr double ConnectionWaitInterval = 0.1;
};
