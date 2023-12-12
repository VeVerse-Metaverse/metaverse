// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#pragma once

#include "VeAiCharacter.h"
#include "VeUserMetadata.h"
#include "VeAiPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAiUserMetadataUpdatedDelegate, const FVeUserMetadata&, UserMetadata);

/**
 * Replicated AI player state
 */
UCLASS(BlueprintType, Blueprintable)
class VEAI_API AVeAiPlayerState : public AInfo {
	GENERATED_BODY()

	friend class AVeGameMode;

	DECLARE_EVENT_OneParam(AVeAiPlayerState, FOnAiPlayerState, AVeAiPlayerState* PlayerState);

public:
	AVeAiPlayerState(const FObjectInitializer& ObjectInitializer);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

#pragma region User Metadata

private:
	friend class AVeAiController;

	/**
	 * @brief Sets the user metadata called by the AI controller.
	 */
	void Login_SetUserMetadata(const FVeUserMetadata& InUserMetadata);

	/**
	 * @brief Replicated user metadata.
	 */
	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_UserMetadata)
	FVeUserMetadata UserMetadata;

	/**
	 * @brief Delegate for user metadata updates.
	 */
	UPROPERTY(BlueprintAssignable)
	FOnAiUserMetadataUpdatedDelegate OnUserMetadataUpdated;

public:
	/**
	 * @brief Returns user metadata for this player state.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVeUserMetadata GetUserMetadata() const {
		return UserMetadata;
	}

	/**
	 * @brief Returns user ID for this player state.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FGuid GetUserId() const {
		return UserMetadata.Id;
	}

	/**
	 * @brief Replication callback to broadcast user metadata changes during network updates.
	 */
	UFUNCTION()
	void OnRep_UserMetadata() const;

private:
	DECLARE_EVENT_OneParam(AVeAiPlayerState, FOnAiUserMetadataUpdated, const FVeUserMetadata& /*InUserMetadata*/);

	/** @brief Delegate for user metadata updates. */
	FOnAiUserMetadataUpdated OnAiUserMetadataUpdated;

public:
	/** @brief Delegate for user metadata updates. */
	FOnAiUserMetadataUpdated& GetOnAiUserMetadataUpdated();

#pragma endregion User Metadata

#pragma region AI Voice

	/**
	 * @brief Sets the is speaking flag, called by the AI controller.
	 */
	UFUNCTION()
	void Authority_SetIsSpeaking(bool bInIsSpeaking);

	/**
	 * @brief Replicated is speaking flag.
	 */
	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_IsSpeaking)
	bool bIsSpeaking = false;

	/**
	 * @brief Returns is speaking flag for this player state.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetIsSpeaking() const {
		return bIsSpeaking;
	}

	/**
	 * @brief Replication callback to broadcast is speaking changes during network updates.
	 */
	UFUNCTION()
	void OnRep_IsSpeaking() const;

	DECLARE_EVENT_OneParam(AVePlayerState, FOnIsSpeakingChanged, bool bInIsSpeaking);

private:
	FOnIsSpeakingChanged OnIsSpeakingChanged;

public:
	FOnIsSpeakingChanged& GetOnIsSpeakingChanged() {
		return OnIsSpeakingChanged;
	}

#pragma endregion AI Voice

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAIPlayerStatePawnSet, AVeAiPlayerState*, AIPlayer, APawn*, NewPawn, APawn*, OldPawn);

	/**
	 * @brief Broadcast whenever this AI player's possessed pawn is set
	 */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnAIPlayerStatePawnSet OnPawnSet;

	/**
	 * @brief Returns the pawn associated with this AI player state.
	 */
	APawn* GetPawn();

	/**
	 * @brief Returns the pawn associated with this AI player state.
	 */
	template <class T>
	T* GetPawn() const {
		return Cast<T>(PawnPrivate);
	}

	/**
	 * @brief Sets the pawn associated with this AI player state.
	 */
	void SetPawn(APawn* InPawn);

private:
	/**
	 * @brief The pawn that is associated with this AI player state.
	 */
	UPROPERTY(BlueprintReadOnly, Category=AIPlayerState, meta=(AllowPrivateAccess="true"))
	TObjectPtr<APawn> PawnPrivate;

	/**
	 * @brief Callback to handle pawn destruction.
	 */
	UFUNCTION()
	void OnPawnPrivateDestroyed(AActor* InActor);

	/**
	 * @brief Sets the pawn associated with this AI player state.
	 */
	void SetPawnPrivate(APawn* InPawn);
};
