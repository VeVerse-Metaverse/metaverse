// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "L7LLSocketConnectionManager.h"

#include "L7LLSocketConnection.h"
#include "L7LLSocketConnectionRunnable.h"

FL7LLSocketConnectionManager::FL7LLSocketConnectionManager(const FString& InAddress, int32 InPort) {
	SocketConnection = new FL7LLSocketConnection(InAddress, InPort);
	SocketConnectionRunnable = new FL7LLSocketConnectionRunnable(SocketConnection);
	SocketConnectionThread = nullptr;
}

FL7LLSocketConnectionManager::~FL7LLSocketConnectionManager() {
	StopConnection();
	delete SocketConnection;
	delete SocketConnectionRunnable;
}

void FL7LLSocketConnectionManager::Tick(float DeltaTime) {
	TSharedPtr<FJsonObject> JsonObject = ReceiveJson();
	if (JsonObject.IsValid()) {
		// Process the JSON
	}
}

bool FL7LLSocketConnectionManager::IsTickable() const {
	return true;
}

TStatId FL7LLSocketConnectionManager::GetStatId() const {
	RETURN_QUICK_DECLARE_CYCLE_STAT(FL7LLSocketConnectionManager, STATGROUP_Tickables);
}

void FL7LLSocketConnectionManager::StartConnection() {
	if (SocketConnection) {
		if (SocketConnection->Connect()) {
			SocketConnectionThread = FRunnableThread::Create(SocketConnectionRunnable, TEXT("L7LLSocketConnectionThread"));
		}
	}
}

void FL7LLSocketConnectionManager::StopConnection() {
	if (SocketConnectionThread) {
		SocketConnectionRunnable->Stop();
		SocketConnectionThread->WaitForCompletion();
		delete SocketConnectionThread;
		SocketConnectionThread = nullptr;
	}
}

void FL7LLSocketConnectionManager::SendJson(const TSharedPtr<FJsonObject>& JsonObject) {
	FString JsonString;
	TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&JsonString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter);
	SocketConnection->SendData(JsonString);
}

TSharedPtr<FJsonObject> FL7LLSocketConnectionManager::ReceiveJson() {
	FString ReceivedData = SocketConnection->ReceiveData();
	if (!ReceivedData.IsEmpty()) {
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ReceivedData);
		if (FJsonSerializer::Deserialize(Reader, JsonObject)) {
			JsonObject;
		}
	}

	return nullptr;
}
