// Author: Egor A. Pristavka. 
// Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#include "NetworkCompatibilitySubsystem.h"

#include "Misc/NetworkVersion.h"
#include "VeShared.h"

void UVeNetworkCompatibilitySubsystem::Initialize(FSubsystemCollectionBase& Collection) {
	Super::Initialize(Collection);
	FNetworkVersion::IsNetworkCompatibleOverride.BindWeakLambda(this, [](uint32 LocalNetworkVersion, uint32 RemoteNetworkVersion) {
		LogWarningF("Network version compare disabled");
		return true;
	});
}
