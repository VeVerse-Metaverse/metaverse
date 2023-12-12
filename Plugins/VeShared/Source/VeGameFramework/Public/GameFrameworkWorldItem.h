// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.


#pragma once

#include "ApiSpaceMetadata.h"
#include "Api2ServerMetadata.h"
#include "DownloadChunkSubsystem.h"
#include "GameFrameworkWorldTypes.h"
#include "HttpDownloadTypes.h"

/**
 * 
 */
class VEGAMEFRAMEWORK_API FGameFrameworkWorldItem : public TSharedFromThis<FGameFrameworkWorldItem> {

	DECLARE_EVENT_OneParam(UWorldController, UVeSpaceEvent, const FGameFrameworkWorldItem& InSpace);

public:
	explicit FGameFrameworkWorldItem(FGuid InId)
		: Id(InId) { }

	const FGuid& GetId() const { return Id; }

	const FApiSpaceMetadata& GetMetadata() const { return Metadata; }
	void SetMetadata(const FApiSpaceMetadata& InMetadata);
	void RefreshMetadata(TSharedRef<FOnGenericRequestCompleted> InCallback);

	EVeWorldState GetState() const { return State; }
	float GetStateRatio() const { return StateRatio; }

public:
	/** State == EVeSpaceState::PendingDownloadPak */
	void DownloadPackage(bool bPakAutoMount, TSharedRef<FOnPackageState> InCallback);

	/** State == EVeSpaceState::PackageDownloaded */
	void MountPackage(TSharedRef<FOnPackageState> InCallback);

	/** State == EVeSpaceState::PackageMounted */
	void MatchingServer(TSharedRef<FOnMatchingServerState> InCallback);

	/** State == EVeSpaceState::ServerOnline (Online mode) */
	void TeleportOnline(TSharedRef<FOnOpenWorldState> InCallback, class APlayerController* PlayerController, const FGuid& DestinationId = FGuid());

	/** State >= EVeSpaceState::PackageMounted (Offline mode) */
	void TeleportOffline(TSharedRef<FOnOpenWorldState> InCallback, APlayerController* PlayerController, const FGuid& DestinationId = FGuid());

	bool CanStartTeleport() const;
	void StartTeleportOnline(TSharedRef<FOnOpenWorldState> InCallback, APlayerController* PlayerController, const FGuid& DestinationId = FGuid());
	void StartTeleportOffline(TSharedRef<FOnOpenWorldState> InCallback, APlayerController* PlayerController, const FGuid& DestinationId = FGuid());
	
	UVeSpaceEvent& GetOnStateChanged() { return OnStateChanged; }

	int64 GetPackageSize() const;

protected:
	int32 DownloadingPackageMaxAttempts = 2;
	float DownloadingPackageAttemptsTime = 3.0f;

	int32 MatchingServerMaxAttempts = 20;
	float MatchingServerAttemptsTime = 3.0f;
	float MatchingServerExpireTime = 1.0f * 60.0f;

	void SetState(EVeWorldState InState);
	void SetState(EVeWorldState InState, float InRatio);

	EVePackageState GetPackageState() const;

	FTimerHandle TimerHandle_DownloadingPackage;
	FTimerHandle TimerHandle_MatchingServer;

	void DownloadFile(TSharedRef<FVeDownloadPackageList> DownloadPackageList, bool bPakAutoMount, TSharedRef<FOnPackageState> InCallback);
	void RequestMatchingServer(const FGuid& WorldId, TSharedRef<FOnMatchingServerState> InCallback);
	FApiFileMetadata* GetPakFileMetadata() const;

	static void ResetPlayerControllerInputMode(APlayerController* PlayerController);
	static bool IsPackageMounted(const FString& Map);
	static bool MountPackageFile(const FString& Filename);
	static void RegisterPackageMountPoint(const FString& InModName);
	static void LoadPackageAssetRegistry(const FString& ModName);
	static FString GetPackageFilename(const FString& Url);

private:
	FGuid Id;
	FApiSpaceMetadata Metadata;
	FApiServerMetadata ServerMetadata;

	int32 DownloadingPackageAttempts = 0;
	int32 MatchingServerAttempts = 0;

	FDownloadSubscriptionPtr BoundDownloadPackage;

	EVeWorldState State = EVeWorldState::None;
	float StateRatio = 1.0f;

	UVeSpaceEvent OnStateChanged;
};
