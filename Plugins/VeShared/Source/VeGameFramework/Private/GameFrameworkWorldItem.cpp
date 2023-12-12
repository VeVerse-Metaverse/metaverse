// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "GameFrameworkWorldItem.h"

#include "VeShared.h"
#include "VeApi.h"
#include "VeApi2.h"
#include "Api2AuthSubsystem.h"
#include "Api2WorldSubsystem.h"
#include "Api2ServerSubsystem.h"
#include "VeGameFramework.h"
#include "VeDownload.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "PakPakSubsystem.h"
#include "VeWorldSubsystem.h"
#include "FileStorageSubsystem.h"
#include "FileStorageTypes.h"
#include "PakLoader.h"
#include "VeMonitoring/VeMonitoringModule.h"


void FGameFrameworkWorldItem::SetMetadata(const FApiSpaceMetadata& InMetadata) {
	if (Id != InMetadata.Id) {
		VeLogErrorFunc("Invalid metadata id");
		return;
	}

	Metadata = InMetadata;

	const auto PackageState = GetPackageState();
	switch (PackageState) {
	case EVePackageState::Error:
		SetState(EVeWorldState::Error);
		break;

	case EVePackageState::Error_Unavailable:
		SetState(EVeWorldState::Error_Unavailable);
		break;

	case EVePackageState::Error_DownloadFail:
		SetState(EVeWorldState::Error_DownloadFail);
		break;

	case EVePackageState::Error_MountFail:
		SetState(EVeWorldState::Error_MountFail);
		break;

	case EVePackageState::NotDownloaded:
		SetState(EVeWorldState::PendingDownloadPackage);
		break;

	case EVePackageState::Downloaded:
		SetState(EVeWorldState::PackageDownloaded);
		break;

	case EVePackageState::Mounted:
		SetState(EVeWorldState::PackageMounted);
		break;

	default:
		SetState(EVeWorldState::None);
		break;
	}
}

void FGameFrameworkWorldItem::RefreshMetadata(TSharedRef<FOnGenericRequestCompleted> InCallback) {
	if (!Id.IsValid()) {
		InCallback->ExecuteIfBound(true, FString());
		return;
	}
	
	GET_MODULE_SUBSYSTEM(WorldSubsystem, Api2, World);
	if (!WorldSubsystem) {
		InCallback->ExecuteIfBound(false, TEXT(""));
		return;
	}

	const auto Callback = MakeShared<FOnWorldRequestCompleted2>();
	Callback->BindLambda([=](const FApiSpaceMetadata& InMetadata, const EApi2ResponseCode InResponseCode, const FString& InError) {
		const bool bSuccessful = InResponseCode == EApi2ResponseCode::Ok;
		if (!bSuccessful) {
			VeLogErrorFunc("Failed to request: %s", *InError);

			InCallback->ExecuteIfBound(false, InError);
			return;
		}

		SetMetadata(InMetadata);

		InCallback->ExecuteIfBound(bSuccessful, InError);
	});

	WorldSubsystem->Get(Id, Callback);
}

void FGameFrameworkWorldItem::SetState(EVeWorldState InState) {
	// todo: teleporting status must be reset
	if (InState == EVeWorldState::Teleporting) {
		return;
	}

	if (State != InState && InState != EVeWorldState::Teleporting) {
		State = InState;
		OnStateChanged.Broadcast(*this);
	}
}

void FGameFrameworkWorldItem::SetState(EVeWorldState InState, float InRatio) {
	// todo: teleporting status must be reset
	if (InState == EVeWorldState::Teleporting) {
		return;
	}

	if (State != InState || StateRatio != InRatio) {
		State = InState;
		StateRatio = InRatio;
		OnStateChanged.Broadcast(*this);
	}
}

