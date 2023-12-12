// Author: Egor A. Pristavka

#include "PakPakSubsystem.h"
#include "IPlatformFilePak.h"
#if WITH_PAK
#include "PakLoaderLibrary.h"
#include "PakLoader/Public/PakLoader.h"
#endif
#include "ApiFileMetadata.h"
#include "VeMonitoring/VeMonitoringModule.h"

const FName FPakPakSubsystem::Name = FName(TEXT("PakSubsystem"));

void FPakPakSubsystem::Initialize() {
#if WITH_PAK
	FPakLoader* PakLoaderPtr = FPakLoader::Get();
	if (!PakLoaderPtr) {
		if (FVeMonitoringModule* MonitoringModule = FVeMonitoringModule::Get()) {
			if (const TSharedPtr<FMonitoringMonitoringSubsystem> MonitoringSubsystem = MonitoringModule->GetMonitoringSubsystem()) {
				MonitoringSubsystem->ReportMessage(TEXT("Pak"), TEXT("error"), TEXT("failed to get a pak loader instance"));
			} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Monitoring), STRINGIFY(Monitoring)); }
		} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Monitoring)); }
	}
#endif
}

void FPakPakSubsystem::Shutdown() {}

FString FPakPakSubsystem::GetMetaversePakFileUrl(const FApiPackageMetadata& InModMetadata, const bool bServer) {
	const FApiFileMetadata* Result = InModMetadata.Files.FindByPredicate([=](const FApiFileMetadata& InFileMetadata) {
		if (bServer) {
			return InFileMetadata.Type == EApiFileType::Pak && InFileMetadata.Platform == FVeSharedModule::GetPlatformString() && InFileMetadata.DeploymentType == EApiFileDeploymentType::Server;
		}
		return InFileMetadata.Type == EApiFileType::Pak && InFileMetadata.Platform == FVeSharedModule::GetPlatformString() && InFileMetadata.DeploymentType == EApiFileDeploymentType::Client;
	});

	if (Result) {
		return Result->Url;
	}

	return FString();
}

FGuid FPakPakSubsystem::GetMetaversePakFileId(const FApiPackageMetadata& InModMetadata, const bool bServer) {
	const FApiFileMetadata* Result = InModMetadata.Files.FindByPredicate([=](const FApiFileMetadata& InFileMetadata) {
		if (bServer) {
			return InFileMetadata.Type == EApiFileType::Pak && InFileMetadata.Platform == FVeSharedModule::GetPlatformString() && InFileMetadata.DeploymentType == EApiFileDeploymentType::Server;
		}
		return InFileMetadata.Type == EApiFileType::Pak && InFileMetadata.Platform == FVeSharedModule::GetPlatformString() && InFileMetadata.DeploymentType == EApiFileDeploymentType::Client;
	});

	if (Result) {
		return Result->Id;
	}

	return FGuid();
}

int64 FPakPakSubsystem::GetMetaversePakFileSize(const FApiPackageMetadata& InModMetadata, const bool bServer) {
	const FApiFileMetadata* Result = InModMetadata.Files.FindByPredicate([=](const FApiFileMetadata& InFileMetadata) {
		const FString PlatformString = FVeSharedModule::GetPlatformString();
		if (bServer) {
			return InFileMetadata.Type == EApiFileType::Pak && InFileMetadata.Platform == PlatformString && InFileMetadata.DeploymentType == EApiFileDeploymentType::Server;
		}
		return InFileMetadata.Type == EApiFileType::Pak && InFileMetadata.Platform == PlatformString && InFileMetadata.DeploymentType == EApiFileDeploymentType::Client;
	});

	if (Result) {
		return Result->Size;
	}

	return -1;
}

FString FPakPakSubsystem::GetMetaversePakPath(const FGuid& InPakFileId) {
	return FPaths::ConvertRelativePathToFull(FPaths::ProjectPersistentDownloadDir() / TEXT("Paks") / InPakFileId.ToString() + FString(TEXT(".pak")));
}

bool FPakPakSubsystem::DoesMetaversePakFileExist(const FApiPackageMetadata& InModMetadata, const bool bServer) {
	const FGuid ModPakFileId = GetMetaversePakFileId(InModMetadata, bServer);
	if (!ModPakFileId.IsValid()) {
		return false;
	}

	const FString ModPakFilePath = GetMetaversePakPath(ModPakFileId);
	return DoesPakFileExist(ModPakFilePath);
}

bool FPakPakSubsystem::DoesPakFileExist(const FString& InPakFilePath) {
	if (InPakFilePath.IsEmpty()) {
		return false;
	}

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	return PlatformFile.FileExists(*InPakFilePath);
}

