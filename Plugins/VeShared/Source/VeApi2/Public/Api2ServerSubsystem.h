// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "VeShared.h"
#include "Api2ServerRequest.h"
#include "Api2WorldRequest.h"


class VEAPI2_API FApi2ServerSubsystem final : public IModuleSubsystem {
public:
	static FName Name;

	virtual void Initialize() override;
	virtual void Shutdown() override;

#if 0
	/**
	 * Requests the server metadata by the id.
	 * @param InServerId Request server ID.
	 * @param InCallback Complete callback delegate.
	 */
	void GetServer(const FGuid& InServerId, TSharedRef<FOnServerRequestCompleted1> InCallback) const;
#endif
	/**
	 * Requests the server unregister.
	 * @param InServerId Request server id.
	 * @param InCallback Complete callback delegate.
	 */
	void Delete(const FGuid& InServerId, TSharedRef<FOnGenericRequestCompleted2> InCallback) const;

	/**
	 * Request to send heartbeat.
	 * @param InServerId Request server id.
	 * @param InCallback Complete callback delegate.
	 */
	void Heartbeat(const FGuid& InServerId, const FApi2RequestServerHeartbeatMetadata& InRequestMetadata, TSharedRef<FOnGenericRequestCompleted2> InCallback) const;

	/**
	 * Requests the scheduled space metadata for the server.
	 * @param InCallback Complete callback delegate.
	 */
	void GetScheduledWorld(TSharedRef<FOnWorldRequestCompleted2> InCallback) const;

	/**
	 * Requests the server metadata by the space id.
	 * @param InRequestMetadata Request metadata.
	 * @param InCallback Complete callback delegate.
	 */
	void GetMatchingServer(const FApi2RequestServerMatchMetadata& InRequestMetadata, TSharedRef<FOnServerRequestCompleted2> InCallback) const;

	/**
	 * Request to connect the new online player.
	 * @param InServerId Request server id.
	 * @param InPlayerId Request user id.
	 * @param InCallback Complete callback delegate.
	 */
	void PlayerConnect(const FGuid& InServerId, const FGuid& InPlayerId, TSharedRef<FOnGenericRequestCompleted2> InCallback) const;

	/**
	 * Request to unregister connected player.
	 * @param InServerId Request server id.
	 * @param InPlayerId Request user id.
	 * @param InCallback Complete callback delegate.
	 */
	void PlayerDisconnect(const FGuid& InServerId, const FGuid& InPlayerId, TSharedRef<FOnGenericRequestCompleted2> InCallback) const;
};