void FGameFrameworkWorldItem::TeleportOnline(TSharedRef<FOnOpenWorldState> InCallback, APlayerController* PlayerController, const FGuid& DestinationId) {
	if (State != EVeWorldState::ServerOnline && State != EVeWorldState::Teleporting) {
		VeLogWarningFunc("State not valid");
		InCallback->ExecuteIfBound(false);
		return;
	}

	if (!Metadata.Id.IsValid()) {
		VeLogErrorFunc("Failed to travel to the world: invalid world id");
		InCallback->ExecuteIfBound(false);
		return;
	}

	if (!IsValid(PlayerController)) {
		VeLogErrorFunc("Invalid PlayerController");
		InCallback->ExecuteIfBound(false);
		return;
	}

	GET_MODULE_SUBSYSTEM(AuthSubsystem, Api2, Auth);
	if (!AuthSubsystem) {
		InCallback->ExecuteIfBound(false);
		return;
	}

	// Reset player controller input mode
	ResetPlayerControllerInputMode(PlayerController);

	const auto UserId = AuthSubsystem->GetUserId();
	const auto Host = ServerMetadata.Host;
	const auto Port = ServerMetadata.Port;

	// Reset the input mode to game only so player can control the character.
	FInputModeGameOnly InputMode;
	PlayerController->SetInputMode(InputMode);
	PlayerController->SetShowMouseCursor(false);
	PlayerController->ResetIgnoreMoveInput();

	const FString Command = FString::Printf(TEXT("open %s:%d?UserId=%s?MetaverseKey=%s#%s"),
		*Host,
		Port,
		*UserId.ToString(EGuidFormats::DigitsWithHyphensLower),
		TEXT("6e500749-0833-4b02-b3f4-cebc7e6b227b"),
		*DestinationId.ToString(EGuidFormats::DigitsWithHyphensLower)
		);

	VeLog("Teleport: %s", *Command);
	PlayerController->ConsoleCommand(Command);

	SetState(EVeWorldState::Teleporting);
	InCallback->ExecuteIfBound(true);
}

void FGameFrameworkWorldItem::TeleportOffline(TSharedRef<FOnOpenWorldState> InCallback, APlayerController* PlayerController, const FGuid& DestinationId) {
	if (State < EVeWorldState::PackageMounted || State == EVeWorldState::Teleporting) {
		VeLogWarningFunc("State not valid");
		InCallback->ExecuteIfBound(false);
		return;
	}

	if (!Metadata.Id.IsValid()) {
		VeLogErrorFunc("Failed to travel to the world: invalid world id");
		InCallback->ExecuteIfBound(false);
		return;
	}

	if (!IsValid(PlayerController)) {
		VeLogErrorFunc("Invalid PlayerController");
		InCallback->ExecuteIfBound(false);
		return;
	}

	// Reset player controller input mode
	ResetPlayerControllerInputMode(PlayerController);

	const FString Command = FString::Printf(TEXT("open %s?WorldId=%s#%s"),
		*Metadata.Map,
		*Metadata.Id.ToString(EGuidFormats::DigitsWithHyphensLower),
		*DestinationId.ToString(EGuidFormats::DigitsWithHyphensLower)
		);

	// Reset the input mode to game only so player can control the character.
	FInputModeGameOnly InputMode;
	PlayerController->SetInputMode(InputMode);
	PlayerController->SetShowMouseCursor(false);
	PlayerController->ResetIgnoreMoveInput();

	VeLog("World Open: %s", *Command);
	PlayerController->ConsoleCommand(Command);

	SetState(EVeWorldState::Teleporting);
	InCallback->ExecuteIfBound(true);
}

bool FGameFrameworkWorldItem::CanStartTeleport() const {
#if WITH_EDITORONLY_DATA
	return Metadata.Id.IsValid() && (
		State == EVeWorldState::PackageMounted ||
		State == EVeWorldState::ServerOnline
	);
#else
	return Metadata.Id.IsValid() && (
		State == EVeWorldState::PendingDownloadPackage ||
		State == EVeWorldState::PackageMounted ||
		State == EVeWorldState::ServerOnline
	);
#endif
}

