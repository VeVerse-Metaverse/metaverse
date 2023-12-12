// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

class FL7LLSocketConnection;

class FL7LLSocketConnectionRunnable : public FRunnable {
public:
	FL7LLSocketConnectionRunnable(FL7LLSocketConnection* InSocketConnection);
	virtual ~FL7LLSocketConnectionRunnable();

	virtual uint32 Run() override;
	virtual void Stop() override;

private:
	FL7LLSocketConnection* SocketConnection = nullptr;
	FThreadSafeBool bShouldStop = false;
};
