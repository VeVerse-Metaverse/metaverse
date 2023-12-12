// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "RpcWebSocketsServiceSubsystem.h"

#include "VeApi2.h"
#include "Api2AuthSubsystem.h"
#include "IWebSocket.h"
#include "WebSocketsModule.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "VeShared.h"
#include "RpcWebSocketsServiceTypes.h"

const FName FRpcWebSocketsServiceSubsystem::Name = FName("WebSocketsRpcServiceSubsystem");

FRpcWebSocketsServiceSubsystem::FRpcWebSocketsServiceSubsystem() {
#if WITH_RPC

#if UE_BUILD_DEBUG
	WebSocketsEndpoint = TEXT("ws://dev.rpc.veverse.com/ws");
#elif UE_BUILD_DEVELOPMENT
	WebSocketsEndpoint = TEXT("ws://dev.rpc.veverse.com/ws");
#elif UE_BUILD_TEST
	WebSocketsEndpoint = TEXT("ws://test.rpc.veverse.com/ws");
#elif UE_BUILD_SHIPPING
	WebSocketsEndpoint = TEXT("ws://test.rpc.veverse.com/ws");
#endif

	WebSocketsProtocol = TEXT("ws");
#endif

	VeLog("Starting WebSockets service. RPC Enabled: %d", WITH_RPC);
}

void FRpcWebSocketsServiceSubsystem::Initialize() {
	// if (IsRunningDedicatedServer()) {
		// return;
	// }

	GET_MODULE_SUBSYSTEM(AuthSubsystem, Api2, Auth);
	if (AuthSubsystem) {
		// FString Key = AuthSubsystem->GetUserApiKey();

		auto OnChangeAuthorizationStateCallback = [=](EAuthorizationState AuthorizationState) {
			if (AuthorizationState == EAuthorizationState::LoginSuccess) {
				ApiKey = AuthSubsystem->GetUserId().ToString(EGuidFormats::DigitsWithHyphensLower);
				Connect();
				return;
			}

			if (AuthorizationState == EAuthorizationState::LogoutSuccess) {
				Disconnect();
				ApiKey.Empty();
				return;
			}
		};

		AuthSubsystem_OnChangeAuthorizationStateHandle = AuthSubsystem->GetOnChangeAuthorizationState().AddLambda(OnChangeAuthorizationStateCallback);
	}
}

void FRpcWebSocketsServiceSubsystem::Shutdown() {
#if WITH_RPC
	Disconnect();

	GET_MODULE_SUBSYSTEM(AuthSubsystem, Api2, Auth);
	if (AuthSubsystem) {
		if (AuthSubsystem_OnChangeAuthorizationStateHandle.IsValid()) {
			AuthSubsystem->GetOnChangeAuthorizationState().Remove(AuthSubsystem_OnChangeAuthorizationStateHandle);
		}
	}

	// Remove all listeners.
	OnConnected.Clear();
#endif
}

void FRpcWebSocketsServiceSubsystem::WebSocketsLogin() {
#if WITH_RPC
	auto BodyJsonObject = MakeShared<FJsonObject>();
	BodyJsonObject->SetStringField(RpcWebSocketsServiceJsonFields::Key, GetApiKey());

	FRpcWebSocketsMessage RpcMessage(
		ERpcWebSocketsMessageType::Request,
		ERpcWebSocketsMessageTopic::System,
		RpcWebSocketsServiceMethods::ConnectMethod,
		BodyJsonObject
		);

	auto Callback = MakeShared<FRpcWebSocketsMessageCallback>();
	Callback->BindRaw(this, &FRpcWebSocketsServiceSubsystem::OnWebSocketsLoginComplete);

	if (SendMessageInternal(RpcMessage, Callback)) {
		return;
	}

	VeLogError("Failed to login.");

#endif
}

void FRpcWebSocketsServiceSubsystem::WebSocketsNotifyNameChanged() {
#if WITH_RPC
	auto BodyJsonObject = MakeShared<FJsonObject>();
	BodyJsonObject->SetStringField(RpcWebSocketsServiceJsonFields::Key, GetApiKey());

	FRpcWebSocketsMessage RpcMessage(
		ERpcWebSocketsMessageType::Request,
		ERpcWebSocketsMessageTopic::System,
		RpcWebSocketsServiceMethods::UserChangeNameMethod,
		BodyJsonObject
		);

	auto Callback = MakeShared<FRpcWebSocketsMessageCallback>();
	Callback->BindRaw(this, &FRpcWebSocketsServiceSubsystem::OnWebSocketsLoginComplete);

	if (SendMessageInternal(RpcMessage, Callback)) {
		return;
	}

	VeLogError("Failed to change name.");
#endif
}