void FGameFrameworkWorldItem::StartTeleportOnline(TSharedRef<FOnOpenWorldState> InCallback, APlayerController* PlayerController, const FGuid& DestinationId) {
	if (!CanStartTeleport()) {
		VeLogWarningFunc("State not valid");
		InCallback->ExecuteIfBound(false);
		return;
	}

	const auto MatchingServerCallback = MakeShared<FOnMatchingServerState>();
	MatchingServerCallback->BindLambda([=](EVeMatchingServerState InState) {
		if (InState == EVeMatchingServerState::Online) {
			TeleportOnline(InCallback, PlayerController, DestinationId);
		} else {
			InCallback->ExecuteIfBound(false);
		}
	});

	const auto DownloadPackageCallback = MakeShared<FOnPackageState>();
	DownloadPackageCallback->BindLambda([=](EVePackageState InState) {
		if (InState == EVePackageState::Mounted) {
			MatchingServer(MatchingServerCallback);
		} else {
			InCallback->ExecuteIfBound(false);
		}
	});

	switch (State) {
	case EVeWorldState::PendingDownloadPackage:
		DownloadPackage(true, DownloadPackageCallback);
		break;
	case EVeWorldState::PackageMounted:
		MatchingServer(MatchingServerCallback);
		break;
	default:
		TeleportOnline(InCallback, PlayerController, DestinationId);
		break;
	}
}

void FGameFrameworkWorldItem::StartTeleportOffline(TSharedRef<FOnOpenWorldState> InCallback, APlayerController* PlayerController, const FGuid& DestinationId) {
	if (!CanStartTeleport()) {
		VeLogWarningFunc("State not valid");
		InCallback->ExecuteIfBound(false);
		return;
	}

	const auto DownloadPackageCallback = MakeShared<FOnPackageState>();
	DownloadPackageCallback->BindLambda([=](EVePackageState InState) {
		if (InState == EVePackageState::Mounted) {
			TeleportOffline(InCallback, PlayerController, DestinationId);
		} else {
			InCallback->ExecuteIfBound(false);
		}
	});

	switch (State) {
	case EVeWorldState::PendingDownloadPackage:
		DownloadPackage(true, DownloadPackageCallback);
		break;
	default:
		TeleportOffline(InCallback, PlayerController, DestinationId);
		break;
	}
}

int64 FGameFrameworkWorldItem::GetPackageSize() const {
	const auto* PakFileMetadata = GetPakFileMetadata();
	if (PakFileMetadata) {
		return PakFileMetadata->Size;
	}
	return 0;
}

EVePackageState FGameFrameworkWorldItem::GetPackageState() const {
	// If package is already exists
	bool IsMounted = IsPackageMounted(Metadata.Map);
	if (IsMounted) {
		return EVePackageState::Mounted;
	}

#if WITH_EDITORONLY_DATA

	return EVePackageState::Error_Unavailable;

#else

	if (!Metadata.Package.IsValid()) {
		VeLogWarningFunc("Package metadata fail");
		return EVePackageState::Error_Unavailable;
	}

	const FString PackageName = Metadata.Package->Name;

	const auto* PakFileMetadata = GetPakFileMetadata();
	if (!PakFileMetadata) {
		VeLogErrorFunc("Package file not exists: %s", *PackageName);
		return EVePackageState::Error_Unavailable;
	}

	if (!PakFileMetadata->Id.IsValid()) {
		VeLogErrorFunc("Invalid package id: %s", *PackageName);
		return EVePackageState::Error_Unavailable;
	}

	const FString PakFilePath = GetPackageFilename(PakFileMetadata->Url);
	if (PakFilePath.IsEmpty()) {
		return EVePackageState::NotDownloaded;
	}

	if (PakFileMetadata->Url.IsEmpty()) {
		VeLogErrorFunc("No package url: %s (%s)", *PackageName, *PakFileMetadata->Id.ToString(EGuidFormats::DigitsWithHyphensLower));
		return EVePackageState::Error_Unavailable;
	}

	return EVePackageState::Downloaded;

#endif
}

