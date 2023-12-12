// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "L7LLSocketConnection.h"

#include "Sockets.h"
#include "VeShared.h"
#include "L7LauncherLink.h"
#include "SocketSubsystem.h"

FL7LLSocketConnection::FL7LLSocketConnection(const FString& InAddress, const int32 InPort)
	: Address(InAddress)
	  , Port(InPort) {
	ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
	Socket = SocketSubsystem->CreateSocket(NAME_Stream, TEXT("L7LLSocketConnection"), false);
}

FL7LLSocketConnection::~FL7LLSocketConnection() {
	if (Socket) {
		Socket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(Socket);
	}
}

bool FL7LLSocketConnection::Connect() {
	if (!Socket) {
		return false;
	}

	TSharedRef<FInternetAddr> Addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	bool bIsValid = false;
	if (Address == TEXT("localhost") || Address == TEXT("loopback") || Address == TEXT("127.0.0.1")) {
		Addr->SetLoopbackAddress();
		bIsValid = true;
	} else {
		Addr->SetIp(*Address, bIsValid);
	}
	Addr->SetPort(Port);

	if (!bIsValid) {
		VeLogErrorFunc("Invalid IP address: %s", *Address);
		return false;
	}

	return Socket->Connect(*Addr);
}

bool FL7LLSocketConnection::Reconnect() {
	if (Socket) {
		Socket->Close();
	}

	return Connect();
}

bool FL7LLSocketConnection::IsConnected() const {
	return Socket && Socket->GetConnectionState() == SCS_Connected;
}

bool FL7LLSocketConnection::SendData(const FString& Data) {
	if (!IsConnected()) {
		return false;
	}

	int32 BytesSent;
	return Socket->Send(reinterpret_cast<const uint8*>(TCHAR_TO_UTF8(*Data)), Data.Len(), BytesSent);
}

FString FL7LLSocketConnection::ReceiveData() {
	FString ReceivedData;

	if (!IsConnected()) {
		return ReceivedData;
	}

	int32 BufferSize = 1024;

	TArray<uint8> Data;
	Data.SetNumUninitialized(BufferSize);

	int32 BytesRead;
	while (Socket->Recv(Data.GetData(), Data.Num(), BytesRead)) {
		ReceivedData.Append(UTF8_TO_TCHAR(Data.GetData()), BytesRead);
	}

	return ReceivedData;
}