void FRpcWebSocketsServiceSubsystem::OnWebSocketsLoginComplete(const FString& Method, const TSharedPtr<FJsonObject>& BodyJsonObject) {
#if WITH_RPC
	if (!BodyJsonObject) {
		VeLogError("Error authentication. Method: {%s}, Body: {null}.", *Method);
		return;
	}

	FString Status, StatusMessage;
	if (!CheckStatus(BodyJsonObject, Status, StatusMessage)) {
		VeLogError("Error authentication. Status: {%s}, Message: {%s}.", *Status, *StatusMessage);
		return;
	}

	FRpcWebSocketsUser User;
	User.ParseJson(BodyJsonObject->GetObjectField(RpcWebSocketsServiceJsonFields::Sender));
	if (!User.IsValid()) {
		VeLogError("Error authentication.");
		return;
	}

	VeLog("WebSocket login success for {%s}", *User.Name);
	bAuthenticated = true;
	OnLoggedIn.Broadcast();
#endif
}

bool FRpcWebSocketsServiceSubsystem::IsConnected() const {
	return WebSocketPtr && WebSocketPtr->IsConnected();
}

void FRpcWebSocketsServiceSubsystem::Connect(const bool InIsServer) {
#if WITH_RPC
	if (WebSocketPtr) {
		if (WebSocketPtr->IsConnected()) {
			VeLogWarning("WebSocket already connected.");
			return;
		}
		if (bConnectionInProcess) {
			VeLogWarning("WebSocket connection in process.");
			return;
		}
		WebSocketPtr->Close();
	}

	// Destroy previous WebSocket and create new WebSocket
	WebSocketPtr = GetWebSocketsModule()->CreateWebSocket(WebSocketsEndpoint, WebSocketsProtocol);

	// Event bindings.
	WebSocketPtr->OnMessage().AddRaw(this, &FRpcWebSocketsServiceSubsystem::WebSocket_OnMessageReceive);
	WebSocketPtr->OnMessageSent().AddRaw(this, &FRpcWebSocketsServiceSubsystem::WebSocket_OnMessageSent);
	WebSocketPtr->OnConnected().AddRaw(this, &FRpcWebSocketsServiceSubsystem::WebSocket_OnConnected);
	WebSocketPtr->OnConnectionError().AddRaw(this, &FRpcWebSocketsServiceSubsystem::WebSocket_OnConnectionError);
	WebSocketPtr->OnClosed().AddRaw(this, &FRpcWebSocketsServiceSubsystem::WebSocket_OnClosed);

	// Initiate WebSocket connection.
	bAuthenticated = false;
	bConnectionInProcess = true;
	WebSocketPtr->Connect();

	bIsServer = InIsServer;
#endif
}

void FRpcWebSocketsServiceSubsystem::Disconnect() {
#if WITH_RPC
	if (WebSocketPtr) {
		WebSocketPtr->Close();
		WebSocketPtr.Reset();
	}
	RequestCallbacks.Reset();
#endif
}

FWebSocketsModule* FRpcWebSocketsServiceSubsystem::GetWebSocketsModule() {
#if WITH_RPC
	if (!FModuleManager::Get().IsModuleLoaded("WebSockets")) {
		const auto WebSocketsModuleInterface = FModuleManager::Get().LoadModule("WebSockets");
		if (WebSocketsModuleInterface == nullptr) {
			VeLogError("Can not load WebSockets module.")
			return nullptr;
		}
		return StaticCast<FWebSocketsModule*>(WebSocketsModuleInterface);
	}

	return StaticCast<FWebSocketsModule*>(FModuleManager::Get().GetModule("WebSockets"));
#else
    return nullptr;
#endif
}

void FRpcWebSocketsServiceSubsystem::WebSocket_OnMessageReceive(const FString& MessagesStr) {
#if WITH_RPC
	if (MessagesStr.IsEmpty()) {
		VeLogVerbose("Empty message received, skipping.");
		return;
	}

	TArray<FString> Messages;
	MessagesStr.ParseIntoArrayLines(Messages);

	for (const auto& Message : Messages) {
		if (Message.IsEmpty()) {
			VeLogVerbose("Empty message line, skipping.");
			continue;
		}

		VeLogVerbose("Received message: {%s}", *Message);

		ProcessReceivedWebSocketsMessage(Message);
	}
#endif
}

void FRpcWebSocketsServiceSubsystem::WebSocket_OnMessageSent(const FString& Message) {
#if WITH_RPC
	VeLogVerbose("Sent message: {%s}", *Message);
#endif
}

void FRpcWebSocketsServiceSubsystem::WebSocket_OnConnected() {
#if WITH_RPC
	VeLog("WebSocket connection opened.");

	bConnectionInProcess = false;

	// Notify everyone that WebSockets service is connected.
	OnConnected.Broadcast();

	WebSocketsLogin();
#endif
}

