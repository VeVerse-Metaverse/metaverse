// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

class FL7LLSocketConnectionRunnable;
class FL7LLSocketConnection;

class FL7LLSocketConnectionManager : public FTickableGameObject {
public:
	FL7LLSocketConnectionManager(const FString& InAddress, int32 InPort);
	virtual ~FL7LLSocketConnectionManager();

	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override;
	virtual TStatId GetStatId() const override;

	void StartConnection();
	void StopConnection();
	void SendJson(const TSharedPtr<FJsonObject>& JsonObject);
	TSharedPtr<FJsonObject> ReceiveJson();

private:
	FL7LLSocketConnection* SocketConnection = nullptr;
	FL7LLSocketConnectionRunnable* SocketConnectionRunnable = nullptr;
	FRunnableThread* SocketConnectionThread = nullptr;
};