// FApiFileMetadata* UUIModDetailWidget::GetModPakFileMetadata() {
// 	return Metadata->Files.FindByPredicate([=](const FApiFileMetadata& File) {
// 		return File.Type == EApiFileType::Pak && File.Platform == FVeSharedModule::GetPlatform() && File.Version.Contains(TEXT("Client"));
// 	});
// }
//
// bool UUIModDetailWidget::GetPakFileExists() {
// 	const FApiFileMetadata* ModFile = GetModPakFileMetadata();
//
// 	if (ModFile) {
// 		IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
// 		return PlatformFile.FileExists(*GetModPakPath(ModFile->Id));
// 	}
//
// 	return false;
// }

bool FPakPakSubsystem::DoesPakFileExist(const FApiPackageMetadata& InModMetadata, const bool bServer) {
	const FGuid FileId = GetMetaversePakFileId(InModMetadata, bServer);
	const FString FilePath = GetMetaversePakPath(FileId);
	return DoesPakFileExist(FilePath);
}

bool FPakPakSubsystem::IsMetaversePakMounted(const FApiPackageMetadata& InModMetadata, const bool bServer) {
	const FGuid ModPakFileId = GetMetaversePakFileId(InModMetadata, bServer);
	const FString ModPakFilePath = GetMetaversePakPath(ModPakFileId);
	return IsPakMounted(ModPakFilePath);
}

bool FPakPakSubsystem::IsPakMounted(const FString& InPakFileName) {
#if WITH_PAK
	FPakLoader* PakLoaderPtr = FPakLoader::Get();

	if (!PakLoaderPtr) {
		GET_MODULE_SUBSYSTEM(MonitoringSubsystem, Monitoring, Monitoring);
		if (MonitoringSubsystem) {
			MonitoringSubsystem->ReportMessage(TEXT("Pak"),TEXT("error"), TEXT("failed to get a pak loader instance"));
		}

		return false;
	}

	return PakLoaderPtr->GetMountedPakFilenames().Contains(InPakFileName);
#else
	return false;
#endif
}

bool FPakPakSubsystem::MountPakFile(const FString& InPakFileName, const FString& InPakMountPath) {
#if WITH_PAK
	FPakLoader* PakLoaderPtr = FPakLoader::Get();
	if (PakLoaderPtr) {
		if (PakLoaderPtr->MountPakFile(InPakFileName, INDEX_NONE, InPakMountPath)) {
			return true;
		}

		GET_MODULE_SUBSYSTEM(MonitoringSubsystem, Monitoring, Monitoring);
		if (MonitoringSubsystem) {
			MonitoringSubsystem->ReportMessage(TEXT("Pak"), TEXT("error"), FString::Printf(TEXT("failed to mount a pak file: %s, %s"), *InPakFileName, *InPakMountPath));
		}

		return false;
	}

	GET_MODULE_SUBSYSTEM(MonitoringSubsystem, Monitoring, Monitoring);
	if (MonitoringSubsystem) {
		MonitoringSubsystem->ReportMessage(TEXT("Pak"), TEXT("error"), TEXT("failed to get a pak loader instance"));
	}
#endif
	return false;
}

bool FPakPakSubsystem::UnmountPakFile(const FString& InPakFileName) {
#if WITH_PAK
	FPakLoader* PakLoaderPtr = FPakLoader::Get();
	if (PakLoaderPtr) {
		if (PakLoaderPtr->UnmountPakFile(InPakFileName)) {
			return true;
		}

		GET_MODULE_SUBSYSTEM(MonitoringSubsystem, Monitoring, Monitoring);
		if (MonitoringSubsystem) {
			MonitoringSubsystem->ReportMessage(TEXT("Pak"), TEXT("error"), TEXT("failed to unmount a pak file"));
		}

		return false;
	}

	GET_MODULE_SUBSYSTEM(MonitoringSubsystem, Monitoring, Monitoring);
	if (MonitoringSubsystem) {
		MonitoringSubsystem->ReportMessage(TEXT("Pak"), TEXT("error"), TEXT("failed to get a pak loader instance"));
	}
#endif
	return false;
}