void FRpcWebSocketsServiceSubsystem::WebSocket_OnConnectionError(const FString& Error) {
#if WITH_RPC
	ConnectRetries--;

	if (ConnectRetries > 0) {
		VeLogError("WebSocket connection error. Message: {%s}", *Error);

		if (!WebSocketPtr) {
			VeLogError("WebSocketPtr not valid.");
			return;
		}

		OnDisconnected.Broadcast();

		if (bAuthenticated) {
			OnLoggedOut.Broadcast();
			bAuthenticated = false;
		}

		bConnectionInProcess = true;

		WebSocketPtr->Connect();
		return;
	}
#endif
}

void FRpcWebSocketsServiceSubsystem::WebSocket_OnClosed(const int32 StatusCode, const FString& Reason, const bool bWasClean) {
#if WITH_RPC
	VeLogWarning("WebSocket connection closed. Status: {%d}, Reason: {%s}, WasClean: {%d}", StatusCode, *Reason, bWasClean);

	OnDisconnected.Broadcast();

	if (bAuthenticated) {
		OnLoggedOut.Broadcast();
		bAuthenticated = false;
	}
#endif
}

bool FRpcWebSocketsServiceSubsystem::SendMessage(const FRpcWebSocketsMessage& Message, TSharedPtr<FRpcWebSocketsMessageCallback> InCallback) {
#if WITH_RPC
	if (!IsAuthenticated()) {
		VeLogError("Not ready for use (not authenticated). Please run startup and authenticate first.");
		return false;
	}

	return SendMessageInternal(Message, InCallback);
#else
	return true;
#endif
}

bool FRpcWebSocketsServiceSubsystem::CheckStatus(const TSharedPtr<FJsonObject>& BodyJsonObject, FString& OutStatus, FString& OutMessage) {
	if (!BodyJsonObject->HasTypedField<EJson::String>(RpcWebSocketsServiceJsonFields::Status)) {
		return false;
	}

	OutStatus = BodyJsonObject->GetStringField(RpcWebSocketsServiceJsonFields::Status);

	if (BodyJsonObject->HasTypedField<EJson::String>(RpcWebSocketsServiceJsonFields::Message)) {
		OutMessage = BodyJsonObject->GetStringField(RpcWebSocketsServiceJsonFields::Message);
	}

	return OutStatus == RpcWebSocketsServiceStatus::Ok;
}

bool FRpcWebSocketsServiceSubsystem::SendMessageInternal(const FRpcWebSocketsMessage& Message, TSharedPtr<FRpcWebSocketsMessageCallback> InCallback) {
#if WITH_RPC
	if (!IsConnected()) {
		VeLogError("Not ready for use (not connected). Please run startup and authenticate first.");
		return false;
	}

	// Convert the request into JSON object.
	const auto JsonRequestObj = Message.ToJson();

	// Serialize the JSON object into a string. 
	FString JsonMessageStr;
	const TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&JsonMessageStr);
	FJsonSerializer::Serialize(JsonRequestObj, JsonWriter);

	if (static_cast<ERpcWebSocketsMessageType>(Message.Type) == ERpcWebSocketsMessageType::Request) {
		RequestCallbacks.Add(Message.Id, InCallback);
	}

	WebSocketPtr->Send(JsonMessageStr);

#endif
	return true;
}

void FRpcWebSocketsServiceSubsystem::ProcessReceivedWebSocketsMessage(const FString& MessageStr) {
#if WITH_RPC
	const TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(MessageStr);

	TSharedPtr<FJsonObject> JsonPayload;
	if (!FJsonSerializer::Deserialize(JsonReader, JsonPayload) || !JsonPayload) {
		VeLogError("Failed to deserialize the message: {%s}", *MessageStr);
		return;
	}

	FRpcWebSocketsMessage Message;
	Message.ParseJson(JsonPayload);

	// Push
	if (static_cast<ERpcWebSocketsMessageType>(Message.Type) == ERpcWebSocketsMessageType::Push) {
		OnPushed.Broadcast(Message.Topic, Message.Method, Message.Body);
		return;
	}

	// Response
	if (RequestCallbacks.Contains(Message.Id)) {
		const auto RequestCallback = RequestCallbacks[Message.Id];
		if (RequestCallback.IsValid()) {
			RequestCallback->ExecuteIfBound(Message.Method, Message.Body);
		}
		RequestCallbacks.Remove(Message.Id);
	} else {
		VeLogWarning("Response is invalid and not removed from the request map, id: {%s}", *Message.Id.ToString(EGuidFormats::DigitsWithHyphens));
	}

#endif
}
