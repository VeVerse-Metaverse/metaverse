// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "ArtheonWebSocketsService.h"

#include "IWebSocket.h"
#include "WebSocketsModule.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "VeShared.h"
#include "ArtheonWebSocketsTypes.h"
#include "VeConfigLibrary.h"

UArtheonWebSocketsService::UArtheonWebSocketsService(): Super() {
	LogF("Starting WebSockets service. RPC Enabled: %d, RPC ChatEnabled: false", WITH_RPC);
}

void UArtheonWebSocketsService::Initialize(FSubsystemCollectionBase& Collection) {
#if WITH_RPC
	LoginCompleteCallback.BindUObject(this, &UArtheonWebSocketsService::OnWebSocketsLoginComplete);
#endif
}

void UArtheonWebSocketsService::Deinitialize() {
#if WITH_RPC
	Disconnect();

	LoginCompleteCallback.Unbind();

	// Remove all listeners.
	OnWebSocketsInitialized.Clear();
#endif
}

void UArtheonWebSocketsService::WebSocketsLogin(const FString& InKey) {
#if WITH_RPC
	// #if WITH_AUTOMATION_TESTS
	// if (GIsAutomationTesting) { return; }
	// #endif

	TSharedPtr<FArtheonWebSocketsRequest> Request = MakeShareable(new FArtheonWebSocketsRequest());
	Request->Message.Type = ArtheonWebSocketsType::RequestMessageType;
	Request->Message.Topic = ArtheonWebSocketsTopic::ConnectTopic;
	Request->Message.Method = ArtheonWebSocketsMethods::ConnectMethod;

	TSharedPtr<FArtheonWebSocketsPayload> Args = MakeShareable(new FArtheonWebSocketsPayload());
	Args->Key = InKey;
	WebSocketsUserKey = InKey;

	Request->Message.Args = Args;

	SendWebSocketsRequestInternal(Request, FOnCppWebSocketsRequestComplete());
#endif
}

void UArtheonWebSocketsService::OnWebSocketsLoginComplete(const FArtheonWebSocketsMessage Message, const bool bWasSuccessful) {
#if WITH_RPC
	if (bWasSuccessful) {
		if (Message.Payload.IsValid() && Message.Payload->User.IsValid()) {
			// WebSocketsUser = Message.Payload->User;
			bAuthenticated = true;

			OnWebSocketsLoggedIn.Broadcast();
			LogF("Websockets login success for {%s}", *Message.Payload->User.Name);

			//#if WITH_ANALYTICS
			//            SendUserActionMessage(Message.Payload->User.Id, TEXT("WebSockets.LoginComplete"), TEXT(""));
			//#endif
			return;
		}

		LogErrorF("Payload has no user data.");
		return;
	}

	if (Message.Payload.IsValid()) {
		LogErrorF("Error, status: {%s}, message: {%s}.", *Message.Payload->Status, *Message.Payload->Message);
		return;
	}

	LogErrorF("Unknown error.");
#endif
}

bool UArtheonWebSocketsService::IsConnected() const {
	return WebSocketPtr && WebSocketPtr->IsConnected();
}

void UArtheonWebSocketsService::Connect(const FString& InKey, const bool InIsServer) {
#if WITH_RPC
	if (InKey.IsEmpty()) {
		LogErrorF("ws key is empty");
		return;
	}

	if (WebSocketPtr) {
		if (WebSocketPtr->IsConnected()) {
			LogF("WebSocket already connected.");
			return;
		}
		if (bConnectionInProcess) {
			LogF("WebSocket connection in process.");
			return;
		}
		WebSocketPtr->Close();
	}

	// Destroy previous WebSocket and create new WebSocket
	WebSocketPtr = GetWebSocketsModule()->CreateWebSocket(FVeConfigLibrary::GetWebSocketsUrl(), FVeConfigLibrary::GetWebSocketsProtocol());

	// Event bindings.

	// if (!GIsAutomationTesting) {
	WebSocketPtr->OnMessage().AddUObject(this, &UArtheonWebSocketsService::OnWebSocketMessageReceive);
	WebSocketPtr->OnMessageSent().AddStatic(&UArtheonWebSocketsService::OnWebSocketMessageSent);
	WebSocketPtr->OnConnected().AddUObject(this, &UArtheonWebSocketsService::OnWebSocketConnect);
	WebSocketPtr->OnConnectionError().AddUObject(this, &UArtheonWebSocketsService::OnWebSocketConnectionError);
	WebSocketPtr->OnClosed().AddStatic(&UArtheonWebSocketsService::OnWebSocketClose);
	// }


	// Initiate WebSocket connection.
	bConnectionInProcess = true;
	WebSocketPtr->Connect();

	Key = InKey;
	bInitialized = true;
	bAuthenticated = false;
	WebSocketsReady.Broadcast();
	bIsServer = InIsServer;
#endif
}

