// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "VeGameStateBase.h"
#include "VeGameState.generated.h"

/**
 * Replicated game state
 */
UCLASS(BlueprintType, Blueprintable)
class VEGAME_API AVeGameState final : public AVeGameStateBase {
	GENERATED_BODY()

public:
	AVeGameState();

	/** Called before BeginPlay. */
	virtual void  BeginInitialization() override;

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UVePakExtraFilesGameStateComponent* VePakExtraFileGameStateComponent;

public:
	bool IsClientPakFilesInitializationComplete() const { return bPakFilesInitializationComplete; }
	void ClientPakFilesInitializationComplete();

	bool IsServerPlaceableInitializationComplete() const { return bPlaceableInitializationComplete; }
	void ServerPlaceableInitializationComplete();

protected:
	virtual bool NativeCheckClientInitializationComplete() override;
	virtual bool NativeCheckServerInitializationComplete() override;

private:
	bool bPakFilesInitializationComplete = false;
	bool bPlaceableInitializationComplete = false;

};
