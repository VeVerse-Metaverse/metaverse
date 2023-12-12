// Author: Egor A. Pristavka

#pragma once

/** Used by the server code to parse startup options. */
class VESHARED_API FVeNetworkSubsystem {
public:
	/**
	 * Check if the game is in the standalone mode.
	 * @param bOutResult set to true if standalone.
	 * @returns True on success, false on error.
	 */
	static bool IsStandalone(bool& bOutResult);

	/**
	 * Check if the game is in the server mode.
	 * @param bOutResult set to true if server.
	 * @returns True on success, false on error.
	 */
	static bool IsServer(bool& bOutResult);

	/**
	 * Check if the game is in the client mode.
	 * @param bOutResult set to true if client.
	 * @returns True on success, false on error.
	 */
	static bool IsClient(bool& bOutResult);
};
