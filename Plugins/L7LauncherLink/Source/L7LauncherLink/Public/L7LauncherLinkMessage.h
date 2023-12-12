// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

struct FL7LauncherLinkMessagePayload {
public:
	/** @brief Deep link passed to the application dynamically. */
	FString DeepLink = TEXT("");
};

struct FL7LauncherLinkMessage {
public:
	/** @brief Protocol name. */
	FString Protocol = TEXT("L7Launcher");

	/** @brief Protocol version. */
	uint32 Version = 1;

	/** @brief Message payload. */
	FL7LauncherLinkMessagePayload Payload;

	void FromJson(const TSharedPtr<FJsonObject>& JsonObject);
};
