// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

class FL7LLSocketConnection {
public:
	FL7LLSocketConnection(const FString& InAddress, int32 InPort);
	~FL7LLSocketConnection();

	bool Connect();
	bool Reconnect();
	bool IsConnected() const;
	bool SendData(const FString& Data);
	FString ReceiveData();

private:
	FString Address;
	int32 Port;
	FSocket* Socket = nullptr;
};
