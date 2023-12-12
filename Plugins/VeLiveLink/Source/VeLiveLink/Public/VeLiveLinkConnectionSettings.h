// Author: Nikolay Bulatov, Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "VeLiveLinkConnectionSettings.generated.h"


USTRUCT()
struct FVeLiveLinkConnectionSettings {
	GENERATED_BODY()

	/** The address to bind */
	UPROPERTY(EditAnywhere, Category = "Settings")
	FString BindAddress = FString(TEXT("localhost"));

	/** The port on which the binding socket listens */
	UPROPERTY(EditAnywhere, Category = "Settings")
	uint32 ListenPort = 11112;

	/** Maximum receive buffer size */
	UPROPERTY(EditAnywhere, Category = "Settings")
	int32 BufferSize = 512 * 1024;

	/** Number of pending connections to queue */
	UPROPERTY(EditAnywhere, Category = "Settings")
	int32 ConnectionsBacklogSize = 16;

	/** Max Number of connections to accept per frame */
	UPROPERTY(EditAnywhere, Category = "Settings")
	int32 MaxConnectionsAcceptPerFrame = 1;

	/** If true, call FSocket::SetReuseAddr when binding to allow the use of an already bound address/port */
	UPROPERTY(EditAnywhere, Category = "Settings")
	bool bReuseAddressAndPort = false;
};
