// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#include "Voice/VivoxService.h"

#include "VeApi.h"
#include "ArtheonWebSocketsService.h"
#include "ArtheonWebSocketsTypes.h"
#include "VePlayerController.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#pragma region Client
#if USE_VERPC_VIVOX
#include "RpcWebSocketsServiceSubsystem.h"
#include "VeRpc.h"
#endif
#if WITH_VIVOX
#include "VivoxCore.h"
#include "Misc/DefaultValueHelper.h"
#if PLATFORM_ANDROID
#include "AndroidPermissionFunctionLibrary.h"
#include "AndroidPermissionCallbackProxy.h"
#endif
#endif
#pragma endregion

// ReSharper disable once CppMemberFunctionMayBeStatic
void UVivoxService::OnWebSocketsLoggedIn() {
#if WITH_RPC
#if WITH_VIVOX
	LogVerboseF("Vivox: WebSockets authenticated.");

	// Wait for the websockets to become ready then do login procedure.
	if (!PendingLoginUserId.IsEmpty() && !(IsLoggedIn() || bLoggingIn)) {
		LogVerboseF("Vivox: Authenticating as {%s}. Status: LoggedIn: {%d}, LoggingIn: {%d}.", *PendingLoginUserId, IsLoggedIn(), bLoggingIn);
		VivoxLogin(PendingLoginUserId);
		return;
	}

#if USE_VERPC_VIVOX

	GET_MODULE_SUBSYSTEM(AuthSubsystem, Api, Auth);
	if (AuthSubsystem) {
		if (AuthSubsystem->GetUserId().IsValid()) {
			LogVerboseF("Vivox: Authenticating as {%s}, LoggedIn: {%d}, LoggingIn: {%d}.",
						 *AuthSubsystem->GetUserId().ToString(EGuidFormats::DigitsWithHyphens), IsLoggedIn(), bLoggingIn);
			VivoxLogin(AuthSubsystem->GetUserId().ToString(EGuidFormats::Digits));
		}
	}

#else
	
	if (FVeApiModule* ApiModule = FVeApiModule::Get()) {
		if (const TSharedPtr<FApiAuthSubsystem> AuthSubsystem = ApiModule->GetAuthSubsystem()) {
			if (AuthSubsystem->GetUserId().IsValid()) {
				LogVerboseF("Vivox: Authenticating as {%s}, LoggedIn: {%d}, LoggingIn: {%d}.", *AuthSubsystem->GetUserId().ToString(EGuidFormats::DigitsWithHyphens), IsLoggedIn(),
							bLoggingIn);
				VivoxLogin(AuthSubsystem->GetUserId().ToString(EGuidFormats::Digits));
			}
		} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Api), STRINGIFY(Auth)); }
	} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Api)); }
	
