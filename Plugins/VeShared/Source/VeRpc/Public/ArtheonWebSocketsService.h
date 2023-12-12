// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "ArtheonWebSocketsRequest.h"

#include "WebSocketsModule.h"
#include "IWebSocket.h"
#include "ArtheonWebSocketsPayload.h"
#include "ArtheonWebSocketsService.generated.h"

/** Used as a callback for API requests. */ 
DECLARE_DELEGATE_TwoParams(FArtheonWebSocketsCallDelegate, FArtheonWebSocketsRequest, bool);

DECLARE_MULTICAST_DELEGATE(FOnWebSocketsInitialized);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWebSocketsLoggedIn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWebSocketsLoggedOut);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWebsocketsReady);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWebsocketsConnected);

// DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnWebSocketsChatMessageReceived, FString, ChannelId, FString, Message, FString, Category, FArtheonWebSocketsUser, User);
//
// DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWebSocketsChatSendComplete, FString, ChannelId, FString, Message);
//
// DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWebSocketsChatSubscribeComplete, FString, ChannelId, FString, Category);
//
// DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWebSocketsChatUnsubscribeComplete, FString, ChannelId);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWebsocketsMessagesResetCompleted);

UCLASS()
class VERPC_API UArtheonWebSocketsService final : public UGameInstanceSubsystem {
    GENERATED_BODY()

public:
    UArtheonWebSocketsService();
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    /** User API key used to authenticate requests. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FString WebSocketsUserKey;

    FOnWebSocketsInitialized OnWebSocketsInitialized;

    UFUNCTION(BlueprintCallable)
    void WebSocketsLogin(const FString& InKey);

    FOnCppWebSocketsRequestComplete LoginCompleteCallback;
    void OnWebSocketsLoginComplete(FArtheonWebSocketsMessage Message, bool bWasSuccessful);

    UPROPERTY(BlueprintAssignable, Category = "Artheon")
    FOnWebsocketsReady WebSocketsReady;

    UPROPERTY(BlueprintAssignable, Category = "Artheon")
    FOnWebsocketsConnected OnWebSocketsConnected;

    UPROPERTY(BlueprintAssignable, Category = "Artheon")
    FOnWebSocketsLoggedIn OnWebSocketsLoggedIn;

    UPROPERTY(BlueprintAssignable, Category = "Artheon")
    FOnWebSocketsLoggedOut OnWebSocketsLoggedOut;

    UPROPERTY(BlueprintAssignable, Category = "Artheon")
    FOnWebsocketsMessagesResetCompleted WebsocketsMessagesResetCompleted;

    UPROPERTY()
    bool bInitialized;

    UPROPERTY()
    bool bAuthenticated = false;

    UPROPERTY()
    bool bIsServer;

    UFUNCTION(BlueprintGetter)
    bool IsConnected() const;
    
    /** Returns true if service is fully initialized and ready to go. */
    UFUNCTION(BlueprintGetter)
    bool IsReady() const { return bInitialized; }

    /** Returns true if service has authenticated within the RPC framework. */
    UFUNCTION(BlueprintGetter)
    bool IsAuthenticated() const { return bAuthenticated; }

    void Connect(const FString& InKey, bool InIsServer = false);
    void Disconnect();

protected:
    UPROPERTY()
    FString Key;

    TArray<FArtheonWebSocketsMessage> SavedMessages;

    TSharedPtr<IWebSocket> WebSocketPtr = nullptr;

    TMap<FGuid, TSharedPtr<FArtheonWebSocketsRequest>> WebSocketsRequests;

    static FWebSocketsModule* GetWebSocketsModule();

    // Delegate callback.
    void OnWebSocketMessageReceive(const FString& InMessage);
    // Delegate callback.
    static void OnWebSocketMessageSent(const FString& Message);
    // Delegate callback.
    void OnWebSocketConnect();
    // Delegate callback.
    void OnWebSocketConnectionError(const FString& Error);
    // Delegate callback.
    static void OnWebSocketClose(int32 StatusCode, const FString& Reason, bool bWasClean);

    void ProcessReceivedWebSocketsMessage(const FString& InJsonMessageStr);

    /** Completes a previously sent request by its Id, triggers request delegates. */
    void CompleteRequest(const FGuid& Id, const bool bSuccessful, const FArtheonWebSocketsMessage& ResponseMessage);

    // void OnActiveOnlineGameChange(FArtheonOnlineGame NewGame);
    void OnActiveSpaceChanged(FGuid InNewSpaceId);

    void CacheNewMessage(const FArtheonWebSocketsMessage& Message);

public:
    /** Public version, requires the websockets to be authenticated. */
    bool SendWebSocketsMessage(FArtheonWebSocketsMessage Message) const;
    /** Public version, requires the websockets to be authenticated. */
    bool SendWebSocketsRequest(TSharedPtr<FArtheonWebSocketsRequest> Request, const FOnCppWebSocketsRequestComplete& CallbackDelegate);

private:
    /** Internal version, skips the authentication check. */
    bool SendWebSocketsMessageInternal(FArtheonWebSocketsMessage Message) const;
    /** Internal version, skips the authentication check. */
    bool SendWebSocketsRequestInternal(TSharedPtr<FArtheonWebSocketsRequest> Request, const FOnCppWebSocketsRequestComplete& CallbackDelegate);

    bool bConnectionInProcess = false;
};
