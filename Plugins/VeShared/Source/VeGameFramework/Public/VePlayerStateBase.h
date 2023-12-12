// Author: Egor A. Pristavka

#pragma once

#include "VeUserMetadata.h"
#include "GameFramework/PlayerState.h"
#include "VePlayerStateBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSpeechRecognizedDelegate, const FString&, Message);

/**
 * Replicated player state
 */
UCLASS(BlueprintType, Blueprintable)
class VEGAMEFRAMEWORK_API AVePlayerStateBase : public APlayerState {
	GENERATED_BODY()

	friend class AVeGameMode;

	DECLARE_EVENT_OneParam(AVePlayerStateBase, FOnUserMetadataUpdated, const FVeUserMetadata& /*InUserMetadata*/);
	DECLARE_EVENT_OneParam(AVePlayerStateBase, FOnPlayerState, AVePlayerStateBase* PlayerState);

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;

protected:
	virtual void BeginPlay() override; 
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

#pragma region InitializationComplete
#if 0
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsClientInitializationComplete() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsServerInitializationComplete() const;

	UFUNCTION(BlueprintCallable)
	void ClientInitializationComplete();

	UFUNCTION(BlueprintCallable)
	void ServerInitializationComplete();

	FOnPlayerState OnClientInitializationComplete;
	FOnPlayerState OnServerInitializationComplete;

protected:
	virtual bool NativeCheckClientInitializationComplete();
	virtual bool NativeCheckServerInitializationComplete();
	
	UFUNCTION(BlueprintNativeEvent)
	bool CheckClientInitializationComplete();

	UFUNCTION(BlueprintNativeEvent)
	bool CheckServerInitializationComplete();
	
	virtual void NativeOnClientInitializationComplete();
	virtual void NativeOnServerInitializationComplete();
	
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnClientInitializationComplete"))
	void OnClientInitializationComplete_BP();

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnServerInitializationComplete"))
	void OnServerInitializationComplete_BP();

private:
	bool bClientInitializationComplete = false;
	bool bServerInitializationComplete = false;

	UFUNCTION(Client, Reliable)
	void Client_InitializationComplete();

	UFUNCTION(Server, Reliable)
	void Server_InitializationComplete();

#endif
#pragma endregion InitializationComplete

#pragma region User Metadata

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	const FVeUserMetadata& GetUserMetadata() const { return UserMetadata; }

	/** Returns user ID for this player state. */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	const FGuid& GetUserId() const { return UserMetadata.Id; }

	/** Is the user admin or not. */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsAdmin() const { return UserMetadata.bIsAdmin; }

	/** Delegate for user metadata updates. */
	FOnUserMetadataUpdated& GetOnUserMetadataUpdated() { return OnUserMetadataUpdated; }
	
protected:
	virtual void NativeOnUserMetadataUpdated();

private:
	/** Sets the user metadata, authority only, called by the game state during user login process. */
	void Login_SetUserMetadata(const FVeUserMetadata& InUserMetadata);

	/** Replicated user metadata. */
	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_UserMetadata)
	FVeUserMetadata UserMetadata;

	/** Replication callback to broadcast user metadata changes during network updates. */
	UFUNCTION()
	void OnRep_UserMetadata();

	/** Delegate for user metadata updates. */
	FOnUserMetadataUpdated OnUserMetadataUpdated;

#pragma endregion User Metadata

#pragma region Voice

public:
	UFUNCTION()
	void Authority_SetIsSpeaking(bool bInIsSpeaking);

private:
	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_IsSpeaking)
	bool bIsSpeaking = false;

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetIsSpeaking() const { return bIsSpeaking; }

	UFUNCTION()
	void OnRep_IsSpeaking() const;

	DECLARE_EVENT_OneParam(AVePlayerState, FOnIsSpeakingChanged, bool bInIsSpeaking);

private:
	FOnIsSpeakingChanged OnIsSpeakingChanged;

public:
	FOnIsSpeakingChanged& GetOnIsSpeakingChanged() { return OnIsSpeakingChanged; }

	// void MyFunc()

#pragma endregion Voice

#pragma region Speech Recognition

public:
	DECLARE_EVENT_OneParam(AVeGameStateBase, FSpeechRecognizedEvent, const FString& Message);

	FSpeechRecognizedDelegate OnSpeechRecognized;
	FSpeechRecognizedEvent SpeechRecognizedEvent;

	/**
	 * @brief Called by the player controller when speech recognition is completed
	 * @param InResult Transcribed text
	 */
	void OnSpeechRecognitionCompleted(const FString& InResult);

	/**
	 * @brief Speech recognition message replicated to the clients
	 */
	UPROPERTY(ReplicatedUsing="OnRep_SpeechRecognitionMessage", meta=(AllowPrivateAccess))
	FString SpeechRecognitionMessage;

private:
	/**
	 * @brief Called when the speech recognition message is replicated
	 */
	UFUNCTION()
	void OnRep_SpeechRecognitionMessage() const;

	/**
	 * @brief Called on the client when the speech recognition is completed
	 */
	UFUNCTION()
	void ClientSpeechRecognitionComplete() const;
#pragma endregion
	
};
