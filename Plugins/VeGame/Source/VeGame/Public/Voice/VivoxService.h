// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "VeShared.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ApiServerMetadata.h"
#include "ArtheonWebSocketsRequest.h"

#pragma region Win64 Client
#if WITH_VIVOX
#include "VivoxCore.h"
#endif
#pragma endregion

#include "VivoxService.generated.h"

namespace ArtheonVivoxAction {
    const auto Login = TEXT("login");
    const auto Join = TEXT("join");
    const auto Mute = TEXT("mute");
    const auto Unmute = TEXT("unmute");
    const auto Kick = TEXT("kick");
}

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnVivoxLoggedIn);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnVivoxLoggedOut);


/**
 * 
 */
UCLASS()
class VEGAME_API UVivoxService final : public UGameInstanceSubsystem {
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable, Category = "Artheon")
    FOnVivoxLoggedIn OnVivoxLoggedInDelegate;

    UPROPERTY(BlueprintAssignable, Category = "Artheon")
    FOnVivoxLoggedOut OnVivoxLoggedOutDelegate;

    DECLARE_EVENT_TwoParams(UVivoxService, FOnVivoxVoiceStateChanged, const FGuid& /*UserId*/, bool /*bIsSpeaking*/);
    FOnVivoxVoiceStateChanged OnVivoxVoiceStateChanged;

    UFUNCTION()
    void OnWebSocketsLoggedIn();
    UFUNCTION()
    void OnWebSocketsLoggedOut();

    UFUNCTION()
    void OnVivoxLoggedIn();
    UFUNCTION()
    void OnVivoxLoggedOut();

    UVivoxService();

    bool bInitialized = false;
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    bool IsInitialized() const { return bInitialized; }

#pragma region Admin Features
 #if WITH_RPC && UE_SERVER
    FOnCppWebSocketsRequestComplete OnMuteAtPositionalChannelCompleteDelegate;
    void MuteAtPositionalChannel(const FString& InChannelId, const FString& InAdminId, const FString& InUserId);
    void RequestMuteAtPositionalChannel(const FString& From, const FString& To, const FString& Sub);
#if USE_VERPC_VIVOX
    void OnMuteAtPositionalChannelComplete(const FString& Token, bool bSuccessful);
#else
    void OnMuteAtPositionalChannelComplete(FArtheonWebSocketsMessage Response, bool bSuccessful);
#endif

    FOnCppWebSocketsRequestComplete OnUnmuteAtPositionalChannelCompleteDelegate;
    void UnmuteAtPositionalChannel(const FString& InChannelId, const FString& InAdminId, const FString& InUserId);
    void RequestUnmuteAtPositionalChannel(const FString& From, const FString& To, const FString& Sub);
#if USE_VERPC_VIVOX
    void OnUnmuteAtPositionalChannelComplete(const FString& Token, bool bSuccessful);
#else
    void OnUnmuteAtPositionalChannelComplete(FArtheonWebSocketsMessage Response, bool bSuccessful);
#endif

    FOnCppWebSocketsRequestComplete OnKickCompleteDelegate;
    void KickFromPositionalChannel(const FString& InChannelId, const FString& InAdminId, const FString& InUserId);
    void RequestKickFromPositionalChannel(const FString& From, const FString& To, const FString& Sub);
#if USE_VERPC_VIVOX
    void OnKickFromPositionalChannelComplete(const FString& Token, bool bSuccessful);
#else
    void OnKickFromPositionalChannelComplete(FArtheonWebSocketsMessage Response, bool bSuccessful);
#endif
#endif
#pragma endregion

#if WITH_VIVOX
private:
    IClient* VivoxVoiceClient;

#pragma region Initialization
    VivoxCoreError InitializeVivox(int InLogLevel = 1);
#pragma endregion

#pragma region Login
    bool bLoggedIn = false;
    bool bLoggingIn = false;

    /**
     * User ID to use to login.
     * Note that the WebSockets and Vivox initialization processes both are asynchronous.
     * We cache pending user id that will be used when the system is ready to proceed with authentication.
     */
    FString PendingLoginUserId = TEXT("");
    /**
     * Online game to use to join channels.
     * Note that the WebSockets and Vivox initialization processes both are asynchronous.
     * We cache pending online game that will be used when the system is ready to proceed with channel join. 
     */
    FApiServerMetadata PendingOnlineGame = FApiServerMetadata();

    FGuid PendingServerId = {};

    AccountId LoggedInAccountId;
    void SetLoggedInAccountId(const AccountId& InAccountId);
    void ResetLoginState();

    FOnCppWebSocketsRequestComplete OnGetLoginTokenComplete;

public:
    bool IsLoggedIn() const;

    /** Login to Vivox using the user id. Initiates the procedure by requesting the token via WebSockets. Further processing is asynchronous. */
    void VivoxLogin(const FString& UserId);
    void VivoxLogout() const;

private:
    void RequestVivoxLoginToken(const FString& From);
    /** Callback for login token request operation. */
#if USE_VERPC_VIVOX
    void OnReceivedLoginToken(const FString& Token, const bool bSuccessful);
#else
    void OnReceivedLoginToken(FArtheonWebSocketsMessage Response, bool bSuccessful);