// AVePortal::DownloadMetaversePak
void FGameFrameworkWorldItem::DownloadPackage(bool bPakAutoMount, TSharedRef<FOnPackageState> InCallback) {
	const auto PreviousState = EVeWorldState::PendingDownloadPackage;
	if (State != PreviousState) {
		VeLogWarningFunc("State not valid");
		InCallback->ExecuteIfBound(EVePackageState::None);
		return;
	}

	if (DownloadingPackageMaxAttempts < 1) {
		VeLogWarningFunc("DownloadingPackageMaxAttempts not valid");
		InCallback->ExecuteIfBound(EVePackageState::None);
		return;
	}

	const auto* PakFileMetadata = GetPakFileMetadata();
	if (!PakFileMetadata) {
		SetState(EVeWorldState::Error_Unavailable);
		VeLogErrorFunc("Package file not exists: {%s} %s", *PakFileMetadata->Id.ToString(EGuidFormats::DigitsWithHyphensLower), *Metadata.Package->Name);
		InCallback->ExecuteIfBound(EVePackageState::Error_Unavailable);
		return;
	}

	if (!PakFileMetadata->Id.IsValid()) {
		VeLogErrorFunc("Invalid package id: {%s} %s", *PakFileMetadata->Id.ToString(EGuidFormats::DigitsWithHyphensLower), *Metadata.Package->Name);
		SetState(EVeWorldState::Error_Unavailable);
		InCallback->ExecuteIfBound(EVePackageState::Error_Unavailable);
		return;
	}

	if (PakFileMetadata->Url.IsEmpty()) {
		VeLogErrorFunc("No package url: {%s} %s)", *PakFileMetadata->Id.ToString(EGuidFormats::DigitsWithHyphensLower), *Metadata.Package->Name);
		SetState(EVeWorldState::Error_Unavailable);
		InCallback->ExecuteIfBound(EVePackageState::Error_Unavailable);
		return;
	}


	const TSharedRef<FVeDownloadPackageList> DownloadPackageList = MakeShared<FVeDownloadPackageList>();
	DownloadPackageList->Attempts = DownloadingPackageMaxAttempts - 1;

	for (auto& ApiFileMetadata : Metadata.Package->Files) {
		if (ApiFileMetadata.Type == EApiFileType::PakExtraContent) {
			FVeDownloadPackageFile DownloadFile;
			DownloadFile.Id = ApiFileMetadata.Id;
			DownloadFile.Url = ApiFileMetadata.Url;
			DownloadFile.Size = ApiFileMetadata.Size;
			DownloadPackageList->ExtraContentFiles.Emplace(MoveTemp(DownloadFile));
			DownloadPackageList->TotalSize += DownloadFile.Size;
		} else if (ApiFileMetadata.Type == EApiFileType::Pak) {
			FVeDownloadPackageFile DownloadFile;
			DownloadFile.Id = ApiFileMetadata.Id;
			DownloadFile.Url = ApiFileMetadata.Url;
			DownloadFile.Size = ApiFileMetadata.Size;
			DownloadPackageList->PakFiles.Emplace(MoveTemp(DownloadFile));
			DownloadPackageList->TotalSize += DownloadFile.Size;
		}
	}


	const auto RequestCallback = MakeShared<FOnPackageState>();
	RequestCallback->BindLambda([=](EVePackageState RequestState) {
		switch (RequestState) {
		case EVePackageState::Error_DownloadFail: {
			if (DownloadPackageList->Attempts) {
				--DownloadPackageList->Attempts;

				// Repeat
				
				FTimerDelegate TimerDelegate;
				TimerDelegate.BindLambda([=]() {
					DownloadFile(DownloadPackageList, bPakAutoMount, RequestCallback);
				});

				FVeSharedModule::GetTimerManager().ClearTimer(TimerHandle_DownloadingPackage);
				FVeSharedModule::GetTimerManager().SetTimer(TimerHandle_DownloadingPackage, TimerDelegate, DownloadingPackageAttemptsTime, false);
				break;
			}

			SetState(PreviousState);
			InCallback->ExecuteIfBound(RequestState);
			break;
		}

		case EVePackageState::Downloaded: {
			// Download next file
			if (DownloadPackageList->ExtraContentFiles.Num() || DownloadPackageList->PakFiles.Num()) {
				const float Progress = static_cast<float>(DownloadPackageList->DownloadedSize) / static_cast<float>(DownloadPackageList->TotalSize);
				SetState(EVeWorldState::PackageDownloading, Progress);
				DownloadFile(DownloadPackageList, bPakAutoMount, RequestCallback);
				break;
			}

			// Download is complete
			SetState(EVeWorldState::PackageDownloaded);
			InCallback->ExecuteIfBound(RequestState);
			break;
		}

		case EVePackageState::Error: {
			SetState(EVeWorldState::Error);
			InCallback->ExecuteIfBound(RequestState);
			break;
		}

		default: {
			InCallback->ExecuteIfBound(RequestState);
			break;
		}
		}
	});

	SetState(EVeWorldState::PackageDownloading, 0.f);
	DownloadFile(DownloadPackageList, bPakAutoMount, RequestCallback);
}