void UArtheonWebSocketsService::Disconnect() {
#if WITH_RPC
	if (WebSocketPtr.IsValid()) {
		OnWebSocketsLoggedOut.Broadcast();
		WebSocketPtr->Close();
		WebSocketPtr.Reset();
	}
	bAuthenticated = false;
	SavedMessages.Empty();
#endif
}

FWebSocketsModule* UArtheonWebSocketsService::GetWebSocketsModule() {
#if WITH_RPC
	if (!FModuleManager::Get().IsModuleLoaded("WebSockets")) {
		const auto WebSocketsModuleInterface = FModuleManager::Get().LoadModule("WebSockets");
		if (WebSocketsModuleInterface == nullptr) {
			UE_LOG(LogTemp, Error, TEXT("UArtheonWebSocketsService::GetWebSocketsModule() -> Can not load WebSockets module."));
			return nullptr;
		}
		return StaticCast<FWebSocketsModule*>(WebSocketsModuleInterface);
	}

	return StaticCast<FWebSocketsModule*>(FModuleManager::Get().GetModule("WebSockets"));
#else
    return nullptr;
#endif
}

void UArtheonWebSocketsService::OnWebSocketMessageReceive(const FString& InMessage) {
#if WITH_RPC
	if (InMessage.IsEmpty()) {
		LogF("Empty message received, skipping.");
		return;
	}

	LogVerboseF("New WebSockets message received: %s", *InMessage);
	UE_LOG(LogTemp, Warning, TEXT("RESPONSE: %s"), *InMessage);

	TArray<FString> Messages;
	InMessage.ParseIntoArrayLines(Messages);

	for (auto& Message : Messages) {
		if (Message.IsEmpty()) {
			UE_LOG(LogTemp, Warning, TEXT("UArtheonWebSocketsService::OnWebSocketMessageReceived() -> Empty message line, skipping."));
			continue;
		}

		ProcessReceivedWebSocketsMessage(Message);
	}
#endif
}

void UArtheonWebSocketsService::OnWebSocketMessageSent(const FString& Message) {
#if WITH_RPC
	UE_LOG(LogTemp, Log, TEXT("UArtheonWebSocketsService::OnWebSocketMessageSent() -> Message: {%s}"), *Message);
	UE_LOG(LogTemp, Warning, TEXT("REQUEST: %s"), *Message);
#endif
}

void UArtheonWebSocketsService::OnWebSocketConnect() {
#if WITH_RPC
	bConnectionInProcess = false;
	
	if (OnWebSocketsConnected.IsBound()) {
		OnWebSocketsConnected.Broadcast();
	}
	
	// Notify everyone that WebSockets service is initialized.
	OnWebSocketsInitialized.Broadcast();

	WebSocketsLogin(Key);
#endif
}

void UArtheonWebSocketsService::OnWebSocketConnectionError(const FString& Error) {
#if WITH_RPC
	UE_LOG(LogTemp, Error, TEXT("UArtheonWebSocketsService::OnWebSocketConnectionError() -> Error: {%s}"), *Error);
	if (!WebSocketPtr.IsValid()) {
		UE_LOG(LogTemp, Warning, TEXT("UArtheonWebSocketsService::OnWebSocketConnectionError() -> WebSocketPtr not valid"));
		return;
	}

	bConnectionInProcess = true;
	WebSocketPtr->Connect();
#endif
}

void UArtheonWebSocketsService::OnWebSocketClose(const int32 StatusCode, const FString& Reason, const bool bWasClean) {
#if WITH_RPC
	UE_LOG(LogTemp, Warning, TEXT("UArtheonWebSocketsService::OnWebSocketClose() -> Status: {%d}, Reason: {%s}, bWasClean: {%d}"), StatusCode, *Reason, bWasClean);
#endif
}

