// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "VeShared.h"
#include "WebSocketsModule.h"
#include "IWebSocket.h"
#include "RpcWebSocketsServiceTypes.h"
#include "ArtheonWebSocketsPayload.h"

typedef TDelegate<void(const FString& Method, const TSharedPtr<FJsonObject>& BodyJsonObject)> FRpcWebSocketsMessageCallback;

class VERPC_API FRpcWebSocketsServiceSubsystem final : public IModuleSubsystem {
	class FRpcWebSocketsServiceEvent : public TMulticastDelegate<void()> {
		friend class FRpcWebSocketsServiceSubsystem;
	};

	class FRpcWebSocketsServicePushEvent : public TMulticastDelegate<void(ERpcWebSocketsMessageTopic Topic, const FString& Method, const TSharedPtr<FJsonObject>& JsonObject)> {
		friend class FRpcWebSocketsServiceSubsystem;
	};

public:
	const static FName Name;
	virtual void Initialize() override;
	virtual void Shutdown() override;

	FRpcWebSocketsServiceSubsystem();

	/** The endpoint used for WebSocket messaging. */
	FString WebSocketsEndpoint;

	/** The endpoint used for WebSocket messaging. */
	FString WebSocketsProtocol;

	FRpcWebSocketsServiceEvent OnConnected;
	FRpcWebSocketsServiceEvent OnDisconnected;
	FRpcWebSocketsServiceEvent OnLoggedIn;
	FRpcWebSocketsServiceEvent OnLoggedOut;
	FRpcWebSocketsServicePushEvent OnPushed;

	/** Returns true if service is fully initialized and ready to go. */
	bool IsConnected() const;

	/** Returns true if service has authenticated within the RPC framework. */
	bool IsAuthenticated() const {
		return bAuthenticated;
	}

	FString GetApiKey() const {
		return ApiKey;
	}

	void Connect(bool InIsServer = false);

	void Disconnect();

	/** Public version, requires the websockets to be authenticated. */
	bool SendMessage(const FRpcWebSocketsMessage& Message, TSharedPtr<FRpcWebSocketsMessageCallback> InCallback);

	static bool CheckStatus(const TSharedPtr<FJsonObject>& BodyJsonObject, FString& OutStatus, FString& OutMessage);

protected:
	void WebSocketsLogin();
	void WebSocketsNotifyNameChanged();
	void OnWebSocketsLoginComplete(const FString& Method, const TSharedPtr<FJsonObject>& BodyJsonObject);

	int ConnectRetries = 3;
	TSharedPtr<IWebSocket> WebSocketPtr = nullptr;

	TMap<FGuid, TSharedPtr<FRpcWebSocketsMessageCallback>> RequestCallbacks;

	static FWebSocketsModule* GetWebSocketsModule();

	// Delegate callback.
	void WebSocket_OnMessageReceive(const FString& MessagesStr);

	// Delegate callback.
	void WebSocket_OnMessageSent(const FString& Message);

	// Delegate callback.
	void WebSocket_OnConnected();

	// Delegate callback.
	void WebSocket_OnConnectionError(const FString& Error);

	// Delegate callback.
	void WebSocket_OnClosed(int32 StatusCode, const FString& Reason, bool bWasClean);

	void ProcessReceivedWebSocketsMessage(const FString& MessageStr);

	/** Internal version, skips the authentication check. */
	bool SendMessageInternal(const FRpcWebSocketsMessage& Message, TSharedPtr<FRpcWebSocketsMessageCallback> InCallback);

private:
	FString ApiKey;
	bool bAuthenticated = false;
	bool bIsServer = false;
	bool bConnectionInProcess = false;

	FDelegateHandle AuthSubsystem_OnChangeAuthorizationStateHandle;

};