void FGameFrameworkWorldItem::DownloadFile(TSharedRef<FVeDownloadPackageList> DownloadPackageList, bool bPakAutoMount, TSharedRef<FOnPackageState> InCallback) {
	GET_MODULE_SUBSYSTEM(FileStorageSubsystem, Download, FileStorage);
	if (!FileStorageSubsystem) {
		InCallback->ExecuteIfBound(EVePackageState::Error);
		return;
	}

	bool IsPakFile = false;
	FVeDownloadPackageFile DownloadFile;

	if (DownloadPackageList->ExtraContentFiles.Num()) {
		DownloadFile = DownloadPackageList->ExtraContentFiles.Last();
	} else {
		DownloadFile = DownloadPackageList->PakFiles.Last();
		IsPakFile = true;
	}

	const FStorageItemId StorageItemId(DownloadFile.Url);
	constexpr FBaseStorageItemOptions StorageItemOptions;

	TSharedPtr<FBaseStorageItem> StorageItem;
	if (IsPakFile) {
		StorageItem = FileStorageSubsystem->GetPakStorageItem(StorageItemId);
	} else {
		StorageItem = FileStorageSubsystem->GetFileStorageItem(StorageItemId);
	}

	StorageItem->GetOnProgress().AddLambda([=](FBaseStorageItem* InStorageItem, const FFileStorageProgressMetadata& InMetadata) {
		const int64 DownloadedSize = DownloadPackageList->DownloadedSize + InMetadata.DownloadedSize;
		const float Progress = static_cast<float>(DownloadedSize) / static_cast<float>(DownloadPackageList->TotalSize);
		SetState(EVeWorldState::PackageDownloading, Progress);
	});

	StorageItem->GetOnComplete().AddLambda([=](FBaseStorageItem* InStorageItem, const FFileStorageResultMetadata& InMetadata) {
		if (!InMetadata.bSuccessful) {
			VeLogWarningFunc("Failed to download package file: {%s}", *DownloadFile.Id.ToString(EGuidFormats::DigitsWithHyphensLower));
			InCallback->ExecuteIfBound(EVePackageState::Error_DownloadFail);
			return;
		}

		DownloadPackageList->DownloadedSize += InMetadata.ContentLength;

		if (DownloadPackageList->ExtraContentFiles.Num()) {
			DownloadPackageList->ExtraContentFiles.Pop();
		} else {
			DownloadPackageList->PakFiles.Pop();
		}

		// Automount package
		if (IsPakFile && bPakAutoMount) {
			State = EVeWorldState::PackageDownloaded;
			MountPackage(InCallback);
		} else {
			InCallback->ExecuteIfBound(EVePackageState::Downloaded);
		}
	});

	StorageItem->Process(StorageItemOptions);
}

// AVePortal::MountPak
void FGameFrameworkWorldItem::MountPackage(TSharedRef<FOnPackageState> InCallback) {
	const auto PreviousState = EVeWorldState::PackageDownloaded;
	if (State != PreviousState) {
		VeLogWarningFunc("State not valid");
		InCallback->ExecuteIfBound(EVePackageState::None);
		return;
	}

	// If pak file is already mounted
	if (IsPackageMounted(Metadata.Map)) {
		SetState(EVeWorldState::PackageMounted);
		InCallback->ExecuteIfBound(EVePackageState::Mounted);
		return;
	}

	const FString PackageName = Metadata.Package->Name;

	const auto* PakFileMetadata = GetPakFileMetadata();
	if (!PakFileMetadata) {
		VeLogErrorFunc("Package file not exists: %s", *PackageName);
		SetState(EVeWorldState::Error_Unavailable);
		InCallback->ExecuteIfBound(EVePackageState::Error_Unavailable);
		return;
	}

	if (!PakFileMetadata->Id.IsValid()) {
		VeLogErrorFunc("Invalid package id: %s", *PackageName);
		SetState(EVeWorldState::Error_Unavailable);
		InCallback->ExecuteIfBound(EVePackageState::Error_Unavailable);
		return;
	}

	const FString PakFilePath = GetPackageFilename(PakFileMetadata->Url);
	if (PakFilePath.IsEmpty()) {
		VeLogWarningFunc("Package file not exists: %s", *PakFilePath);
		SetState(EVeWorldState::PendingDownloadPackage);
		InCallback->ExecuteIfBound(EVePackageState::NotDownloaded);
		return;
	}

	// Pak file is not mounted, but exists on the disk, mount it.
	if (MountPackageFile(PakFilePath)) {
		RegisterPackageMountPoint(PackageName);
		LoadPackageAssetRegistry(PackageName);

		if (IsPackageMounted(Metadata.Map)) {
			SetState(EVeWorldState::PackageMounted);
			InCallback->ExecuteIfBound(EVePackageState::Mounted);
			return;
		}
	}

	VeLogErrorFunc("Failed to mount package file: %s", *PakFilePath);
	SetState(EVeWorldState::Error_MountFail);
	InCallback->ExecuteIfBound(EVePackageState::Error_MountFail);
}

