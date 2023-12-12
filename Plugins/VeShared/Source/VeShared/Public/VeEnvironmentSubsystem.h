// Author: Egor A. Pristavka

#pragma once

namespace VeEnvironment {
	/** The map name the server is running. */
	constexpr auto ServerMapName = TEXT("VE_SERVER_MAP");
	/** The game mode the server is running. */
	constexpr auto ServerGameMode = TEXT("VE_SERVER_MODE");
	/** The space id server is running. */
	constexpr auto ServerWorldId = TEXT("VE_SERVER_WORLD_ID");
	/** The space id server is running. */
	constexpr auto ServerId = TEXT("VE_SERVER_ID");
	/** The IP address or DNS of the server. */
	constexpr auto ServerHost = TEXT("VE_SERVER_HOST");
	/** The internal UDP port of the server. */
	constexpr auto ServerPort = TEXT("VE_SERVER_PORT");
	/** The external public UDP port of the server. */
	constexpr auto ServerPublicPort = TEXT("VE_SERVER_PUBLIC_PORT");
	/** Max players number. */
	constexpr auto ServerMaxPlayers = TEXT("VE_SERVER_MAX_PLAYERS");
	/** The API key of the server. */
	constexpr auto ServerApiKey = TEXT("VE_SERVER_API_KEY");
	/** The API secret of the server. */
	constexpr auto ServerApiSecret = TEXT("VE_SERVER_API_SECRET");
	/** The API email of the server. */
	constexpr auto ServerApiEmail = TEXT("VE_SERVER_API_EMAIL");
	/** The API password of the server. */
	constexpr auto ServerApiPassword = TEXT("VE_SERVER_API_PASSWORD");
}

/** Used by the server code to parse startup options. */
class VESHARED_API FVeEnvironmentSubsystem {
public:
	/** Returns the environment variable value as a string. */
	static FString GetEnvironmentVariable(const FString& Key);

	static bool IsMobilePlatform();
	static bool IsDesktopPlatform();
};