void UArtheonWebSocketsService::ProcessReceivedWebSocketsMessage(const FString& InJsonMessageStr) {
#if WITH_RPC
	const TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(InJsonMessageStr);

	TSharedPtr<FJsonObject> JsonPayload;
	if (!FJsonSerializer::Deserialize(JsonReader, JsonPayload) || !JsonPayload.IsValid()) {
		UE_LOG(LogTemp, Error, TEXT("UArtheonWebSocketsService::ProcessReceivedWebSocketsMessage() -> Failed to deserialize the message: {%s}"), *InJsonMessageStr);
		return;
	}

	FArtheonWebSocketsMessage WebSocketsMessage;
	WebSocketsMessage.ParseJson(JsonPayload);

	switch (WebSocketsMessage.Type) {
	case ArtheonWebSocketsType::PushMessageType:
		{
			switch (WebSocketsMessage.Topic) {
			case ArtheonWebSocketsTopic::ConnectTopic:
				{
					UE_LOG(LogTemp, Log, TEXT("UArtheonWebSocketsService::ProcessReceivedWebSocketsMessage() -> Received connect message, message: {%s}"),
						   *WebSocketsMessage.Payload->Message);
				}
				break;
			case ArtheonWebSocketsTopic::ChatTopic:
				{
				}
				break;

			default:
				UE_LOG(LogTemp, Error, TEXT("UArtheonWebSocketsService::ProcessReceivedWebSocketsMessage() -> Received unrelated push message, ignoring"));
				break;
			}
			break;
		}
	case ArtheonWebSocketsType::RequestMessageType:
		{
			UE_LOG(LogTemp, Warning, TEXT("UArtheonWebSocketsService::ProcessReceivedWebSocketsMessage() -> Received request message, ignoring, message: {%s}"),
				   *InJsonMessageStr);
			break;
		}
	case ArtheonWebSocketsType::ResponseMessageType:
		{
			if (WebSocketsMessage.Payload->Status != ArtheonWebSocketsStatus::Ok) {
				UE_LOG(LogTemp, Error,
					   TEXT("UArtheonWebSocketsService::ProcessReceivedWebSocketsMessage() -> Received response error message, status: {%s}, message: {%s}"),
					   *WebSocketsMessage.Payload->Status, *WebSocketsMessage.Payload->Message);
				CompleteRequest(WebSocketsMessage.Id, false, WebSocketsMessage);
				return;
			}

			UE_LOG(LogTemp, Log,
				   TEXT("UArtheonWebSocketsService::ProcessReceivedWebSocketsMessage() -> Received response from the server, requestId: {%s} status: {%s}, method: {%s}"),
				   *WebSocketsMessage.Id.ToString(), *WebSocketsMessage.Payload->Status, *WebSocketsMessage.Method);

			const auto ChannelPayload = StaticCastSharedPtr<FArtheonWebSocketsPayload>(WebSocketsMessage.Payload);

			if (!ChannelPayload.IsValid()) {
				UE_LOG(LogTemp, Error, TEXT("UArtheonWebSocketsService::ProcessReceivedWebSocketsMessage() -> Invalid payload, status: {%s}, message: {%s}"),
					   *WebSocketsMessage.Payload->Status, *WebSocketsMessage.Payload->Message);
				CompleteRequest(WebSocketsMessage.Id, false, WebSocketsMessage);
				return;
			}

			if (WebSocketsMessage.Method == ArtheonWebSocketsMethods::ConnectMethod) {
				// ReSharper disable once CppExpressionWithoutSideEffects
				LoginCompleteCallback.ExecuteIfBound(WebSocketsMessage, true);
			}

#if WITH_VIVOX
			else if (WebSocketsMessage.Method == ArtheonWebSocketsMethods::VivoxGetLoginToken) {
				CompleteRequest(WebSocketsMessage.Id, true, WebSocketsMessage);
				return;
			} else if (WebSocketsMessage.Method == ArtheonWebSocketsMethods::VivoxGetJoinToken) {
				CompleteRequest(WebSocketsMessage.Id, true, WebSocketsMessage);
				return;
			} else if (WebSocketsMessage.Method == ArtheonWebSocketsMethods::VivoxMute) {
				CompleteRequest(WebSocketsMessage.Id, true, WebSocketsMessage);
				return;
			} else if (WebSocketsMessage.Method == ArtheonWebSocketsMethods::VivoxKick) {
				CompleteRequest(WebSocketsMessage.Id, true, WebSocketsMessage);
				return;
			}
#endif
			else {
				UE_LOG(LogTemp, Warning, TEXT("UArtheonWebSocketsService::ProcessReceivedWebSocketsMessage() -> Unknown method, method: {%s}"),
					   *WebSocketsMessage.Method);
				CompleteRequest(WebSocketsMessage.Id, false, WebSocketsMessage);
				return;
			}

			CompleteRequest(WebSocketsMessage.Id, true, WebSocketsMessage);
		}

		break;
	default:
		break;
	}
#endif
}