// AVePortal::RequestMatchingServer
void FGameFrameworkWorldItem::MatchingServer(TSharedRef<FOnMatchingServerState> InCallback) {
	const auto PreviousState = EVeWorldState::PackageMounted;
	if (State != PreviousState) {
		VeLogWarningFunc("State not valid");
		InCallback->ExecuteIfBound(EVeMatchingServerState::None);
		return;
	}

	const FGuid& WorldId = Metadata.Id;

	if (!WorldId.IsValid()) {
		VeLogErrorFunc("Invalid world id");
		InCallback->ExecuteIfBound(EVeMatchingServerState::Error);
		return;
	}

	if (MatchingServerMaxAttempts < 1) {
		VeLogWarningFunc("MatchingServerMaxAttempts not valid");
		InCallback->ExecuteIfBound(EVeMatchingServerState::None);
		return;
	}

	SetState(EVeWorldState::ServerMatching, 0.0f);
	ServerMetadata = {};

	auto RequestCallback = MakeShared<FOnMatchingServerState>();
	RequestCallback->BindLambda([=](const EVeMatchingServerState& RequestState) {
		switch (RequestState) {
		case EVeMatchingServerState::WaitServer: {
			if (MatchingServerAttempts) {
				const float Ratio = static_cast<float>(MatchingServerMaxAttempts - MatchingServerAttempts) / static_cast<float>(MatchingServerMaxAttempts);
				--MatchingServerAttempts;

				// Repeat
				
				SetState(EVeWorldState::ServerMatching, Ratio);

				FTimerDelegate TimerDelegate;
				TimerDelegate.BindLambda([=]() {
					RequestMatchingServer(WorldId, RequestCallback);
				});

				FVeSharedModule::GetTimerManager().ClearTimer(TimerHandle_MatchingServer);
				FVeSharedModule::GetTimerManager().SetTimer(TimerHandle_MatchingServer, TimerDelegate, MatchingServerAttemptsTime, false);
				break;
			}

			SetState(PreviousState);
			InCallback->ExecuteIfBound(EVeMatchingServerState::Offline);
			break;
		}

		case EVeMatchingServerState::Online: {
			SetState(EVeWorldState::ServerOnline);
			InCallback->ExecuteIfBound(RequestState);

			// Set offline after expiration
			FTimerDelegate TimerDelegate;
			TimerDelegate.BindLambda([=]() {
				SetState(PreviousState);
			});

			FVeSharedModule::GetTimerManager().ClearTimer(TimerHandle_MatchingServer);
			FVeSharedModule::GetTimerManager().SetTimer(TimerHandle_MatchingServer, TimerDelegate, MatchingServerExpireTime, false);
			break;
		}

		case EVeMatchingServerState::Error: {
			SetState(EVeWorldState::Error);
			InCallback->ExecuteIfBound(RequestState);
		}

		default: {
			SetState(PreviousState);
			InCallback->ExecuteIfBound(RequestState);
			break;
		}
		}
	});

	MatchingServerAttempts = MatchingServerMaxAttempts - 1;
	RequestMatchingServer(WorldId, RequestCallback);
}