bool FPakPakSubsystem::MountMetaversePakFile(const FGuid& InModPakFileId) {
#if WITH_PAK
	FPakLoader* PakLoaderPtr = FPakLoader::Get();

	if (!PakLoaderPtr) {
		GET_MODULE_SUBSYSTEM(MonitoringSubsystem, Monitoring, Monitoring);
		if (MonitoringSubsystem) {
			MonitoringSubsystem->ReportMessage(TEXT("Pak"), TEXT("error"), TEXT("failed to get a pak loader instance"));
		}

		return false;
	}

	const FString ModPakPath = GetMetaversePakPath(InModPakFileId);
	if (PakLoaderPtr->MountPakFile(ModPakPath, INDEX_NONE, TEXT(""))) {
		return true;
	}

	GET_MODULE_SUBSYSTEM(MonitoringSubsystem, Monitoring, Monitoring);
	if (MonitoringSubsystem) {
		MonitoringSubsystem->ReportMessage(TEXT("Pak"), TEXT("error"), TEXT("failed to mount a pak file"));
	}
#endif
	return false;
}

void FPakPakSubsystem::RegisterMetaverseMountPoint(const FString& InModName) {
#if WITH_PAK
	if (FPakLoader* PakLoaderPtr = FPakLoader::Get()) {
		const FString RootPath = FString(TEXT("/")) + InModName + TEXT("/");
		const FString ContentPath = FString::Printf(TEXT("../../Plugins/%s/Content/"), *InModName);
		PakLoaderPtr->RegisterMountPoint(RootPath, ContentPath);
	} else {
		GET_MODULE_SUBSYSTEM(MonitoringSubsystem, Monitoring, Monitoring);
		if (MonitoringSubsystem) {
			MonitoringSubsystem->ReportMessage(TEXT("Pak"), TEXT("error"), TEXT("failed to get a pak loader instance"));
		}
	}
#endif
}

void FPakPakSubsystem::LogFilesInDirectoryRecursively(const FString& ModName) {
#if WITH_PAK
	FPakLoader* PakLoaderPtr = FPakLoader::Get();
	if (PakLoaderPtr) {
		TArray<FString> List = PakLoaderPtr->GetFilesInDirectoryRecursively(TEXT("../../..") / ModName);
		for (FString Item : List) {
			FString OutPath;
			UPakLoaderLibrary::TryConvertFilenameToLongPackageName(Item, OutPath);
			// LogScreenLogF("%s -> %s", *Item, *OutPath);
		}
	} else {
		if (FVeMonitoringModule* MonitoringModule = FVeMonitoringModule::Get()) {
			if (const TSharedPtr<FMonitoringMonitoringSubsystem> MonitoringSubsystem = MonitoringModule->GetMonitoringSubsystem()) {
				MonitoringSubsystem->ReportMessage(TEXT("Pak"), TEXT("error"), TEXT("failed to get a pak loader instance"));
			} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Monitoring), STRINGIFY(Monitoring)); }
		} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Monitoring)); }
	}
#endif
}

void FPakPakSubsystem::LoadPakAssetRegistry(const FString& ModName) {
#if WITH_PAK
	FPakLoader* PakLoaderPtr = FPakLoader::Get();
	if (PakLoaderPtr) {
		const FString Path = FString::Printf(TEXT("../../Plugins/%s/AssetRegistry.bin"), *ModName);
		PakLoaderPtr->LoadAssetRegistryFile(Path);
		// if (!) {
		// 	if (FVeMonitoringModule* MonitoringModule = FVeMonitoringModule::Get()) {
		// 		if (const TSharedPtr<FVeMonitoringSubsystem> MonitoringSubsystem = MonitoringModule->GetMonitoringSubsystem()) {
		// 			MonitoringSubsystem->ReportMessage(TEXT("Pak"), TEXT("error"), FString::Printf(TEXT("failed to load an asset registry from a file: %s"), *Path));
		// 		} else { LogErrorF("failed to get subsystem F%s%sSubsystem", STRINGIFY(Monitoring), STRINGIFY(Monitoring)); }
		// 	} else { LogErrorF("failed to get module FVe%sModule", STRINGIFY(Monitoring)); }
		// }
	} else {
		GET_MODULE_SUBSYSTEM(MonitoringSubsystem, Monitoring, Monitoring);
		if (MonitoringSubsystem) {
			MonitoringSubsystem->ReportMessage(TEXT("Pak"), TEXT("error"), TEXT("failed to get a pak loader instance"));
		}
	}
#endif
}

FString FPakPakSubsystem::GetPakPath(const FGuid& InId) {
	return FPaths::ConvertRelativePathToFull(FPaths::ProjectPersistentDownloadDir() / TEXT("Paks") / InId.ToString() + FString(TEXT(".pak")));
}

bool FPakPakSubsystem::IsPakMounted(const FGuid& InId) {
	return IsPakMounted(GetPakPath(InId));
}

bool FPakPakSubsystem::DoesMetaversePakFileExist(const FGuid& InId) {
	return DoesPakFileExist(GetPakPath(InId));
}