#endif
#endif
#endif
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void UVivoxService::OnWebSocketsLoggedOut() {
#if WITH_RPC
#if WITH_VIVOX
	LogVerboseF("Vivox: WebSockets logout: %s, LoggedIn: {%d}, LoggingIn: {%d}.", *PendingLoginUserId, IsLoggedIn(), bLoggingIn);
	// Wait for the websockets to become ready then do login procedure.
	if (IsLoggedIn()) {
		VivoxLogout();
	}

	ResetLoginState();
#endif
#endif
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void UVivoxService::OnVivoxLoggedIn() {
#if WITH_VIVOX
	LogVerboseF("Vivox: Logged in as %s, LoggedIn: {%d}, PendingServerId: {%s}.", *LoggedInAccountId.Name(), IsLoggedIn(), *PendingServerId.ToString());
	if (!IsLoggedIn()) {
		return;
	}

	// if (PendingOnlineGame.Id.IsValid()) {
	// JoinOnlineGame(PendingOnlineGame);
	if (PendingServerId.IsValid()) {
		JoinServer(PendingServerId);
	} else {
		// const FApiServerMetadata DefaultServer;
		// JoinOnlineGame(DefaultServer);
		JoinServer({});
		LogVerboseF("vivox: no pending online game to join, joining default channel");
	}
#endif
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void UVivoxService::OnVivoxLoggedOut() {
#if WITH_VIVOX
	LogVerboseF("Vivox: Logged out: %s, LoggedIn: {%d}, PendingGameId: {%s}.", *PendingLoginUserId, IsLoggedIn(), *PendingOnlineGame.Id.ToString());

	if (IsLoggedIn()) {
		LeaveAllVoiceChannels();
	}
#endif
}

UVivoxService::UVivoxService()
	: UGameInstanceSubsystem() {
#if WITH_VIVOX
	LogVerboseF("Vivox: Constructor. Loading VivoxCore module.");

	const auto VivoxCoreModule = static_cast<FVivoxCoreModule*>(&FModuleManager::Get().LoadModuleChecked(TEXT("VivoxCore")));
	if (VivoxCoreModule == nullptr) {
		LogErrorF("Vivox: No vivox core module!");
	}

	VivoxVoiceClient = &VivoxCoreModule->VoiceClient();
	if (!VivoxVoiceClient) {
		LogErrorF("Vivox: failed to get voice client");
	}

	LogVerboseF("Vivox: Loaded VivoxCore module.");
#endif
}

#pragma region Admin Features
#if WITH_RPC && UE_SERVER
void UVivoxService::MuteAtPositionalChannel(const FString& InChannelId, const FString& InAdminId, const FString& InUserId) {
#if USE_VERPC_VIVOX
	GET_MODULE_SUBSYSTEM(WebSocketsRpcServiceSubsystem, Rpc, WebSocketsService);
	if (!WebSocketsRpcServiceSubsystem) {
		return;
	}
	if (!WebSocketsRpcServiceSubsystem->IsAuthenticated()) {
		return;
	}
#else
    const auto GameInstance = GetGameInstance();
    if (!(GameInstance)) { return; }

    const auto WebSocketsService = GameInstance->GetSubsystem<UArtheonWebSocketsService>();
    if (!(WebSocketsService)) { return; }
    if (!(WebSocketsService->IsAuthenticated())) { return; }
#endif

	const auto From = InAdminId;

	RequestMuteAtPositionalChannel(From, InChannelId, InUserId);
}

void UVivoxService::RequestMuteAtPositionalChannel(const FString& From, const FString& To, const FString& Sub) {
#if USE_VERPC_VIVOX

	FArtheonWebSocketsVivoxPayload VivoxPayload;
	VivoxPayload.Action = ArtheonVivoxAction::Mute;
	VivoxPayload.From = From;
	VivoxPayload.To = To;
	VivoxPayload.Subject = Sub;
	VivoxPayload.ChannelProperties.ChannelType = EArtheonVivoxChannelType::Positional;
	VivoxPayload.ChannelProperties.Id = To;

	GET_MODULE_SUBSYSTEM(WebSocketsRpcServiceSubsystem, Rpc, WebSocketsService);
	if (!WebSocketsRpcServiceSubsystem) {
		return;
	}

	TSharedPtr<FJsonObject> BodyJsonObject = MakeShareable(new FJsonObject());

	// ApiKey
	const FString ApiKey = WebSocketsRpcServiceSubsystem->GetApiKey();
	BodyJsonObject->SetStringField(ArtheonWebSocketsJsonFields::Key, ApiKey);

	// VivoxPayload
	BodyJsonObject->SetObjectField(ArtheonWebSocketsJsonFields::VxPayload, VivoxPayload.ToJson());

	FRpcWebSocketsMessage RpcMessage(
		ERpcWebSocketsMessageType::Request,
		ERpcWebSocketsMessageTopic::Vivox,
		ArtheonWebSocketsMethods::VivoxMute,
		BodyJsonObject
		);

	TSharedPtr<FRpcWebSocketsMessageCallback> CallbackPtr = MakeShareable(new FRpcWebSocketsMessageCallback());
	CallbackPtr->BindWeakLambda(this, [this](const FString& Method, const TSharedPtr<FJsonObject>& BodyJsonObject) {
		FString Status, StatusMessage;
		if (FRpcWebSocketsServiceSubsystem::CheckStatus(BodyJsonObject, Status, StatusMessage)) {
			OnMuteAtPositionalChannelComplete(BodyJsonObject->GetStringField(ArtheonWebSocketsJsonFields::Message), true);
		} else {
			LogErrorF("Vivox: Failed to mute at positional channel. Status: {%s}, message: {%s}.", *Status, *StatusMessage);
			OnMuteAtPositionalChannelComplete(BodyJsonObject->GetStringField(ArtheonWebSocketsJsonFields::Message), false);
		}
	});

	if (WebSocketsRpcServiceSubsystem->SendMessage(RpcMessage, CallbackPtr)) {
		LogVerboseF("Vivox: Sent mute at positional channel request.");
		return;
	}

	LogErrorF("Vivox: Failed to sent mute at positional channel request.");

#else

    const auto GameInstance = GetGameInstance();
    if (!(GameInstance)) { return; }

    const auto WebSocketsService = GameInstance->GetSubsystem<UArtheonWebSocketsService>();
    if (!(WebSocketsService)) { return; }

    if (!(WebSocketsService->IsAuthenticated())) { return; }

    TSharedPtr<FArtheonWebSocketsRequest> Request = MakeShareable(new FArtheonWebSocketsRequest());
    Request->Message.Type = ArtheonWebSocketsType::RequestMessageType;
    Request->Message.Topic = ArtheonWebSocketsTopic::VivoxTopic;
    Request->Message.Method = ArtheonWebSocketsMethods::VivoxMute;

    FArtheonWebSocketsVivoxPayload VivoxPayload;
    VivoxPayload.Action = ArtheonVivoxAction::Mute;
    VivoxPayload.From = From;
    VivoxPayload.To = To;
    VivoxPayload.Subject = Sub;
    VivoxPayload.ChannelProperties.ChannelType = EArtheonVivoxChannelType::Positional;
    VivoxPayload.ChannelProperties.Id = To;

    TSharedPtr<FArtheonWebSocketsPayload> Args = MakeShareable(new FArtheonWebSocketsPayload());
    Args->VivoxPayload = VivoxPayload;

    Request->Message.Args = Args;

    OnMuteAtPositionalChannelCompleteDelegate.BindUObject(this, &UVivoxService::OnMuteAtPositionalChannelComplete);

    WebSocketsService->SendWebSocketsRequest(Request, OnMuteAtPositionalChannelCompleteDelegate);
#endif
}

#if USE_VERPC_VIVOX
void UVivoxService::OnMuteAtPositionalChannelComplete(const FString& Token, const bool bSuccessful) {
#else
void UVivoxService::OnMuteAtPositionalChannelComplete(const FArtheonWebSocketsMessage Response, const bool bSuccessful) {
#endif
	OnMuteAtPositionalChannelCompleteDelegate.Unbind();

#if USE_VERPC_VIVOX
	if (!bSuccessful) {
		LogErrorF("Vivox: Failed to mute at positional channel: %s", *Token);
		return;
	}
#else
    if (!bSuccessful) {
        LogErrorF("Vivox: Failed to mute at positional channel: %s", *Response.Payload->Message);
        return;
    }
#endif

	LogF("Vivox: Muted user.");
}

void UVivoxService::UnmuteAtPositionalChannel(const FString& InChannelId, const FString& InAdminId, const FString& InUserId) {
#if USE_VERPC_VIVOX
	GET_MODULE_SUBSYSTEM(WebSocketsRpcServiceSubsystem, Rpc, WebSocketsService);
	if (!WebSocketsRpcServiceSubsystem) {
		return;
	}
	if (!WebSocketsRpcServiceSubsystem->IsAuthenticated()) {
		return;
	}
#else
    const auto GameInstance = GetGameInstance();
    if (!(GameInstance)) { return; }

    const auto WebSocketsService = GameInstance->GetSubsystem<UArtheonWebSocketsService>();
    if (!(WebSocketsService)) { return; }
    if (!(WebSocketsService->IsAuthenticated())) { return; }
#endif

	const auto From = InAdminId;

	RequestUnmuteAtPositionalChannel(From, InChannelId, InUserId);
}

void UVivoxService::RequestUnmuteAtPositionalChannel(const FString& From, const FString& To, const FString& Sub) {
#if USE_VERPC_VIVOX
	FArtheonWebSocketsVivoxPayload VivoxPayload;
	VivoxPayload.Action = ArtheonVivoxAction::Unmute;
	VivoxPayload.From = From;
	VivoxPayload.To = To;
	VivoxPayload.Subject = Sub;
	VivoxPayload.ChannelProperties.ChannelType = EArtheonVivoxChannelType::Positional;
	VivoxPayload.ChannelProperties.Id = To;

	GET_MODULE_SUBSYSTEM(WebSocketsRpcServiceSubsystem, Rpc, WebSocketsService);
	if (!WebSocketsRpcServiceSubsystem) {
		return;
	}

	TSharedPtr<FJsonObject> BodyJsonObject = MakeShareable(new FJsonObject());

	// ApiKey
	const FString ApiKey = WebSocketsRpcServiceSubsystem->GetApiKey();
	BodyJsonObject->SetStringField(ArtheonWebSocketsJsonFields::Key, ApiKey);

	// VivoxPayload
	BodyJsonObject->SetObjectField(ArtheonWebSocketsJsonFields::VxPayload, VivoxPayload.ToJson());

	FRpcWebSocketsMessage RpcMessage(
		ERpcWebSocketsMessageType::Request,
		ERpcWebSocketsMessageTopic::Vivox,
		ArtheonWebSocketsMethods::VivoxUnmute,
		BodyJsonObject
		);

	TSharedPtr<FRpcWebSocketsMessageCallback> CallbackPtr = MakeShareable(new FRpcWebSocketsMessageCallback());
	CallbackPtr->BindWeakLambda(this, [this](const FString& Method, const TSharedPtr<FJsonObject>& BodyJsonObject) {
		FString Status, StatusMessage;
		if (FRpcWebSocketsServiceSubsystem::CheckStatus(BodyJsonObject, Status, StatusMessage)) {
			OnUnmuteAtPositionalChannelComplete(BodyJsonObject->GetStringField(ArtheonWebSocketsJsonFields::Message), true);
		} else {
			LogErrorF("Vivox: Failed to unmute at positional channel. Status: {%s}, message: {%s}.", *Status, *StatusMessage);
			OnUnmuteAtPositionalChannelComplete(BodyJsonObject->GetStringField(ArtheonWebSocketsJsonFields::Message), false);
		}
	});

	if (WebSocketsRpcServiceSubsystem->SendMessage(RpcMessage, CallbackPtr)) {
		LogVerboseF("Vivox: Sent unmute at positional channel request.");
		return;
	}

	LogErrorF("Vivox: Failed to sent unmute at positional channel request.");

#else
	
    const auto GameInstance = GetGameInstance();
    if (!GameInstance) { return; }

    const auto WebSocketsService = GameInstance->GetSubsystem<UArtheonWebSocketsService>();
    if (!WebSocketsService) { return; }

    if (!WebSocketsService->IsAuthenticated()) { return; }

    TSharedPtr<FArtheonWebSocketsRequest> Request = MakeShareable(new FArtheonWebSocketsRequest());
    Request->Message.Type = ArtheonWebSocketsType::RequestMessageType;
    Request->Message.Topic = ArtheonWebSocketsTopic::VivoxTopic;
    Request->Message.Method = ArtheonWebSocketsMethods::VivoxUnmute;

    FArtheonWebSocketsVivoxPayload VivoxPayload;
    VivoxPayload.Action = ArtheonVivoxAction::Unmute;
    VivoxPayload.From = From;
    VivoxPayload.To = To;
    VivoxPayload.Subject = Sub;
    VivoxPayload.ChannelProperties.ChannelType = EArtheonVivoxChannelType::Positional;
    VivoxPayload.ChannelProperties.Id = To;

    TSharedPtr<FArtheonWebSocketsPayload> Args = MakeShareable(new FArtheonWebSocketsPayload());
    Args->VivoxPayload = VivoxPayload;

    Request->Message.Args = Args;

    OnUnmuteAtPositionalChannelCompleteDelegate.BindUObject(this, &UVivoxService::OnUnmuteAtPositionalChannelComplete);

    WebSocketsService->SendWebSocketsRequest(Request, OnUnmuteAtPositionalChannelCompleteDelegate);
#endif
}

#if USE_VERPC_VIVOX
void UVivoxService::OnUnmuteAtPositionalChannelComplete(const FString& Token, const bool bSuccessful) {
#else
void UVivoxService::OnUnmuteAtPositionalChannelComplete(const FArtheonWebSocketsMessage Response, const bool bSuccessful) {
#endif
	OnUnmuteAtPositionalChannelCompleteDelegate.Unbind();

#if USE_VERPC_VIVOX
	if (!bSuccessful) {
		LogErrorF("Vivox: Failed to unmute at positional channel: %s", *Token);
		return;
	}
#else
    if (!bSuccessful) {
        LogErrorF("Vivox: Failed to unmute at positional channel: %s", *Response.Payload->Message);
        return;
    }
#endif

	LogF("Vivox: Unmuted user.");
}

void UVivoxService::KickFromPositionalChannel(const FString& InChannelId, const FString& InAdminId, const FString& InUserId) {
#if USE_VERPC_VIVOX
	GET_MODULE_SUBSYSTEM(WebSocketsRpcServiceSubsystem, Rpc, WebSocketsService);
	if (!WebSocketsRpcServiceSubsystem) {
		return;
	}
	if (!WebSocketsRpcServiceSubsystem->IsAuthenticated()) {
		return;
	}
#else
    const auto GameInstance = GetGameInstance();
    if (!GameInstance) { return; }

    const auto WebSocketsService = GameInstance->GetSubsystem<UArtheonWebSocketsService>();
    if (!WebSocketsService) { return; }
    if (!WebSocketsService->IsAuthenticated()) { return; }
#endif

	const auto From = InAdminId;

	RequestKickFromPositionalChannel(From, InChannelId, InUserId);
}

void UVivoxService::RequestKickFromPositionalChannel(const FString& From, const FString& To, const FString& Sub) {
#if USE_VERPC_VIVOX
	FArtheonWebSocketsVivoxPayload VivoxPayload;
	VivoxPayload.Action = ArtheonVivoxAction::Kick;
	VivoxPayload.From = From;
	VivoxPayload.To = To;
	VivoxPayload.Subject = Sub;
	VivoxPayload.ChannelProperties.ChannelType = EArtheonVivoxChannelType::Positional;
	VivoxPayload.ChannelProperties.Id = To;

	GET_MODULE_SUBSYSTEM(WebSocketsRpcServiceSubsystem, Rpc, WebSocketsService);
	if (!WebSocketsRpcServiceSubsystem) {
		return;
	}

	TSharedPtr<FJsonObject> BodyJsonObject = MakeShareable(new FJsonObject());

	// ApiKey
	const FString ApiKey = WebSocketsRpcServiceSubsystem->GetApiKey();
	BodyJsonObject->SetStringField(ArtheonWebSocketsJsonFields::Key, ApiKey);

	// VivoxPayload
	BodyJsonObject->SetObjectField(ArtheonWebSocketsJsonFields::VxPayload, VivoxPayload.ToJson());

	FRpcWebSocketsMessage RpcMessage(
		ERpcWebSocketsMessageType::Request,
		ERpcWebSocketsMessageTopic::Vivox,
		ArtheonWebSocketsMethods::VivoxKick,
		BodyJsonObject
		);

	TSharedPtr<FRpcWebSocketsMessageCallback> CallbackPtr = MakeShareable(new FRpcWebSocketsMessageCallback());
	CallbackPtr->BindWeakLambda(this, [this](const FString& Method, const TSharedPtr<FJsonObject>& BodyJsonObject) {
		FString Status, StatusMessage;
		if (FRpcWebSocketsServiceSubsystem::CheckStatus(BodyJsonObject, Status, StatusMessage)) {
			OnKickFromPositionalChannelComplete(BodyJsonObject->GetStringField(ArtheonWebSocketsJsonFields::Message), true);
		} else {
			LogErrorF("Vivox: Failed to kick. Status: {%s}, message: {%s}.", *Status, *StatusMessage);
			OnKickFromPositionalChannelComplete(BodyJsonObject->GetStringField(ArtheonWebSocketsJsonFields::Message), false);
		}
	});

	if (WebSocketsRpcServiceSubsystem->SendMessage(RpcMessage, CallbackPtr)) {
		LogVerboseF("Vivox: Sent kick request.");
		return;
	}

	LogErrorF("Vivox: Failed to sent kick request.");

#else
	
    const auto GameInstance = GetGameInstance();
    if (!GameInstance) { return; }

    const auto WebSocketsService = GameInstance->GetSubsystem<UArtheonWebSocketsService>();
    if (!WebSocketsService) { return; }

    if (!WebSocketsService->IsAuthenticated()) { return; }

    TSharedPtr<FArtheonWebSocketsRequest> Request = MakeShareable(new FArtheonWebSocketsRequest());
    Request->Message.Type = ArtheonWebSocketsType::RequestMessageType;
    Request->Message.Topic = ArtheonWebSocketsTopic::VivoxTopic;
    Request->Message.Method = ArtheonWebSocketsMethods::VivoxKick;

    FArtheonWebSocketsVivoxPayload VivoxPayload;
    VivoxPayload.Action = ArtheonVivoxAction::Kick;
    VivoxPayload.From = From;
    VivoxPayload.To = To;
    VivoxPayload.Subject = Sub;
    VivoxPayload.ChannelProperties.ChannelType = EArtheonVivoxChannelType::Positional;
    VivoxPayload.ChannelProperties.Id = To;

    TSharedPtr<FArtheonWebSocketsPayload> Args = MakeShareable(new FArtheonWebSocketsPayload());
    Args->VivoxPayload = VivoxPayload;

    Request->Message.Args = Args;

    OnKickCompleteDelegate.BindUObject(this, &UVivoxService::OnKickFromPositionalChannelComplete);

    WebSocketsService->SendWebSocketsRequest(Request, OnKickCompleteDelegate);
#endif
}

#if USE_VERPC_VIVOX
void UVivoxService::OnKickFromPositionalChannelComplete(const FString& Token, bool bSuccessful) {
#else
void UVivoxService::OnKickFromPositionalChannelComplete(FArtheonWebSocketsMessage Response, bool bSuccessful) {
#endif
	OnMuteAtPositionalChannelCompleteDelegate.Unbind();

#if USE_VERPC_VIVOX
	if (!bSuccessful) {
		LogErrorF("Vivox: Failed to kick: %s", *Token);
		return;
	}
#else
    if (!bSuccessful) {
        LogErrorF("Vivox: Failed to kick: %s", *Response.Payload->Message);
        return;
    }
#endif

	LogF("Vivox: Kicked user.");
}
#endif
#pragma endregion

#pragma region Initialization

void UVivoxService::Initialize(FSubsystemCollectionBase& Collection) {
	Super::Initialize(Collection);

	LogVerboseF("Vivox: Initialize subsystem.");

#ifdef WITH_AUTOMATION_TESTS
	if (GIsAutomationTesting) {
		return;
	}
#endif

#if WITH_RPC
#if USE_VERPC_VIVOX

	LogVerboseF("Vivox: Initializing service. Binding WebSockets delegates.");

	// Bind to websockets authenticated event to be able to request login token when the service is ready.
	GET_MODULE_SUBSYSTEM(WebSocketsRpcServiceSubsystem, Rpc, WebSocketsService);
	if (WebSocketsRpcServiceSubsystem) {
		RpcServiceOnLoggedInHandle = WebSocketsRpcServiceSubsystem->OnLoggedIn.AddUObject(this, &UVivoxService::OnWebSocketsLoggedIn);
		RpcServiceOnLoggedOutHandle = WebSocketsRpcServiceSubsystem->OnLoggedOut.AddUObject(this, &UVivoxService::OnWebSocketsLoggedOut);
		if (WebSocketsRpcServiceSubsystem->IsAuthenticated()) {
			OnWebSocketsLoggedIn();
		}
	}

#else
	
	Collection.InitializeDependency(UArtheonWebSocketsService::StaticClass());

	const auto WebSocketsService = GetGameInstance()->GetSubsystem<UArtheonWebSocketsService>();
	if (!WebSocketsService) {
		return;
	}

	LogVerboseF("Vivox: Initializing service. Binding WebSockets delegates.");

	// Bind to websockets authenticated event to be able to request login token when the service is ready.
	WebSocketsService->OnWebSocketsLoggedIn.AddDynamic(this, &UVivoxService::OnWebSocketsLoggedIn);
	WebSocketsService->OnWebSocketsLoggedOut.AddDynamic(this, &UVivoxService::OnWebSocketsLoggedOut);
#endif
#endif

#pragma region Win64 Client
#if WITH_VIVOX

	// Do not initialize vivox if using null RHI (e.g. dedicated server or headless client).
	if (GUsingNullRHI) {
		LogVerboseF("Vivox: Using Null RHI, Vivox won't be used.");
		return;
	}

	InitializeVivox();

	LogVerboseF("Vivox: Binding VivoxCore delegates.");

	OnVivoxLoggedInDelegate.AddDynamic(this, &UVivoxService::OnVivoxLoggedIn);
	OnVivoxLoggedOutDelegate.AddDynamic(this, &UVivoxService::OnVivoxLoggedOut);

	LogVerboseF("Vivox: Registering console commands.");

	if (!IConsoleManager::Get().IsNameRegistered(TEXT("vxcc"))) {
		IConsoleManager::Get().RegisterConsoleCommand(TEXT("vxcc"), TEXT("Vivox change active channel, required param: 'e' for echo or 'p' for positional"),
													  FConsoleCommandWithWorldAndArgsDelegate::CreateUObject(this, &UVivoxService::Exec_ChangeActiveChannel), ECVF_Default);
	}

	if (!IConsoleManager::Get().IsNameRegistered(TEXT("vxce"))) {
		IConsoleManager::Get().RegisterConsoleCommand(TEXT("vxce"), TEXT("Vivox connect to echo"),
													  FConsoleCommandWithWorldAndArgsDelegate::CreateUObject(this, &UVivoxService::Exec_ConnectToEchoChannel), ECVF_Default);
	}

	if (!IConsoleManager::Get().IsNameRegistered(TEXT("vxde"))) {
		IConsoleManager::Get().RegisterConsoleCommand(TEXT("vxde"), TEXT("Vivox disconnect from echo"),
													  FConsoleCommandWithWorldAndArgsDelegate::CreateUObject(this, &UVivoxService::Exec_DisconnectFromEchoChannel), ECVF_Default);
	}

	if (!IConsoleManager::Get().IsNameRegistered(TEXT("vxcp"))) {
		IConsoleManager::Get().RegisterConsoleCommand(TEXT("vxcp"), TEXT("Vivox connect to positional channel, required params: online game id, address, port, map"),
													  FConsoleCommandWithWorldAndArgsDelegate::CreateUObject(this, &UVivoxService::Exec_ConnectToPositionalChannel), ECVF_Default);
	}

	if (!IConsoleManager::Get().IsNameRegistered(TEXT("vxdp"))) {
		IConsoleManager::Get().RegisterConsoleCommand(TEXT("vxdp"), TEXT("Vivox disconnect from the active positional channel"),
													  FConsoleCommandWithWorldAndArgsDelegate::CreateUObject(this, &UVivoxService::Exec_DisconnectFromPositionalChannel),
													  ECVF_Default);
	}

	if (!IConsoleManager::Get().IsNameRegistered(TEXT("vxpu"))) {
		IConsoleManager::Get().RegisterConsoleCommand(TEXT("vxpu"), TEXT("Vivox print user information"),
													  FConsoleCommandWithWorldAndArgsDelegate::CreateUObject(this, &UVivoxService::Exec_PrintUserInfo), ECVF_Default);
	}
#endif
#pragma endregion
}

void UVivoxService::Deinitialize() {
	Super::Deinitialize();

	LogVerboseF("Vivox: Deinitializing subsystem.");

	if (!bInitialized) {
		return;
	}

#pragma region With Vivox
#if WITH_VIVOX

	if (IsLoggedIn()) {
		LogVerboseF("Vivox: Logging out.");
		VivoxLogout();
	}

	LogVerboseF("Vivox: Uninitialize voice client.");

	if (VivoxVoiceClient) {
		VivoxVoiceClient->Uninitialize();
	}

	LogVerboseF("Vivox: Unbind VivoxCore delegates.");

	OnVivoxLoggedInDelegate.RemoveDynamic(this, &UVivoxService::OnVivoxLoggedIn);
	OnVivoxLoggedOutDelegate.RemoveDynamic(this, &UVivoxService::OnVivoxLoggedOut);
#endif
#pragma endregion

#if USE_VERPC_VIVOX
	LogVerboseF("Vivox: Unbind WebSockets delegates.");

	GET_MODULE_SUBSYSTEM(WebSocketsRpcServiceSubsystem, Rpc, WebSocketsService);
	if (WebSocketsRpcServiceSubsystem) {
		WebSocketsRpcServiceSubsystem->OnLoggedIn.Remove(RpcServiceOnLoggedInHandle);
		WebSocketsRpcServiceSubsystem->OnLoggedOut.Remove(RpcServiceOnLoggedOutHandle);
	}
#else
	const auto WebSocketsService = GetGameInstance()->GetSubsystem<UArtheonWebSocketsService>();
	if (!WebSocketsService) { return; }

	LogWarningF("Vivox: Unbind WebSockets delegates.");

	WebSocketsService->OnWebSocketsLoggedIn.RemoveDynamic(this, &UVivoxService::OnWebSocketsLoggedIn);
	WebSocketsService->OnWebSocketsLoggedOut.RemoveDynamic(this, &UVivoxService::OnWebSocketsLoggedOut);
#endif
}
#pragma endregion

#pragma region With Vivox
#if WITH_VIVOX

VivoxCoreError UVivoxService::InitializeVivox(const int InLogLevel) {
	LogVerboseF("Vivox: Initialize Vivox.");

	if (bInitialized) {
		LogVerboseF("Vivox: Subsystem already initialized.");
		return VxErrorSuccess;
	}

#if PLATFORM_ANDROID
	RUN_WITH_PERMISSION(TEXT("android.permission.RECORD_AUDIO"), {
		LogF("Vivox: Requesting permissions.");
	});
#endif

	LogVerboseF("Vivox: Initialize Vivox Config.");
	VivoxConfig Config;
	Config.SetLogLevel(static_cast<vx_log_level>(InLogLevel));

	if (!VivoxVoiceClient) {
		LogErrorF("No vivox client");
		return VxErrorNotInitialized;
	}

	const VivoxCoreError Status = VivoxVoiceClient->Initialize(Config);

	if (Status == VxErrorAlreadyInitialized) {
		LogVerboseF("Vivox: Already initialized.");
		bInitialized = true;
		return Status;
	}

	if (Status != VxErrorSuccess) {
		LogErrorF("Vivox: Failed to initialize Vivox: %s (%d)", ANSI_TO_TCHAR(FVivoxCoreModule::ErrorToString(Status)), Status);
	} else {
		LogVerboseF("Vivox: Successfully initialized Vivox.");
		bInitialized = true;
	}

	return Status;
}

#pragma endregion

#pragma region Login

void UVivoxService::SetLoggedInAccountId(const AccountId& InAccountId) {
	LoggedInAccountId = InAccountId;
	bLoggedIn = true;

	LogVerboseF("Vivox: Set logged in account id to %s.", *InAccountId.Name());
}

void UVivoxService::ResetLoginState() {
	LoggedInAccountId = AccountId();
	bLoggedIn = false;

	LogVerboseF("Vivox: Reset login state.");
}

bool UVivoxService::IsLoggedIn() const {
	return bLoggedIn && LoggedInAccountId.IsValid() && !LoggedInAccountId.IsEmpty();
}

void UVivoxService::VivoxLogin(const FString& UserId) {
	LogVerboseF("Vivox: Login.");

	// const auto RandomFactor = FMath::RandRange(0,MAX_int32);
	// const FString VivoxLoginId = UserId; // FString::Printf(TEXT("%s-%d"), *UserId, RandomFactor); 

	if (IsLoggedIn()) {
		LogF("Vivox: Already logged in.");
		return;
	}

#if USE_VERPC_VIVOX

	GET_MODULE_SUBSYSTEM(WebSocketsRpcServiceSubsystem, Rpc, WebSocketsService);
	if (WebSocketsRpcServiceSubsystem) {
		if (WebSocketsRpcServiceSubsystem->IsAuthenticated()) {
			LogVerboseF("Vivox: Requesting login token.");
			RequestVivoxLoginToken(UserId);
			// RequestVivoxLoginToken(VivoxLoginId);
			bLoggingIn = true;
		} else {
			LogVerboseF("Vivox: WebSockets subsystem not authenticated, setting pending login user id.");
			PendingLoginUserId = UserId;
			// PendingLoginUserId = VivoxLoginId;
		}
	}

#else
	const auto GameInstance = GetGameInstance();
	if (!GameInstance) {
		return;
	}

	const auto WebSocketsService = GameInstance->GetSubsystem<UArtheonWebSocketsService>();
	if (!WebSocketsService) {
		return;
	}

	if (WebSocketsService->IsAuthenticated()) {
		LogVerboseF("Vivox: Requesting login token.");
		RequestVivoxLoginToken(UserId);
		// RequestVivoxLoginToken(VivoxLoginId);
		bLoggingIn = true;
	} else {
		LogVerboseF("Vivox: WebSockets subsystem not authenticated, setting pending login user id.");
		PendingLoginUserId = UserId;
		// PendingLoginUserId = VivoxLoginId;
	}
#endif
}

void UVivoxService::VivoxLogout() const {
	LogVerboseF("Vivox: Logging out.");
	if (!IsLoggedIn()) { return; }

	if (!VivoxVoiceClient) {
		LogErrorF("No vivox client");
		return;
	}

	for (auto Session : VivoxVoiceClient->LoginSessions()) {
		LogVerboseF("Vivox: Login session %s.", Session.Key.IsValid() ? *Session.Key.Name() : TEXT("INVALID"));
		if (Session.Value.IsValid() && Session.Value->State() != LoginState::LoggedOut && Session.Value->State() != LoginState::LoggingOut) {
			LogVerboseF("Vivox: Logging out session.");
			Session.Value->Logout();
		} else {
			LogVerboseF("Vivox: Session state {%d}.", Session.Value->State());
		}
	}
}

#pragma region WebSockets

void UVivoxService::RequestVivoxLoginToken(const FString& From) {
	LogVerboseF("Vivox: Requesting login token.");

#if USE_VERPC_VIVOX

	FArtheonWebSocketsVivoxPayload VivoxPayload;
	VivoxPayload.From = From;
	VivoxPayload.Action = ArtheonVivoxAction::Login;


	GET_MODULE_SUBSYSTEM(WebSocketsRpcServiceSubsystem, Rpc, WebSocketsService);
	if (!WebSocketsRpcServiceSubsystem) {
		return;
	}

	TSharedPtr<FJsonObject> BodyJsonObject = MakeShareable(new FJsonObject());

	// ApiKey
	const FString ApiKey = WebSocketsRpcServiceSubsystem->GetApiKey();
	BodyJsonObject->SetStringField(ArtheonWebSocketsJsonFields::Key, ApiKey);

	// VivoxPayload
	BodyJsonObject->SetObjectField(ArtheonWebSocketsJsonFields::VxPayload, VivoxPayload.ToJson());

	FRpcWebSocketsMessage RpcMessage(
		ERpcWebSocketsMessageType::Request,
		ERpcWebSocketsMessageTopic::Vivox,
		ArtheonWebSocketsMethods::VivoxGetLoginToken,
		BodyJsonObject
	);

	TSharedPtr<FRpcWebSocketsMessageCallback> CallbackPtr = MakeShareable(new FRpcWebSocketsMessageCallback());
	CallbackPtr->BindWeakLambda(this, [this](const FString& Method, const TSharedPtr<FJsonObject>& BodyJsonObject) {
		FString Status, StatusMessage;
		if (FRpcWebSocketsServiceSubsystem::CheckStatus(BodyJsonObject, Status, StatusMessage)) {
			OnReceivedLoginToken(BodyJsonObject->GetStringField(ArtheonWebSocketsJsonFields::Message), true);
		} else {
			LogErrorF("Vivox: Failed to obtain login token. Status: {%s}, message: {%s}.", *Status, *StatusMessage);
			OnReceivedLoginToken(BodyJsonObject->GetStringField(ArtheonWebSocketsJsonFields::Message), false);
		}
	});

	if (WebSocketsRpcServiceSubsystem->SendMessage(RpcMessage, CallbackPtr)) {
		LogVerboseF("Vivox: Sent login token request.");
		return;
	}

	LogErrorF("Vivox: Failed to sent login token request.");

#else
	
	const auto GameInstance = GetGameInstance();
	if (!GameInstance) { return; }

	const auto WebSocketsService = GameInstance->GetSubsystem<UArtheonWebSocketsService>();
	if (!WebSocketsService) { return; }

	if (!WebSocketsService->IsAuthenticated()) { return; }

	TSharedPtr<FArtheonWebSocketsRequest> Request = MakeShareable(new FArtheonWebSocketsRequest());
	Request->Message.Type = ArtheonWebSocketsType::RequestMessageType;
	Request->Message.Topic = ArtheonWebSocketsTopic::VivoxTopic;
	Request->Message.Method = ArtheonWebSocketsMethods::VivoxGetLoginToken;

	FArtheonWebSocketsVivoxPayload VivoxPayload;
	VivoxPayload.From = From;
	VivoxPayload.Action = ArtheonVivoxAction::Login;

	TSharedPtr<FArtheonWebSocketsPayload> Args = MakeShareable(new FArtheonWebSocketsPayload());
	Args->VivoxPayload = VivoxPayload;

	Request->Message.Args = Args;

	OnGetLoginTokenComplete.BindUObject(this, &UVivoxService::OnReceivedLoginToken);
	WebSocketsService->SendWebSocketsRequest(Request, OnGetLoginTokenComplete);

	LogVerboseF("Vivox: Sent login token request.");
#endif
}

#if USE_VERPC_VIVOX
void UVivoxService::OnReceivedLoginToken(const FString& Token, const bool bSuccessful) {
#else
void UVivoxService::OnReceivedLoginToken(const FArtheonWebSocketsMessage Response, const bool bSuccessful) {
#endif
	bLoggingIn = false;
	OnGetLoginTokenComplete.Unbind();

	if (!bInitialized) { return; }

#if USE_VERPC_VIVOX

	if (!bSuccessful) {
		LogErrorF("Vivox: Failed to obtain login token: %s", *Token);
		return;
	}

	LogVerboseF("Vivox: Received login token.");

	// Read JSON string.
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Token);
	TSharedPtr<FJsonObject> Object;
	FJsonSerializer::Deserialize(Reader, Object);

#else

	if (!bSuccessful) {
		LogErrorF("Vivox: Failed to obtain login token: %s", *Response.Payload->Message);
		return;
	}

	LogVerboseF("Vivox: Received login token.");

	// Read JSON string.
	const auto Token = Response.Payload->Message;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Token);
	TSharedPtr<FJsonObject> Object;
	FJsonSerializer::Deserialize(Reader, Object);
	
#endif

	// Parse JSON to the token data.
	FArtheonWebSocketsVivoxPayload TokenPayload;
	TokenPayload.ParseJson(Object);
	if (!TokenPayload.IsValid()) { return; }

	const auto LogInAccountId = AccountId::CreateFromUri(TokenPayload.From);
	if (!LogInAccountId.IsValid() || LogInAccountId.IsEmpty()) {
		LogErrorF("Vivox: Failed to login: invalid account id");
		return;
	}

	LogVerboseF("Vivox: Logging in as %s with token %s", *LogInAccountId.Name(), *Token);

	if (!VivoxVoiceClient) {
		LogErrorF("No vivox client");
		return;
	}

	ILoginSession& LoginSession = VivoxVoiceClient->GetLoginSession(LogInAccountId);
	ILoginSession::FOnBeginLoginCompletedDelegate OnBeginLoginCompleteCallback;
	OnBeginLoginCompleteCallback.BindWeakLambda(this, [this, &LoginSession](const VivoxCoreError Status) {
		bLoggingIn = false;
		if (VxErrorSuccess == Status) {
			LogVerboseF("Vivox: Logged in as %s", *LoginSession.LoginSessionId().Name());
			SetLoggedInAccountId(LoginSession.LoginSessionId());
			OnVivoxLoggedInDelegate.Broadcast();
		} else {
			LogErrorF("Vivox: Failed to login %s: %s (%d) (%d)", *LoginSession.LoginSessionId().Name(), ANSI_TO_TCHAR(FVivoxCoreModule::ErrorToString(Status)), Status,
					  LoginSession.State());
			UnbindLoginSessionHandlers(LoginSession);
			ResetLoginState();
		}
	});
	BindLoginSessionHandlers(LoginSession);
	bLoggingIn = true;

	const auto Status = LoginSession.BeginLogin(TokenPayload.Server, TokenPayload.Token, OnBeginLoginCompleteCallback);

	if (Status != VxErrorSuccess) {
		LogErrorF("Vivox: Failed to login %s: %s (%d)", *LoggedInAccountId.Name(), ANSI_TO_TCHAR(FVivoxCoreModule::ErrorToString(Status)), Status);
	}
}

#pragma endregion

void UVivoxService::BindLoginSessionHandlers(ILoginSession& LoginSession) {
	LogVerboseF("Vivox: Binding login session handlers.");
	LoginSession.EventStateChanged.AddUObject(this, &UVivoxService::OnLoginSessionStateChange);
}

void UVivoxService::UnbindLoginSessionHandlers(ILoginSession& LoginSession) const {
	LogVerboseF("Vivox: Unbinding login session handlers.");
	LoginSession.EventStateChanged.RemoveAll(this);
}

void UVivoxService::OnLoginSessionStateChange(const LoginState State) {
	LogVerboseF("Vivox: LoginSession state changed: %s", *UEnumShortToString(LoginState, State));

	switch (State) {
	case LoginState::LoggedOut:
		LogErrorF("Vivox: Unexpectedly logged out by network");
		ResetLoginState();
		break;
	default:
		break;
	}
}

#pragma endregion

#pragma region Channel

void UVivoxService::SetPositionalChannel(const ChannelId& InChannelId) {
	LogVerboseF("Vivox: Set positional channel to %s.", *InChannelId.Name());
	ConnectedPositionalChannel = InChannelId;
}

void UVivoxService::ResetPositionalChannel() {
	LogVerboseF("Vivox: Reset positional channel.");
	ConnectedPositionalChannel = ChannelId();
}

void UVivoxService::SetEchoChannel(const ChannelId& InChannelId) {
	LogVerboseF("Vivox: Set echo channel.");
	ConnectedEchoChannel = InChannelId;
}

void UVivoxService::ResetEchoChannel() {
	LogVerboseF("Vivox: Reset echo channel.");
	ConnectedEchoChannel = ChannelId();
}

bool UVivoxService::IsEchoChannelConnected() {
	if (!VivoxVoiceClient) { return false; }
	if (!IsLoggedIn()) { return false; }
	return ConnectedEchoChannel.IsValid()
		&& VivoxVoiceClient->GetLoginSession(LoggedInAccountId).GetChannelSession(ConnectedEchoChannel).ChannelState() == ConnectionState::Connected;
}

bool UVivoxService::IsPositionalChannelConnected() {
	if (!VivoxVoiceClient) { return false; }
	if (!IsLoggedIn()) { return false; }
	return ConnectedPositionalChannel.IsValid()
		&& VivoxVoiceClient->GetLoginSession(LoggedInAccountId).GetChannelSession(ConnectedPositionalChannel).ChannelState() == ConnectionState::Connected;
}

void UVivoxService::JoinServer(const FGuid& InServerId) {
	if (!IsLoggedIn()) {
		LogWarningF("vivox: not logged in");
		return;
	}

	if (!LoggedInAccountId.IsValid()) {
		LogWarningF("vivox: not logged in");
		return;
	}

	PendingServerId = InServerId;

	if (!IsPositionalChannelConnected()) {
		JoinPositionalChannel(InServerId.ToString(EGuidFormats::Digits));
	} else {
		LogF("vivox: already connected to positional channel: %s", *ConnectedPositionalChannel.Name());
	}
}

void UVivoxService::LeaveServer() {
	if (!IsLoggedIn()) {
		LogWarningF("vivox: not logged in");
		return;
	}

	if (!LoggedInAccountId.IsValid()) {
		LogWarningF("vivox: not logged in");
		return;
	}

	PendingServerId = {};

	StopSpeaking();

	if (IsPositionalChannelConnected()) {
		LeavePositionalChannel();
		ResetPositionalChannel();
	}

	bConnectedToPositionalChannel = false;
}

void UVivoxService::JoinOnlineGame(const FApiServerMetadata& OnlineGame) {
	LogF("Vivox: Joining online game.");

	if (!IsLoggedIn()) {
		return;
	}
	if (!LoggedInAccountId.IsValid()) {
		return;
	}

	PendingOnlineGame = OnlineGame;

	if (!IsPositionalChannelConnected()) {
		LogVerboseF("Vivox: Joining positional channel.");
		JoinPositionalChannel(OnlineGame.Id.ToString(EGuidFormats::Digits));
	} else {
		LogF("Vivox: Already connected to positional channel: %s.", *ConnectedPositionalChannel.Name());
	}
}

void UVivoxService::LeaveOnlineGame() {
	LogVerboseF("Vivox: Leaving online game.");

	if (!VivoxVoiceClient) { return; }
	if (!IsLoggedIn()) {
		return;
	}

	PendingOnlineGame = FApiServerMetadata();

	StopSpeaking();

	if (IsPositionalChannelConnected()) {
		LogVerboseF("Vivox: Leaving positional channel.");
		LeavePositionalChannel();
		ResetPositionalChannel();
	}
}

void UVivoxService::JoinPositionalChannel(const FString& InChannelId) {
	LogF("Vivox: Joining positional channel %s.", *InChannelId);

	if (!IsLoggedIn()) { return; }
	ensure(LoggedInAccountId.IsValid());

	if (!!InChannelId.IsEmpty()) { return; }

#if USE_VERPC_VIVOX
	GET_MODULE_SUBSYSTEM(WebSocketsRpcServiceSubsystem, Rpc, WebSocketsService);
	if (!WebSocketsRpcServiceSubsystem) { return; }
#else
	const auto GameInstance = GetGameInstance();
	if (!GameInstance) { return; }

	const auto WebSocketsService = GameInstance->GetSubsystem<UArtheonWebSocketsService>();
	if (!WebSocketsService) { return; }
#endif

	const auto From = LoggedInAccountId.Name();

	LogVerboseF("Vivox: Requesting token to join positional channel %s.", *InChannelId);
	RequestVivoxJoinPositionalChannelToken(From, InChannelId);
}

void UVivoxService::LeavePositionalChannel() {
	LogVerboseF("Vivox: Leaving positional channel.");

	if (!bInitialized) { return; }
	if (!IsLoggedIn()) { return; }
	if (!ConnectedPositionalChannel.IsValid()) {
		return;
	}

	if (!VivoxVoiceClient) {
		LogErrorF("No vivox client");
		return;
	}

	auto& LoginSession = VivoxVoiceClient->GetLoginSession(LoggedInAccountId);

	LogVerboseF("Vivox: Disconnecting from the positional channel %s", *ConnectedPositionalChannel.Name());

	UnbindChannelSessionHandlers(LoginSession.GetChannelSession(ConnectedPositionalChannel));
	LoginSession.DeleteChannelSession(ConnectedPositionalChannel);

	bConnectedToPositionalChannel = false;
}

void UVivoxService::RequestVivoxJoinPositionalChannelToken(const FString& From, const FString& To) {
	LogF("Vivox: Requesting positional channel join token.");

#if USE_VERPC_VIVOX

	FArtheonWebSocketsVivoxPayload VivoxPayload;
	VivoxPayload.Action = ArtheonVivoxAction::Join;
	VivoxPayload.From = From;
	VivoxPayload.To = To;
	VivoxPayload.ChannelProperties.ChannelType = EArtheonVivoxChannelType::Positional;
	VivoxPayload.ChannelProperties.Id = To;


	GET_MODULE_SUBSYSTEM(WebSocketsRpcServiceSubsystem, Rpc, WebSocketsService);
	if (!WebSocketsRpcServiceSubsystem) {
		return;
	}

	TSharedPtr<FJsonObject> BodyJsonObject = MakeShareable(new FJsonObject());

	// ApiKey
	const FString ApiKey = WebSocketsRpcServiceSubsystem->GetApiKey();
	BodyJsonObject->SetStringField(ArtheonWebSocketsJsonFields::Key, ApiKey);

	// VivoxPayload
	BodyJsonObject->SetObjectField(ArtheonWebSocketsJsonFields::VxPayload, VivoxPayload.ToJson());

	FRpcWebSocketsMessage RpcMessage(
		ERpcWebSocketsMessageType::Request,
		ERpcWebSocketsMessageTopic::Vivox,
		ArtheonWebSocketsMethods::VivoxGetJoinToken,
		BodyJsonObject
	);

	TSharedPtr<FRpcWebSocketsMessageCallback> CallbackPtr = MakeShareable(new FRpcWebSocketsMessageCallback());
	CallbackPtr->BindWeakLambda(this, [this](const FString& Method, const TSharedPtr<FJsonObject>& InBodyJsonObject) {
		FString Status, StatusMessage;
		if (FRpcWebSocketsServiceSubsystem::CheckStatus(InBodyJsonObject, Status, StatusMessage)) {
			OnReceivedVivoxJoinPositionalChannelToken(InBodyJsonObject->GetStringField(ArtheonWebSocketsJsonFields::Message), true);
		} else {
			LogErrorF("Vivox: Failed to obtain positional channel join token. Status: {%s}, message: {%s}.", *Status, *StatusMessage);
			OnReceivedVivoxJoinPositionalChannelToken(InBodyJsonObject->GetStringField(ArtheonWebSocketsJsonFields::Message), false);
		}
	});

	if (WebSocketsRpcServiceSubsystem->SendMessage(RpcMessage, CallbackPtr)) {
		LogVerboseF("Vivox: Sent join positional channel token request.");
		return;
	}

	LogErrorF("Vivox: Failed to sent join positional channel token request.");

#else

	const auto GameInstance = GetGameInstance();
	if (!GameInstance) { return; }

	const auto WebSocketsService = GameInstance->GetSubsystem<UArtheonWebSocketsService>();
	if (!WebSocketsService) { return; }

	if (!WebSocketsService->IsAuthenticated()) { return; }

	TSharedPtr<FArtheonWebSocketsRequest> Request = MakeShareable(new FArtheonWebSocketsRequest());
	Request->Message.Type = ArtheonWebSocketsType::RequestMessageType;
	Request->Message.Topic = ArtheonWebSocketsTopic::VivoxTopic;
	Request->Message.Method = ArtheonWebSocketsMethods::VivoxGetJoinToken;

	FArtheonWebSocketsVivoxPayload VivoxPayload;
	VivoxPayload.Action = ArtheonVivoxAction::Join;
	VivoxPayload.From = From;
	VivoxPayload.To = To;
	VivoxPayload.ChannelProperties.ChannelType = EArtheonVivoxChannelType::Positional;
	VivoxPayload.ChannelProperties.Id = To;

	TSharedPtr<FArtheonWebSocketsPayload> Args = MakeShareable(new FArtheonWebSocketsPayload());
	Args->VivoxPayload = VivoxPayload;

	Request->Message.Args = Args;

	OnGetJoinPositionalChannelTokenComplete.BindUObject(this, &UVivoxService::OnReceivedVivoxJoinPositionalChannelToken);

	WebSocketsService->SendWebSocketsRequest(Request, OnGetJoinPositionalChannelTokenComplete);

	LogVerboseF("Vivox: Sent join positional channel token request.");
#endif
}

#if USE_VERPC_VIVOX
void UVivoxService::OnReceivedVivoxJoinPositionalChannelToken(const FString& Token, bool bSuccessful) {
#else
void UVivoxService::OnReceivedVivoxJoinPositionalChannelToken(FArtheonWebSocketsMessage Response, bool bSuccessful) {
#endif
	OnGetJoinPositionalChannelTokenComplete.Unbind();

	if (!bInitialized) { return; }
	if (!IsLoggedIn()) { return; }
	if (!LoggedInAccountId.IsValid()) { return; }

#if USE_VERPC_VIVOX

	if (!bSuccessful) {
		LogErrorF("Vivox: Failed to obtain positional channel join token: %s", *Token);
		return;
	}

	LogVerboseF("Vivox: Received positional channel join token.");

	FArtheonWebSocketsVivoxPayload TokenPayload;

	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Token);
	TSharedPtr<FJsonObject> Object;
	FJsonSerializer::Deserialize(Reader, Object);
	TokenPayload.ParseJson(Object);
	if (!TokenPayload.IsValid()) { return; }

#else
	
	if (!bSuccessful) {
		LogErrorF("Vivox: Failed to obtain positional channel join token: %s", *Response.Payload->Message);
		return;
	}

	LogF("Vivox: Received positional channel join token.");

	FArtheonWebSocketsVivoxPayload TokenPayload;
#pragma region JSON
	const auto Token = Response.Payload->Message;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Token);
	TSharedPtr<FJsonObject> Object;
	FJsonSerializer::Deserialize(Reader, Object);
	TokenPayload.ParseJson(Object);
	if (!TokenPayload.IsValid()) { return; }
#pragma endregion
	
#endif

	if (!VivoxVoiceClient) {
		LogErrorF("No vivox client");
		return;
	}

	ILoginSession& LoginSession = VivoxVoiceClient->GetLoginSession(LoggedInAccountId);
	const auto Channel = ChannelId::CreateFromUri(TokenPayload.To);
	if (!Channel.IsValid()) { return; }

	IChannelSession& ChannelSession = LoginSession.GetChannelSession(Channel);

	LogVerboseF("Vivox: Joining positional channel %s with token %s", *TokenPayload.To, *Token);

	IChannelSession::FOnBeginConnectCompletedDelegate OnBeginConnectCompletedCallback;
	OnBeginConnectCompletedCallback.BindWeakLambda(this, [this, &LoginSession, &ChannelSession](const VivoxCoreError Status) {
		if (VxErrorSuccess == Status) {
			LogVerboseF("Vivox: Joined positional channel %s", *ChannelSession.Channel().Name());
			SetPositionalChannel(ChannelSession.Channel());

#if PLATFORM_ANDROID
            RUN_WITH_PERMISSION(TEXT("android.permission.RECORD_AUDIO"), {
                StartSpeaking(EArtheonVivoxChannelType::Positional);
            });
#endif

		} else {
			LogErrorF("Vivox: Failed to join positional channel %s: %s (%d)", *ChannelSession.Channel().Name(), ANSI_TO_TCHAR(FVivoxCoreModule::ErrorToString(Status)), Status);
			UnbindChannelSessionHandlers(ChannelSession);
			ResetPositionalChannel();
			LoginSession.DeleteChannelSession(ChannelSession.Channel());
		}
	});
	BindChannelSessionHandlers(ChannelSession);
	const auto Status = ChannelSession.BeginConnect(true, false, false, TokenPayload.Token, OnBeginConnectCompletedCallback);

	if (VxErrorSuccess != Status) {
		LogErrorF("Vivox: Failed to join positional channel %s: %s (%d)", *ChannelSession.Channel().Name(), ANSI_TO_TCHAR(FVivoxCoreModule::ErrorToString(Status)), Status);
		UnbindChannelSessionHandlers(ChannelSession);
		ResetPositionalChannel();
		LoginSession.DeleteChannelSession(ChannelSession.Channel());
	}

	bConnectedToPositionalChannel = true;
}

void UVivoxService::JoinEchoChannel() {
	LogVerboseF("Vivox: Joining echo channel.");

	if (!IsLoggedIn()) { return; }
	ensure(LoggedInAccountId.IsValid());

#if USE_VERPC_VIVOX
	GET_MODULE_SUBSYSTEM(WebSocketsRpcServiceSubsystem, Rpc, WebSocketsService);
	if (!WebSocketsRpcServiceSubsystem) { return; }
#else
	const auto GameInstance = GetGameInstance();
	if (!GameInstance) { return; }

	const auto WebSocketsService = GameInstance->GetSubsystem<UArtheonWebSocketsService>();
	if (!WebSocketsService) { return; }
#endif
	const auto From = LoggedInAccountId.Name();

	LogVerboseF("Vivox: Requesting token to join echo channel.");
	RequestVivoxJoinEchoChannelToken(From);
}

void UVivoxService::LeaveEchoChannel() {
	LogVerboseF("Vivox: Leaving echo channel.");

	if (!bInitialized) { return; }
	if (!IsLoggedIn()) { return; }
	if (!ConnectedEchoChannel.IsValid()) {
		return;
	}

	if (!VivoxVoiceClient) {
		LogErrorF("No vivox client");
		return;
	}

	auto& LoginSession = VivoxVoiceClient->GetLoginSession(LoggedInAccountId);

	LogVerboseF("Vivox: Disconnecting from the echo channel %s", *ConnectedEchoChannel.Name());

	UnbindChannelSessionHandlers(LoginSession.GetChannelSession(ConnectedEchoChannel));
	LoginSession.DeleteChannelSession(ConnectedEchoChannel);
}

void UVivoxService::RequestVivoxJoinEchoChannelToken(const FString& From) {
	LogVerboseF("Vivox: Requesting echo channel join token.");

#if USE_VERPC_VIVOX

	const FString To = TEXT("echo");

	FArtheonWebSocketsVivoxPayload VivoxPayload;
	VivoxPayload.Action = ArtheonVivoxAction::Join;
	VivoxPayload.From = From;
	VivoxPayload.To = To;
	VivoxPayload.ChannelProperties.ChannelType = EArtheonVivoxChannelType::Echo;
	VivoxPayload.ChannelProperties.Id = To;

	GET_MODULE_SUBSYSTEM(WebSocketsRpcServiceSubsystem, Rpc, WebSocketsService);
	if (!WebSocketsRpcServiceSubsystem) {
		return;
	}

	TSharedPtr<FJsonObject> BodyJsonObject = MakeShareable(new FJsonObject());

	// ApiKey
	const FString ApiKey = WebSocketsRpcServiceSubsystem->GetApiKey();
	BodyJsonObject->SetStringField(ArtheonWebSocketsJsonFields::Key, ApiKey);

	// VivoxPayload
	BodyJsonObject->SetObjectField(ArtheonWebSocketsJsonFields::VxPayload, VivoxPayload.ToJson());

	FRpcWebSocketsMessage RpcMessage(
		ERpcWebSocketsMessageType::Request,
		ERpcWebSocketsMessageTopic::Vivox,
		ArtheonWebSocketsMethods::VivoxGetJoinToken,
		BodyJsonObject
	);

	TSharedPtr<FRpcWebSocketsMessageCallback> CallbackPtr = MakeShareable(new FRpcWebSocketsMessageCallback());
	CallbackPtr->BindWeakLambda(this, [this](const FString& Method, const TSharedPtr<FJsonObject>& BodyJsonObject) {
		FString Status, StatusMessage;
		if (FRpcWebSocketsServiceSubsystem::CheckStatus(BodyJsonObject, Status, StatusMessage)) {
			OnReceivedVivoxJoinEchoChannelToken(BodyJsonObject->GetStringField(ArtheonWebSocketsJsonFields::Message), true);
		} else {
			LogErrorF("Vivox: Failed to obtain echo channel join token. Status: {%s}, message: {%s}.", *Status, *StatusMessage);
			OnReceivedVivoxJoinEchoChannelToken(BodyJsonObject->GetStringField(ArtheonWebSocketsJsonFields::Message), false);
		}
	});

	if (WebSocketsRpcServiceSubsystem->SendMessage(RpcMessage, CallbackPtr)) {
		LogVerboseF("Vivox: Sent join echo channel token request.");
		return;
	}

	LogErrorF("Vivox: Failed to sent join echo channel token request.");

#else
	
	const auto GameInstance = GetGameInstance();
	if (!GameInstance) { return; }

	const auto WebSocketsService = GameInstance->GetSubsystem<UArtheonWebSocketsService>();
	if (!WebSocketsService) { return; }

	if (!WebSocketsService->IsAuthenticated()) { return; }

	const FString To = TEXT("echo");

	TSharedPtr<FArtheonWebSocketsRequest> Request = MakeShareable(new FArtheonWebSocketsRequest());
	Request->Message.Type = ArtheonWebSocketsType::RequestMessageType;
	Request->Message.Topic = ArtheonWebSocketsTopic::VivoxTopic;
	Request->Message.Method = ArtheonWebSocketsMethods::VivoxGetJoinToken;

	FArtheonWebSocketsVivoxPayload VivoxPayload;
	VivoxPayload.Action = ArtheonVivoxAction::Join;
	VivoxPayload.From = From;
	VivoxPayload.To = To;
	VivoxPayload.ChannelProperties.ChannelType = EArtheonVivoxChannelType::Echo;
	VivoxPayload.ChannelProperties.Id = To;

	TSharedPtr<FArtheonWebSocketsPayload> Args = MakeShareable(new FArtheonWebSocketsPayload());
	Args->VivoxPayload = VivoxPayload;

	Request->Message.Args = Args;

	OnGetJoinEchoChannelTokenComplete.BindUObject(this, &UVivoxService::OnReceivedVivoxJoinEchoChannelToken);

	WebSocketsService->SendWebSocketsRequest(Request, OnGetJoinEchoChannelTokenComplete);

	LogVerboseF("Vivox: Sent join echo channel token request.");
#endif
}

#if USE_VERPC_VIVOX
void UVivoxService::OnReceivedVivoxJoinEchoChannelToken(const FString& Token, bool bSuccessful) {
#else
void UVivoxService::OnReceivedVivoxJoinEchoChannelToken(FArtheonWebSocketsMessage Response, bool bSuccessful) {
#endif
	OnGetJoinEchoChannelTokenComplete.Unbind();

	if (!bInitialized) { return; }
	if (!IsLoggedIn()) { return; }

#if USE_VERPC_VIVOX

	if (!bSuccessful) {
		LogErrorF("Vivox: Failed to obtain echo channel join token: %s", *Token);
		return;
	}

	LogVerboseF("Vivox: Received echo channel join token.");

	FArtheonWebSocketsVivoxPayload TokenPayload;

	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Token);
	TSharedPtr<FJsonObject> Object;
	FJsonSerializer::Deserialize(Reader, Object);
	TokenPayload.ParseJson(Object);
	if (!TokenPayload.IsValid()) { return; }

#else
	
	if (!bSuccessful) {
		LogErrorF("Vivox: Failed to obtain echo channel join token: %s", *Response.Payload->Message);
		return;
	}

	LogVerboseF("Vivox: Received echo channel join token.");

	FArtheonWebSocketsVivoxPayload TokenPayload;
#pragma region JSON
	const auto Token = Response.Payload->Message;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Token);
	TSharedPtr<FJsonObject> Object;
	FJsonSerializer::Deserialize(Reader, Object);
	TokenPayload.ParseJson(Object);
	if (!TokenPayload.IsValid()) { return; }
#pragma endregion
	
#endif

	if (!VivoxVoiceClient) {
		LogErrorF("No vivox client");
		return;
	}

	ILoginSession& LoginSession = VivoxVoiceClient->GetLoginSession(LoggedInAccountId);
	const auto Channel = ChannelId::CreateFromUri(TokenPayload.To);
	if (!Channel.IsValid()) { return; }

	IChannelSession& ChannelSession = LoginSession.GetChannelSession(Channel);

	LogVerboseF("Vivox: Joining echo channel %s with token %s", *TokenPayload.To, *Token);

	IChannelSession::FOnBeginConnectCompletedDelegate OnBeginConnectCompletedCallback;
	OnBeginConnectCompletedCallback.BindWeakLambda(this, [this, &LoginSession, &ChannelSession](const VivoxCoreError Status) {
		if (VxErrorSuccess == Status) {
			LogVerboseF("Vivox: Joined echo channel %s", *ChannelSession.Channel().Name());
			SetEchoChannel(ChannelSession.Channel());
		} else {
			LogErrorF("Vivox: Failed to join echo channel %s: %s (%d)", *ChannelSession.Channel().Name(), ANSI_TO_TCHAR(FVivoxCoreModule::ErrorToString(Status)), Status);
			UnbindChannelSessionHandlers(ChannelSession);
			ResetEchoChannel();
			LoginSession.DeleteChannelSession(ChannelSession.Channel());
		}
	});
	BindChannelSessionHandlers(ChannelSession);
	const auto Status = ChannelSession.BeginConnect(true, false, false, TokenPayload.Token, OnBeginConnectCompletedCallback);

	if (VxErrorSuccess != Status) {
		LogErrorF("Vivox: Failed to join echo channel for %s: %s (%d)", *ChannelSession.Channel().Name(), ANSI_TO_TCHAR(FVivoxCoreModule::ErrorToString(Status)), Status);
		UnbindChannelSessionHandlers(ChannelSession);
		ResetEchoChannel();
		LoginSession.DeleteChannelSession(ChannelSession.Channel());
	}
}

void UVivoxService::LeaveAllVoiceChannels() {
	LogVerboseF("Vivox: Leave all voice channels.");

	if (!bInitialized) { return; }
	if (!IsLoggedIn()) { return; }

	if (!VivoxVoiceClient) {
		LogErrorF("No vivox client");
		return;
	}

	TArray<ChannelId> ChannelSessionKeys;
	auto& LoginSession = VivoxVoiceClient->GetLoginSession(LoggedInAccountId);
	LoginSession.ChannelSessions().GenerateKeyArray(ChannelSessionKeys);
	for (ChannelId ChannelSessionKey : ChannelSessionKeys) {
		LogVerboseF("Vivox: Disconnecting from the channel %s", *ChannelSessionKey.Name());
		UnbindChannelSessionHandlers(LoginSession.GetChannelSession(ChannelSessionKey));
		LoginSession.DeleteChannelSession(ChannelSessionKey);
	}

	ResetPositionalChannel();
	ConnectedEchoChannel = ChannelId();
}

FString UVivoxService::GetChannelId(const FString& Id, EArtheonVivoxChannelType ChannelType) {
	return FString::Printf(TEXT("%d-%s"), static_cast<int>(ChannelType), *Id);
}

void UVivoxService::BindChannelSessionHandlers(IChannelSession& ChannelSession) {
	ChannelSession.EventAfterParticipantAdded.AddUObject(this, &UVivoxService::OnChannelParticipantAdded);
	ChannelSession.EventBeforeParticipantRemoved.AddUObject(this, &UVivoxService::OnChannelParticipantRemoved);
	ChannelSession.EventAfterParticipantUpdated.AddUObject(this, &UVivoxService::OnChannelParticipantUpdated);
	ChannelSession.EventAudioStateChanged.AddUObject(this, &UVivoxService::OnChannelAudioStateChanged);
	ChannelSession.EventTextStateChanged.AddUObject(this, &UVivoxService::OnChannelTextStateChanged);
	ChannelSession.EventChannelStateChanged.AddUObject(this, &UVivoxService::OnChannelStateChanged);
	ChannelSession.EventTextMessageReceived.AddUObject(this, &UVivoxService::OnChannelTextMessageReceived);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UVivoxService::UnbindChannelSessionHandlers(IChannelSession& ChannelSession) {
	ChannelSession.EventAfterParticipantAdded.RemoveAll(this);
	ChannelSession.EventBeforeParticipantRemoved.RemoveAll(this);
	ChannelSession.EventAfterParticipantUpdated.RemoveAll(this);
	ChannelSession.EventAudioStateChanged.RemoveAll(this);
	ChannelSession.EventTextStateChanged.RemoveAll(this);
	ChannelSession.EventChannelStateChanged.RemoveAll(this);
	ChannelSession.EventTextMessageReceived.RemoveAll(this);
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void UVivoxService::OnChannelParticipantAdded(const IParticipant& Participant) {
	const ChannelId Channel = Participant.ParentChannelSession().Channel();
	LogVerboseF("Vivox: User %s has joined the channel %s (self = %d)", *Participant.Account().Name(), *Channel.Name(), Participant.IsSelf());
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void UVivoxService::OnChannelParticipantRemoved(const IParticipant& Participant) {
	const ChannelId Channel = Participant.ParentChannelSession().Channel();
	LogVerboseF("Vivox: User %s has left the channel %s (self = %d)", *Participant.Account().Name(), *Channel.Name(), Participant.IsSelf());
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UVivoxService::OnChannelParticipantUpdated(const IParticipant& Participant) {
	const ChannelId Channel = Participant.ParentChannelSession().Channel();
	LogVerboseF("Vivox: User %s updated at the channel %s (self = %d), speaks = %d", *Participant.Account().Name(), *Channel.Name(), Participant.IsSelf(),
				Participant.SpeechDetected());

	const FGuid UserId(Participant.Account().Name());
	const bool bIsSpeaking = Participant.SpeechDetected();

	OnVivoxVoiceStateChanged.Broadcast(UserId, bIsSpeaking);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UVivoxService::OnChannelAudioStateChanged(const IChannelConnectionState& State) {
	LogVerboseF("Vivox: ChannelSession Audio State Changed at %s: %s", *State.ChannelSession().Channel().Name(), *UEnumShortToString(ConnectionState, State.State()));

	if (State.State() == ConnectionState::Connected) {
		LogVerboseF("Vivox: Connected to the channel.");
	} else {
		LogWarningF("Vivox: Disconnected from channel.");
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UVivoxService::OnChannelTextStateChanged(const IChannelConnectionState& State) {
	LogF("Vivox: ChannelSession Text State Change in %s: %s", *State.ChannelSession().Channel().Name(), *UEnumShortToString(ConnectionState, State.State()));
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UVivoxService::OnChannelStateChanged(const IChannelConnectionState& State) {
	LogF("Vivox: ChannelSession Connection State Change in %s: %s", *State.ChannelSession().Channel().Name(), *UEnumShortToString(ConnectionState, State.State()));
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void UVivoxService::OnChannelTextMessageReceived(const IChannelTextMessage& Message) {
	LogF("Vivox: Message Received from %s: %s", *Message.Sender().Name(), *Message.Message());
}

bool UVivoxService::IsSpeaking() const {
	if (!VivoxVoiceClient) {
		LogErrorF("No vivox client");
		return false;
	}

	auto& LoginSession = VivoxVoiceClient->GetLoginSession(LoggedInAccountId);
	if (LoginSession.GetTransmissionMode() == TransmissionMode::None) {
		return false;
	}
	return true;
}

VivoxCoreError UVivoxService::UpdateTransmissionMode() const {
	LogVerboseF("Vivox: Update transmission mode: echo %d, positional: %d", bTransmitToEchoChannel, bTransmitToPositionalChannel);

	if (!VivoxVoiceClient) {
		LogErrorF("No vivox client");
		return VxErrorNotInitialized;
	}

	auto& LoginSession = VivoxVoiceClient->GetLoginSession(LoggedInAccountId);

	const int Sum = bTransmitToPositionalChannel + bTransmitToEchoChannel;

	if (Sum == 0) {
		return LoginSession.SetTransmissionMode(TransmissionMode::None);
	}

	if (Sum >= 2) {
		return LoginSession.SetTransmissionMode(TransmissionMode::All);
	}

	if (bTransmitToEchoChannel) {
		return LoginSession.SetTransmissionMode(TransmissionMode::Single, ConnectedEchoChannel);
	}

	if (bTransmitToPositionalChannel) {
		return LoginSession.SetTransmissionMode(TransmissionMode::Single, ConnectedPositionalChannel);
	}

	return VxErrorInvalidState;
}

#pragma region 3D

bool UVivoxService::Get3DValuesAreDirty() const {
	return CachedPosition.IsDirty() ||
		CachedForwardVector.IsDirty() ||
		CachedUpVector.IsDirty();
}

void UVivoxService::Clear3DValuesAreDirty() {
	CachedPosition.SetDirty(false);
	CachedForwardVector.SetDirty(false);
	CachedUpVector.SetDirty(false);
}

void UVivoxService::Update3DPosition(APawn* Pawn) {
	if (!bInitialized) { return; }
	if (!IsLoggedIn()) { return; }
	if (!Pawn) { return; }
	if (!IsPositionalChannelConnected()) { return; }

	if (!VivoxVoiceClient) {
		LogErrorF("No vivox client");
		return;
	}

	if (VivoxVoiceClient->GetLoginSession(LoggedInAccountId).GetChannelSession(ConnectedPositionalChannel).ChannelState() != ConnectionState::Connected) {
		return;
	}

	CachedPosition.SetValue(Pawn->GetActorLocation());
	CachedForwardVector.SetValue(Pawn->GetActorForwardVector());
	CachedUpVector.SetValue(Pawn->GetActorUpVector());

	if (!Get3DValuesAreDirty()) {
		return;
	}

	// LogVerboseF("%s %s %s %s %s", *ConnectedPositionalChannel.Name(), *CachedPosition.GetValue().ToCompactString(),
	//      *CachedPosition.GetValue().ToCompactString(), *CachedForwardVector.GetValue().ToCompactString(), *CachedUpVector.GetValue().ToCompactString());

	ILoginSession& LoginSession = VivoxVoiceClient->GetLoginSession(LoggedInAccountId);
	LoginSession.GetChannelSession(ConnectedPositionalChannel).Set3DPosition(CachedPosition.GetValue(), CachedPosition.GetValue(), CachedForwardVector.GetValue(),
																			 CachedUpVector.GetValue());

	Clear3DValuesAreDirty();
}

#pragma endregion

#pragma endregion

#pragma region Input

VivoxCoreError UVivoxService::StartSpeaking(const EArtheonVivoxChannelType ChannelType) {
	LogVerboseF("Vivox: Try to start speaking.");
	if (!IsLoggedIn()) {
		return VxErrorNotLoggedIn;
	}

	if (!VivoxVoiceClient) {
		LogErrorF("No vivox client");
		return VxErrorNotInitialized;
	}

	auto& Session = VivoxVoiceClient->GetLoginSession(LoggedInAccountId);
	if (Session.State() != LoginState::LoggedIn) {
		LogWarningF("Vivox: Failed to start speaking: Not logged in.");
		return VxErrorNotLoggedIn;
	}

	LogVerboseF("Vivox: State %s, user %s", *UEnumShortToString(LoginState, Session.State()), *LoggedInAccountId.Name());

	switch (ChannelType) {
	case EArtheonVivoxChannelType::Positional:
		if (!ConnectedPositionalChannel.IsValid()) {
			LogWarningF("Vivox: Failed to start speaking: Not connected to a positional channel.");
			return VxErrorInvalidState;
		}

		if (Session.GetChannelSession(ConnectedPositionalChannel).ChannelState() != ConnectionState::Connected) {
			LogWarningF("Vivox: Failed to start speaking: Not connected to the positional channel.");
			return VxErrorInvalidState;
		}

		bTransmitToPositionalChannel = true;
		break;
	case EArtheonVivoxChannelType::Echo:
		if (!ConnectedEchoChannel.IsValid()) {
			LogWarningF("Vivox: Failed to start speaking: Not connected to an echo channel.");
			return VxErrorInvalidState;
		}

		if (Session.GetChannelSession(ConnectedEchoChannel).ChannelState() != ConnectionState::Connected) {
			LogWarningF("Vivox: Failed to start speaking: Not connected to the echo channel.");
			return VxErrorInvalidState;
		}

		bTransmitToEchoChannel = true;
		break;
	}

	LogVerboseF("Vivox: Started speaking.");
	return UpdateTransmissionMode();
}

VivoxCoreError UVivoxService::StopSpeaking() {
	LogVerboseF("Vivox: Try to stop speaking.");

	if (!IsLoggedIn()) {
		return VxErrorNotLoggedIn;
	}
	bTransmitToPositionalChannel = false;
	bTransmitToEchoChannel = false;

	if (!VivoxVoiceClient) {
		LogErrorF("No vivox client");
		return VxErrorNotInitialized;
	}

	auto& LoginSession = VivoxVoiceClient->GetLoginSession(LoggedInAccountId);

	LogVerboseF("Vivox: Stopped speaking.");
	return LoginSession.SetTransmissionMode(TransmissionMode::None);
}

#pragma endregion

#pragma region Commands

// ReSharper disable once CppMemberFunctionMayBeStatic
void UVivoxService::Exec_ConnectToEchoChannel(const TArray<FString>& Args, UWorld* World) {
	LogF("Vivox: Connect to Echo Channel");
	JoinEchoChannel();
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void UVivoxService::Exec_DisconnectFromEchoChannel(const TArray<FString>& Args, UWorld* World) {
	LogF("Vivox: Disconnect from Echo Channel");
	LeaveEchoChannel();
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void UVivoxService::Exec_ChangeActiveChannel(const TArray<FString>& Args, UWorld* World) {
	if (Args.Num() != 1) {
		LogWarningF("Vivox: Please enter 'p' or 'e' for the channel type.");
		return;
	}

	if (!World) { return; }

	AVePlayerController* LocalPlayerController = nullptr;
	for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator) {
		APlayerController* PlayerController = (*Iterator).Get();
		if (PlayerController && PlayerController->IsLocalPlayerController()) {
			LocalPlayerController = Cast<AVePlayerController>(PlayerController);
		}
	}
	if (!LocalPlayerController) { return; }

	if (Args[0] == TEXT("p")) {
		LocalPlayerController->SetSelectedVoiceChannel(EArtheonVivoxChannelType::Positional);
		LogF("Vivox: Changed an Active Channel to Positional");
	} else if (Args[0] == TEXT("e")) {
		LocalPlayerController->SetSelectedVoiceChannel(EArtheonVivoxChannelType::Echo);
		LogF("Vivox: Changed an Active Channel to Echo");
	} else {
		LogWarningF("Vivox: Please enter 'p' or 'e' for the channel type.");
	}
}

void UVivoxService::Exec_ConnectToPositionalChannel(const TArray<FString>& Args, UWorld* World) {
	if (Args.Num() < 1) {
		LogErrorF("Vivox: Required argument is missing. Channel id is required.");
		return;
	}
	LogF("Vivox: Connect to the positional channel %s.", *Args[0]);

	JoinPositionalChannel(Args[0]);
}

void UVivoxService::Exec_DisconnectFromPositionalChannel(const TArray<FString>& Args, UWorld* World) {
	LogF("Vivox: Disconnect from positional channel");

	LeavePositionalChannel();
}

void UVivoxService::Exec_PrintUserInfo(const TArray<FString>& Args, UWorld* World) const {
	if (!IsLoggedIn()) {
		LogWarningF("Vivox: User {%s} not logged in.", *LoggedInAccountId.Name());
		return;
	}

	if (!VivoxVoiceClient) {
		LogErrorF("No vivox client");
		return;
	}

	auto& LoginSession = VivoxVoiceClient->GetLoginSession(LoggedInAccountId);
	const auto UserPresenceStatus = UEnumShortToString(PresenceStatus, LoginSession.CurrentPresence().CurrentStatus());
	const auto UserLoginState = UEnumShortToString(LoginState, LoginSession.State());
	FString ChannelSessions = TEXT("Channel Sessions: ");
	for (auto ChannelSession : LoginSession.ChannelSessions()) {
		ChannelSessions.Appendf(TEXT("%s "), *ChannelSession.Key.Name());
	}

	LogF("Vivox: Name: {%s}, state: {%s}, presence: {%s}, channels: {%s}.", *LoggedInAccountId.Name(), *UserLoginState, *UserPresenceStatus, *ChannelSessions);
}

#pragma endregion

#endif

#pragma endregion