void FGameFrameworkWorldItem::RequestMatchingServer(const FGuid& WorldId, TSharedRef<FOnMatchingServerState> InCallback) {
#if 0 // APIv2
	GET_MODULE_SUBSYSTEM(ServerSubsystem, Api2, Server);
	if (!ServerSubsystem) {
		InCallback->ExecuteIfBound(EVeMatchingServerState::Error);
		return;
	}

	const auto OnCompleted = MakeShared<FOnServerRequestCompleted1>();
	OnCompleted->BindLambda([this, InCallback](const FApiServerMetadata& InMetadata, EApi2ResponseCode InResponseCode, const FString& InError) {
		const bool bSuccessful = InResponseCode == EApi2ResponseCode::Ok;
		if (bSuccessful) {
			ServerMetadata = InMetadata;

			switch (ServerMetadata.Status) {
			case EApi2ServerStatus::Online:
				InCallback->ExecuteIfBound(EVeMatchingServerState::Online);
				break;
			case EApi2ServerStatus::Created:
			case EApi2ServerStatus::Launching:
			case EApi2ServerStatus::Downloading:
			case EApi2ServerStatus::Starting:
				InCallback->ExecuteIfBound(EVeMatchingServerState::WaitServer);
				break;
			default:
				InCallback->ExecuteIfBound(EVeMatchingServerState::Offline);
			}
		} else {
			VeLogErrorFunc("Failed to request a matching server: %s", *InError);
			InCallback->ExecuteIfBound(EVeMatchingServerState::Error);
		}
	});

	FApi2RequestServerMatchMetadata RequestMetadata;
	RequestMetadata.WorldId = WorldId;

	ServerSubsystem->GetMatchingServer(RequestMetadata, OnCompleted);
#else // APIv1
	GET_MODULE_SUBSYSTEM(ServerSubsystem, Api, Server);
	if (!ServerSubsystem) {
		InCallback->ExecuteIfBound(EVeMatchingServerState::Error);
		return;
	}

	const auto OnCompleted = MakeShared<FOnServerRequestCompleted>();
	OnCompleted->BindLambda([this, InCallback](const FApiServerMetadata& InMetadata, const bool bSuccessful, const FString& InError) {
		if (bSuccessful) {
			ServerMetadata = InMetadata;

			switch (ServerMetadata.Status) {
			case EApiServerStatus::Online:
				InCallback->ExecuteIfBound(EVeMatchingServerState::Online);
				break;
			case EApiServerStatus::Created:
			case EApiServerStatus::Launching:
			case EApiServerStatus::Downloading:
			case EApiServerStatus::Starting:
				InCallback->ExecuteIfBound(EVeMatchingServerState::WaitServer);
				break;
			default:
				InCallback->ExecuteIfBound(EVeMatchingServerState::Offline);
			}
		} else {
			VeLogErrorFunc("Failed to request a matching server: %s", *InError);
			InCallback->ExecuteIfBound(EVeMatchingServerState::Error);
		}
	});

	ServerSubsystem->GetMatchingServer(WorldId, OnCompleted);
#endif
}

FApiFileMetadata* FGameFrameworkWorldItem::GetPakFileMetadata() const {
	return Metadata.Package->Files.FindByPredicate([=](const FApiFileMetadata& InFileMetadata) {
		return InFileMetadata.Type == EApiFileType::Pak;
	});
}

void FGameFrameworkWorldItem::ResetPlayerControllerInputMode(APlayerController* PlayerController) {
	FInputModeGameOnly InputMode;
	PlayerController->SetInputMode(InputMode);
	PlayerController->SetShowMouseCursor(false);
	PlayerController->ResetIgnoreMoveInput();
}

bool FGameFrameworkWorldItem::IsPackageMounted(const FString& Map) {
	// Look up in the AssetRegistry.
	IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")).Get();
	FName ExistingPackageName = AssetRegistry.GetFirstPackageByName(Map);
	return !ExistingPackageName.IsNone();

#if 0
	auto& Registry = FAssetRegistryModule::GetRegistry();

	TArray<FAssetData> OutAssetData;
	if (Registry.GetAssetsByPackageName(FName(Map), OutAssetData)) {
		return static_cast<bool>(OutAssetData.Num());
	}

	return false;

	// auto& Registry = FAssetRegistryModule::GetRegistry();
	//
	// // Editor mode
	//
	// const FString PackageName = Metadata.Package->Name;
	// auto PackagePath = FString("/") + PackageName;
	// if (Registry.PathExists(PackagePath) && Registry.HasAssets(FName(PackagePath), true)) {
	// 	// For the editor, we assume that the editor can have the required world, so proceed with server request 
	// 	return true;
	// }
	//
	// // Game mode
	//
	// bool bServer = false;
	// const FGuid PackageId = FPakPakSubsystem::GetMetaversePakFileId(*Metadata.Package, bServer);
	// const FString PackageFile = FPakPakSubsystem::GetMetaversePakPath(PackageId);
	//
	// if (FPakPakSubsystem::IsPakMounted(PackageFile)) {
	// 	// Pak file is mounted, request the matching server.
	// 	return false;
	// }
#endif
}

