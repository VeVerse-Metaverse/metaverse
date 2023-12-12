// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once
#include "ApiServerRequest.h"
#include "ApiSpaceRequest.h"
#include "VeShared.h"

class FApiUpdateServerMetadata;
class IApiBatchQueryRequestMetadata;

class VEAPI_API FApiServerSubsystem final : public IModuleSubsystem {
public:
	const static FName Name;

	virtual void Initialize() override;
	virtual void Shutdown() override;

	DECLARE_EVENT_OneParam(FApiServerSubsystem, FOnServerRegistered, const FApiServerMetadata& /*InMetadata*/);

	DECLARE_EVENT(FApiServerSubsystem, FOnServerUnregistered);

	DECLARE_EVENT_OneParam(FApiServerSubsystem, FOnServerPlayerConnected, const FGuid& /*InPlayerId*/);

	DECLARE_EVENT_OneParam(FApiServerSubsystem, FOnServerPlayerDisconnected, const FGuid& /*InPlayerId*/);

	/** Generic server registration event. */
	FOnServerRegistered& GetOnRegistered() { return OnServerRegistered; }

	/** Generic server un-registration event. */
	FOnServerUnregistered& GetOnUnregistered() { return OnServerUnregistered; }

	/** Generic server player connected event. */
	FOnServerPlayerConnected& GetOnServerPlayerConnected() { return OnServerPlayerConnected; }

	/** Generic server player disconnected event. */
	FOnServerPlayerDisconnected& GetOnDeleted() { return OnServerPlayerDisconnected; }

	/** Get server id currently active and registered in the backend. */
	FGuid GetRegisteredServerId() const;

	/**
	 * Requests the scheduled space metadata for the server.
	 * @param InCallback Complete callback delegate.
	 */
	// void GetScheduledWorld(TSharedRef<FOnSpaceRequestCompleted> InCallback);

	/**
	 * Requests a space metadata by its id.
	 * @param InCallback Complete callback delegate.
	 */
	void GetWorld(const FGuid& InId, TSharedRef<FOnSpaceRequestCompleted> InCallback);

	/**
	 * Requests the server metadata by the id.
	 * @param InId Request server ID.
	 * @param InCallback Complete callback delegate.
	 */
	// void GetServer(const FGuid& InId, TSharedRef<FOnServerRequestCompleted> InCallback);

	/**
	 * Requests the server metadata by the space id.
	 * @param InSpaceId Request space ID.
	 * @param InCallback Complete callback delegate.
	 */
	void GetMatchingServer(const FGuid& InSpaceId, TSharedRef<FOnServerRequestCompleted> InCallback) const;

	/**
	 * Requests the server metadata batch.
	 * @param InMetadata Request metadata, offset, limit, query.
	 * @param InCallback Complete callback delegate.
	 */
	bool GetBatch(const IApiBatchQueryRequestMetadata& InMetadata, TSharedPtr<FOnServerBatchRequestCompleted> InCallback);

	/**
	 * Requests the server metadata batch.
	 * @param InMetadata Request metadata, offset, limit, query.
	 * @param InCallback Complete callback delegate.
	 */
	bool GetBatchForUser(const IApiUserBatchRequestMetadata& InMetadata, TSharedPtr<FOnServerBatchRequestCompleted> InCallback);

	/**
	 * Requests the server registration.
	 * @param InMetadata Request metadata.
	 * @param InCallback Complete callback delegate.
	 */
	bool Create(const FApiUpdateServerMetadata& InMetadata, TSharedPtr<FOnServerRequestCompleted> InCallback) const;

	/**
	 * Requests server update.
	 * @param InMetadata Request metadata.
	 * @param InCallback Complete callback delegate.
	 */
	bool Update(const FApiUpdateServerMetadata& InMetadata, TSharedPtr<FOnServerRequestCompleted> InCallback) const;
	
	/**
	 * Requests the server unregister.
	 * @param InServerId Request server id.
	 * @param InCallback Complete callback delegate.
	 */
	// bool Delete(const FGuid& InServerId, TSharedPtr<FOnGenericRequestCompleted> InCallback) const;

	/**
	 * Request to connect the new online player.
	 * @param InServerId Request server id.
	 * @param InUserId Request user id.
	 * @param InCallback Complete callback delegate.
	 */
	bool Connect(const FGuid& InServerId, const FGuid& InUserId, TSharedPtr<FOnGenericRequestCompleted> InCallback);

	/**
	 * Request to unregister connected player.
	 * @param InServerId Request server id.
	 * @param InUserId Request user id.
	 * @param InCallback Complete callback delegate.
	 */
	bool Disconnect(const FGuid& InServerId, const FGuid& InUserId, TSharedPtr<FOnGenericRequestCompleted> InCallback);

	/**
	 * Request to send heartbeat.
	 * @param InServerId Request server id.
	 * @param InCallback Complete callback delegate.
	 */
	// bool Heartbeat(const FGuid& InServerId, const FString& InStatus, const FString& InDetails, TSharedPtr<FOnGenericRequestCompleted> InCallback) const;

private:
	FGuid RegisteredServerId;

	FOnServerRegistered OnServerRegistered;
	FOnServerUnregistered OnServerUnregistered;
	FOnServerPlayerConnected OnServerPlayerConnected;
	FOnServerPlayerDisconnected OnServerPlayerDisconnected;

	void BroadcastApiServerRegistered(const FApiServerMetadata& InMetadata) const;
	void BroadcastApiServerUnregistered() const;
	void BroadcastApiServerPlayerConnected(const FGuid& InPlayerId) const;
	void BroadcastApiServerPlayerDisconnected(const FGuid& InPlayerId) const;
};

typedef TSharedPtr<FApiServerSubsystem> FApiServerSubsystemPtr;
