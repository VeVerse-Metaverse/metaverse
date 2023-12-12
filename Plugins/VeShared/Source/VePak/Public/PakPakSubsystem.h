// Author: Egor A. Pristavka

#pragma once

#include "ApiPackageMetadata.h"
#include "VeShared.h"

struct FVePackageMetadata;
class FPakPlatformFile;

class VEPAK_API FPakPakSubsystem final : public IModuleSubsystem {
public:
	const static FName Name;

private:
	/** List of mounted pak files and corresponding paths. */
	TMap<FString, TPair<FString, FString>> MountedPakFiles;

	/** Mount pak file by its name. */
	static bool MountPakFile(const FString& InPakFileName, const FString& InPakMountPath);
public:
	/** Check if pak file mounted. */
	static bool IsPakMounted(const FString& InPakFileName);
	/** Unmount pak file by its name. */
	static bool UnmountPakFile(const FString& InPakFileName);

	virtual void Initialize() override;
	virtual void Shutdown() override;

	/** Returns URL for the mod pak file. */
	static FString GetMetaversePakFileUrl(const FApiPackageMetadata& InModMetadata, bool bServer);

	/** Returns mod pak file id using mod metadata. */
	static FGuid GetMetaversePakFileId(const FApiPackageMetadata& InModMetadata, bool bServer);

	/** Returns mod pak size using mod metadata. */
	static int64 GetMetaversePakFileSize(const FApiPackageMetadata& InModMetadata, bool bServer);

	/** Returns mod pak path on the disk by pak file id. */
	static FString GetMetaversePakPath(const FGuid& InPakFileId);

	/** Checks if the mod pak file exists. */
	static bool DoesMetaversePakFileExist(const FApiPackageMetadata& InModMetadata, const bool bServer);
	
	/** Checks if the pak file exists. */
	static bool DoesPakFileExist(const FString& InPakFilePath);

	/** Checks if the pak file exists. */
	static bool DoesPakFileExist(const FApiPackageMetadata& InModMetadata, bool bServer);

	/** Check if the mod pak file is mounted. */
	static bool IsMetaversePakMounted(const FApiPackageMetadata& InModMetadata, bool bServer);

	/** Mount the mod pak file. */
	[[deprecated]] static bool MountMetaversePakFile(const FGuid& InModPakFileId);

	static void RegisterMetaverseMountPoint(const FString& InModName);
	static void LogFilesInDirectoryRecursively(const FString& ModName);
	static void LoadPakAssetRegistry(const FString& ModName);

	static FString GetPakPath(const FGuid& InId);
	static bool IsPakMounted(const FGuid& InId);
	static bool DoesMetaversePakFileExist(const FGuid& InId);
};

typedef TSharedPtr<FPakPakSubsystem> FPakSubsystemPtr;
