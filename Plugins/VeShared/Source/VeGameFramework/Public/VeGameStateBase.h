// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


#pragma once

#include "VeWorldMetadata.h"
#include "VeServerMetadata.h"
#include "GameFramework/GameStateBase.h"
#include "VeGameStateBase.generated.h"

class FGameFrameworkWorldItem;

/**
 * Replicated game state
 */
UCLASS(BlueprintType, Blueprintable)
class VEGAMEFRAMEWORK_API AVeGameStateBase : public AGameStateBase {
	GENERATED_BODY()

public:
	AVeGameStateBase();

	/** Called before BeginPlay. */
	virtual void  BeginInitialization();

#pragma region InitializationComplete

public:
	UFUNCTION(BlueprintCallable, Category="Metaverse")
	void ClientInitializationComplete();

	UFUNCTION(BlueprintCallable, Category="Metaverse")
	void ServerInitializationComplete();

	DECLARE_DELEGATE(FGameStateDynamicDelegate);
	void OnReady_ClientInitialization(const FGameStateDynamicDelegate& Callback);
	void OnReady_ServerInitialization(const FGameStateDynamicDelegate& Callback);

protected:
	UFUNCTION(BlueprintImplementableEvent, Category="Metaverse")
	void BeginClientInitialization();
	
	UFUNCTION(BlueprintImplementableEvent, Category="Metaverse")
	void BeginServerInitialization();
	
	virtual bool NativeCheckClientInitializationComplete();
	virtual bool NativeCheckServerInitializationComplete();

	UFUNCTION(BlueprintNativeEvent, Category="Metaverse")
	bool CheckClientInitializationComplete();

	UFUNCTION(BlueprintNativeEvent, Category="Metaverse")
	bool CheckServerInitializationComplete();

	virtual void NativeOnClientInitializationComplete();
	virtual void NativeOnServerInitializationComplete();

private:
	bool bClientInitializationComplete = false;
	bool bServerInitializationComplete = false;

	TArray<FGameStateDynamicDelegate> ClientInitializationCallbacks;
	TArray<FGameStateDynamicDelegate> ServerInitializationCallbacks;
	
#pragma endregion InitializationComplete

};