bool UArtheonWebSocketsService::SendWebSocketsMessage(FArtheonWebSocketsMessage Message) const {
#if WITH_RPC
	// CHECK_RETURN_VALUE(bAuthenticated, false);
	if (!bAuthenticated) {
		return false;
	}
	return SendWebSocketsMessageInternal(Message);
#else
	return false;
#endif
}

bool UArtheonWebSocketsService::SendWebSocketsRequest(const TSharedPtr<FArtheonWebSocketsRequest> Request, const FOnCppWebSocketsRequestComplete& CallbackDelegate) {
#if WITH_RPC
	// CHECK_RETURN_VALUE(bAuthenticated, false);
	if (!bAuthenticated) {
		return false;
	}
	return SendWebSocketsRequestInternal(Request, CallbackDelegate);
#else
	return false;
#endif
}

bool UArtheonWebSocketsService::SendWebSocketsMessageInternal(FArtheonWebSocketsMessage Message) const {
#if WITH_RPC
	if (!bInitialized) {
		return false;
	}
	// CHECK_RETURN_VALUE(bInitialized, false);
	// CHECK_RETURN_VALUE(bAuthenticated, false);

	// Convert the request into JSON object.
	const auto JsonRequestObj = Message.ToJson();

	// Serialize the JSON object into a string. 
	FString JsonMessageStr;
	const TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&JsonMessageStr);
	FJsonSerializer::Serialize(JsonRequestObj.ToSharedRef(), JsonWriter);

	if (!WebSocketPtr.IsValid()) {
		UE_LOG(LogTemp, Error, TEXT("UArtheonWebSocketsService::SendWebSocketsMessage() -> Websockets are not ready for use. Please run startup and authenticate first."));
		return false;
	}

	WebSocketPtr->Send(JsonMessageStr);
#endif
	return true;
}

bool UArtheonWebSocketsService::SendWebSocketsRequestInternal(TSharedPtr<FArtheonWebSocketsRequest> Request, const FOnCppWebSocketsRequestComplete& CallbackDelegate) {
#if WITH_RPC
	if (!bInitialized) {
		return false;
	}
	// CHECK_RETURN_VALUE(bInitialized, false);

	// Sign with the key.
	Request->Message.Args->Key = WebSocketsUserKey;

	// Bind the delegate.
	Request->OnCppRequestComplete = CallbackDelegate;

	WebSocketsRequests.Add(Request->Message.Id, Request);
#endif
	return SendWebSocketsMessageInternal(Request->Message);
}

void UArtheonWebSocketsService::CompleteRequest(const FGuid& Id, const bool bSuccessful, const FArtheonWebSocketsMessage& ResponseMessage) {
#if WITH_RPC
	if (WebSocketsRequests.Contains(Id)) {
		const auto Request = WebSocketsRequests[Id];
		if (Request.IsValid()) {
			Request->ResponseMessage = ResponseMessage;
			// ReSharper disable once CppExpressionWithoutSideEffects
			Request->TriggerDelegates(bSuccessful);
		} else {
			UE_LOG(LogTemp, Warning, TEXT("UArtheonWebSocketsService::CompleteRequest() -> Request is invalid and not removed from the request map, id: {%s}"),
				   *Id.ToString());
		}
		WebSocketsRequests.Remove(Id);
		// UE_LOG(LogTemp, Log, TEXT("UArtheonWebSocketsService::CompleteRequest() -> Completed and removed tracked request, id: {%s}"), *Id.ToString());
	} else {
		UE_LOG(LogTemp, Warning, TEXT("UArtheonWebSocketsService::CompleteRequest() -> Request already complete and removed from the request map, id: {%s}"),
			   *Id.ToString());
	}
#endif
}