#endif
    /** Binds login session handlers. */
    void BindLoginSessionHandlers(ILoginSession& LoginSession);
    /** Unbinds login session handlers. */
    void UnbindLoginSessionHandlers(ILoginSession& LoginSession) const;
    /** Handles the login session state changes. */
    void OnLoginSessionStateChange(LoginState State);

#pragma endregion

#pragma region Channel Control

    /** Local space positional channel. Anyone can listen and speak. */
    ChannelId ConnectedPositionalChannel;
    void SetPositionalChannel(const ChannelId& InChannelId);
    void ResetPositionalChannel();
    bool IsPositionalChannelConnected();
    bool bConnectedToPositionalChannel = false;
public:
    bool GetIsConnectedToPositionalChannel() const { return bConnectedToPositionalChannel; }
private:
    bool bTransmitToPositionalChannel = false;

    /** Echo channel. Only admins can speak and listen. */
    ChannelId ConnectedEchoChannel;
    void SetEchoChannel(const ChannelId& InChannelId);
    void ResetEchoChannel();
    bool IsEchoChannelConnected();
    bool bTransmitToEchoChannel = false;

public:
    FApiServerMetadata CurrentOnlineGame;
    void JoinServer(const FGuid& InServerId);
    void LeaveServer();
    void JoinOnlineGame(const FApiServerMetadata& OnlineGame);
    void LeaveOnlineGame();

private:
    void JoinPositionalChannel(const FString& InChannelId = TEXT(""));
    void LeavePositionalChannel();
    void RequestVivoxJoinPositionalChannelToken(const FString& From, const FString& To);
#if USE_VERPC_VIVOX
    void OnReceivedVivoxJoinPositionalChannelToken(const FString& Token, bool bSuccessful);
#else
    void OnReceivedVivoxJoinPositionalChannelToken(FArtheonWebSocketsMessage Response, bool bSuccessful);
#endif
    FOnCppWebSocketsRequestComplete OnGetJoinPositionalChannelTokenComplete;

    void JoinEchoChannel();
    void LeaveEchoChannel();
    void RequestVivoxJoinEchoChannelToken(const FString& From);
#if USE_VERPC_VIVOX
    void OnReceivedVivoxJoinEchoChannelToken(const FString& Token, bool bSuccessful);
#else
    void OnReceivedVivoxJoinEchoChannelToken(FArtheonWebSocketsMessage Response, bool bSuccessful);
#endif    
    FOnCppWebSocketsRequestComplete OnGetJoinEchoChannelTokenComplete;

public:
    /** Leaves all connected voice channels. */
    void LeaveAllVoiceChannels();

private:
    static FString GetChannelId(const FString& Id, EArtheonVivoxChannelType ChannelType);

    /** Binds channel session handlers. */
    void BindChannelSessionHandlers(IChannelSession& ChannelSession);
    /** Unbinds channel session handlers. */
    void UnbindChannelSessionHandlers(IChannelSession& ChannelSession);

    void OnChannelParticipantAdded(const IParticipant& Participant);
    void OnChannelParticipantRemoved(const IParticipant& Participant);
    void OnChannelParticipantUpdated(const IParticipant& Participant);
    void OnChannelAudioStateChanged(const IChannelConnectionState& State);
    void OnChannelTextStateChanged(const IChannelConnectionState& State);
    void OnChannelStateChanged(const IChannelConnectionState& State);
    void OnChannelTextMessageReceived(const IChannelTextMessage& Message);

    bool IsSpeaking() const;

    /** Updates the voice client login session transmission mode depending on how many channels are active. */
    VivoxCoreError UpdateTransmissionMode() const;

    bool Get3DValuesAreDirty() const;
    void Clear3DValuesAreDirty();
public:
    /** Updates the 3d position of the character. */
    void Update3DPosition(APawn* Pawn);

    TCachedProperty<FVector> CachedPosition = TCachedProperty<FVector>(FVector());
    TCachedProperty<FVector> CachedForwardVector = TCachedProperty<FVector>(FVector());
    TCachedProperty<FVector> CachedUpVector = TCachedProperty<FVector>(FVector());
#pragma endregion

#pragma region Input
    VivoxCoreError StartSpeaking(EArtheonVivoxChannelType ChannelType);
    VivoxCoreError StopSpeaking();
#pragma endregion

#pragma region Debug
    /** vxce */
    void Exec_ConnectToEchoChannel(const TArray<FString>& Args, UWorld* World);
    /** vxde */
    void Exec_DisconnectFromEchoChannel(const TArray<FString>& Args, UWorld* World);
    /** vxcc */
    void Exec_ChangeActiveChannel(const TArray<FString>& Args, UWorld* World);
    /** vxcp */
    void Exec_ConnectToPositionalChannel(const TArray<FString>& Args, UWorld* World);
    /** vxdp */
    void Exec_DisconnectFromPositionalChannel(const TArray<FString>& Args, UWorld* World);
    /** vxpu */
    void Exec_PrintUserInfo(const TArray<FString>& Args, UWorld* World) const;
#pragma endregion

#endif

#if USE_VERPC_VIVOX
private:
    FDelegateHandle RpcServiceOnLoggedInHandle;
    FDelegateHandle RpcServiceOnLoggedOutHandle;
#endif
    
};