// FPakPakSubsystem::MountMetaversePakFile
bool FGameFrameworkWorldItem::MountPackageFile(const FString& Filename) {
	FPakLoader* PakLoaderPtr = FPakLoader::Get();
	if (!PakLoaderPtr) {
		GET_MODULE_SUBSYSTEM(MonitoringSubsystem, Monitoring, Monitoring);
		if (MonitoringSubsystem) {
			MonitoringSubsystem->ReportMessage(TEXT("Pak"), TEXT("error"), TEXT("failed to get a pak loader instance"));
		}
		return false;
	}

	if (PakLoaderPtr->MountPakFile(Filename, INDEX_NONE, TEXT(""))) {
		return true;
	}

	GET_MODULE_SUBSYSTEM(MonitoringSubsystem, Monitoring, Monitoring);
	if (MonitoringSubsystem) {
		MonitoringSubsystem->ReportMessage(TEXT("Pak"), TEXT("error"), TEXT("failed to mount a pak file"));
	}

	return false;
}

// FPakPakSubsystem::RegisterMetaverseMountPoint
void FGameFrameworkWorldItem::RegisterPackageMountPoint(const FString& InModName) {
	FPakLoader* PakLoaderPtr = FPakLoader::Get();
	if (!PakLoaderPtr) {
		GET_MODULE_SUBSYSTEM(MonitoringSubsystem, Monitoring, Monitoring);
		if (MonitoringSubsystem) {
			MonitoringSubsystem->ReportMessage(TEXT("Pak"), TEXT("error"), TEXT("failed to get a pak loader instance"));
		}
		return;
	}

	const FString RootPath = FString(TEXT("/")) + InModName + TEXT("/");
	const FString ContentPath = FString::Printf(TEXT("../../Plugins/%s/Content/"), *InModName);
	PakLoaderPtr->RegisterMountPoint(RootPath, ContentPath);
}

// FPakPakSubsystem::LoadPakAssetRegistry
void FGameFrameworkWorldItem::LoadPackageAssetRegistry(const FString& ModName) {
	FPakLoader* PakLoaderPtr = FPakLoader::Get();
	if (!PakLoaderPtr) {
		GET_MODULE_SUBSYSTEM(MonitoringSubsystem, Monitoring, Monitoring);
		if (MonitoringSubsystem) {
			MonitoringSubsystem->ReportMessage(TEXT("Pak"), TEXT("error"), TEXT("failed to get a pak loader instance"));
		}
		return;
	}

	const FString Path = FString::Printf(TEXT("../../Plugins/%s/AssetRegistry.bin"), *ModName);
	PakLoaderPtr->LoadAssetRegistryFile(Path);
	// if (!) {
	// 	if (FVeMonitoringModule* MonitoringModule = FVeMonitoringModule::Get()) {
	// 		if (const TSharedPtr<FVeMonitoringSubsystem> MonitoringSubsystem = MonitoringModule->GetMonitoringSubsystem()) {
	// 			MonitoringSubsystem->ReportMessage(TEXT("Pak"), TEXT("error"), FString::Printf(TEXT("failed to load an asset registry from a file: %s"), *Path));
	// 		} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Monitoring), STRINGIFY(Monitoring)); }
	// 	} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Monitoring)); }
	// }
}

FString FGameFrameworkWorldItem::GetPackageFilename(const FString& Url) {
	GET_MODULE_SUBSYSTEM(FileStorageSubsystem, Download, FileStorage);
	if (!FileStorageSubsystem) {
		return FString();
	}

	FStorageItemId StorageItemId(Url);
	auto StorageItem = FileStorageSubsystem->GetPakStorageItem(StorageItemId);

	if (StorageItem->IsCacheValid()) {
		return StorageItem->GetFileName();
	} else {
		return FString();
	}
}
