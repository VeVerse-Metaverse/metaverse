// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.



#pragma once

#include "CoreMinimal.h"
#include "GameFramework/WorldSettings.h"
#include "VeWorldSettings.generated.h"

class FGameFrameworkWorldItem;


UCLASS()
class VEGAMEFRAMEWORK_API AVeWorldSettings : public AWorldSettings {
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AVeWorldSettings();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void NotifyBeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
#if WITH_EDITORONLY_DATA
	/** The world id to use in the editor. Will be used to fake the world id in the editor. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VeGameFramework")
	FGuid EditorWorldId;
#endif

public:
	const FGuid& GetWorldId() const;
	const FGuid& GetServerId() const;
	FGameFrameworkWorldItem* GetWorldItem() const;

	DECLARE_DELEGATE_OneParam(FWorldIdDelegate, const FGuid& InWorldId);
	void OnReady_WorldId(const FWorldIdDelegate& Callback);

	DECLARE_DELEGATE_OneParam(FServerIdDelegate, const FGuid& InServerId);
	void OnReady_ServerId(const FServerIdDelegate& Callback);

	DECLARE_DELEGATE_OneParam(FWorldItemDelegate, TSharedPtr<class FGameFrameworkWorldItem> InWorldItem);
	void OnReady_WorldItem(const FWorldItemDelegate& Callback);

	DECLARE_DELEGATE(FInitializationDelegate);
	void OnReady_ClientInitializationComplete(const FInitializationDelegate& Callback);
	void OnReady_ServerInitializationComplete(const FInitializationDelegate& Callback);

	bool IsClientInitializationComplete() const { return bClientInitializationComplete; }
	bool IsServerInitializationComplete() const { return bServerInitializationComplete; }

protected:
	virtual void NativeOnWorldId(bool IsServer);
	virtual void NativeOnServerId(bool IsServer);
	virtual void NativeOnAuthorization(bool IsServer);
	virtual void NativeOnWorldItem(bool IsServer);

	void CheckInitializationComplete();
	virtual void CheckClientInitializationComplete();
	virtual void CheckServerInitializationComplete();
	virtual void NativeOnClientInitializationComplete();
	virtual void NativeOnServerInitializationComplete();

	virtual void NativeOnInitializationSuccessful();
	virtual void NativeOnInitializationFailure();

	virtual bool IsInitComplete() const;

private:
	FDelegateHandle AuthSubsystem_OnChangeAuthorizationStateHandle;
	FDelegateHandle World_GameStateSetHandle;

	TArray<FWorldIdDelegate> WorldIdCallbacks;
	TArray<FServerIdDelegate> ServerIdCallbacks;
	TArray<FWorldItemDelegate> WorldItemCallbacks;
	TArray<FInitializationDelegate> ClientInitCallbacks;
	TArray<FInitializationDelegate> ServerInitCallbacks;

	void SetWorldId(const FGuid& InWorldId);
	void SetServerId(const FGuid& InServerId);

	UPROPERTY(ReplicatedUsing="OnRep_WorldId")
	FGuid WorldId;

	UPROPERTY(ReplicatedUsing="OnRep_ServerId")
	FGuid ServerId;

	TSharedPtr<FGameFrameworkWorldItem> WorldItem;

	UPROPERTY(ReplicatedUsing="OnRep_ServerInitializationComplete")
	bool bServerInitializationComplete = false;

	bool bClientInitializationComplete = false;

	bool bBegunPlay = false;
	bool bGameStateComplete = false;
	bool bClientGameStateInitializationStage = false;
	bool bServerGameStateInitializationStage = false;

	UFUNCTION()
	void OnRep_WorldId();

	UFUNCTION()
	void OnRep_ServerId();

	UFUNCTION()
	void OnRep_ServerInitializationComplete();

};
