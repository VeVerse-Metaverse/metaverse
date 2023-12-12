// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "L7LLSocketConnectionRunnable.h"

#include "L7LauncherLink.h"
#include "L7LLSocketConnection.h"
#include "VeShared.h"

FL7LLSocketConnectionRunnable::FL7LLSocketConnectionRunnable(FL7LLSocketConnection* InSocketConnection)
	: SocketConnection(InSocketConnection), bShouldStop(false) { }

FL7LLSocketConnectionRunnable::~FL7LLSocketConnectionRunnable() { }

uint32 FL7LLSocketConnectionRunnable::Run() {
	while (!bShouldStop) {
		if (SocketConnection) {
			FString Data = SocketConnection->ReceiveData();
			if (!Data.IsEmpty()) {
				// Process the received data
				VeLogWarningFunc("Received data: %s", *Data);
			}
		}

		constexpr float SecondsToWait = 0.01f;
		FPlatformProcess::Sleep(SecondsToWait); // Sleep for a short duration to avoid high CPU usage
	}

	return 0;
}

void FL7LLSocketConnectionRunnable::Stop() {
	bShouldStop = true;
}
