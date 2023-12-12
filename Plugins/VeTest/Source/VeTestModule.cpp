// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#include "VeTestModule.h"

#include "Kismet/GameplayStatics.h"
#if WITH_PAK
#include "PakLoaderLibrary.h"
#include "PakLoader/Public/PakLoader.h"
#endif

IMPLEMENT_GAME_MODULE(FVeTestModule, VeTest);

static FAutoConsoleCommandWithWorld GLogPakFiles(TEXT("ve.test.LogPakFiles"),
												 *NSLOCTEXT("LE7EL", "CCommandText_LogPakFiles",
															"Dumps information about loaded paks and assets to the log.").ToString(),
												 FConsoleCommandWithWorldDelegate::CreateStatic(&FVeTestModule::LogPakFilesCommand)
);

static FAutoConsoleCommandWithWorldAndArgs GLoadPakMap(TEXT("ve.test.LoadPakMap"),
													   *NSLOCTEXT("LE7EL", "CCommandText_LoadPakMap",
																  "Loads map from pak.").ToString(),
													   FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(&FVeTestModule::LoadPakMapCommand)
);

void FVeTestModule::StartupModule() {
#if WITH_PAK
	UPakLoaderLibrary::EnableRuntimeLog(true, FString(TEXT("PakLoaderLog.txt")));
#endif
	ISubsystemModule::StartupModule();
}

void FVeTestModule::ShutdownModule() {
	ISubsystemModule::ShutdownModule();
}

FVeTestModule* FVeTestModule::Get() {
	if (FModuleManager::Get().IsModuleLoaded(TEXT("VeTest"))) {
		return FModuleManager::Get().GetModulePtr<FVeTestModule>(TEXT("VeTest"));
	}
	return FModuleManager::Get().LoadModulePtr<FVeTestModule>(TEXT("VeTest"));
}

void FVeTestModule::LogPakFilesCommand(UWorld* InWorld) {
#if WITH_PAK
	if (FPakLoader* PakLoader = FPakLoader::Get()) {
		TArray<FString> PakFiles = PakLoader->GetMountedPakFilenames();
		for (const FString PakFile : PakFiles) {
			LogF("mounted pak: %s", *PakFile);

			TArray<FString> AssetFiles = PakLoader->GetFilesInPak(PakFile, false);
			for (FString AssetFile : AssetFiles) {
				LogF("mounted asset: %s", *AssetFile);
			}
		}
	} else {
		LogErrorF("failed to get pak loader");
	}
#endif
}

void FVeTestModule::LoadPakMapCommand(const TArray<FString>& InArgs, UWorld* InWorld) {
#if WITH_PAK
	if (InArgs.Num() > 0) {
		if (FPakLoader* PakLoader = FPakLoader::Get()) {
			TArray<FString> PakFiles = PakLoader->GetMountedPakFilenames();
			for (const FString PakFile : PakFiles) {
				TArray<FString> AssetFiles = PakLoader->GetFilesInPak(PakFile, false);
				for (FString AssetFile : AssetFiles) {
					if (AssetFile.Contains(InArgs[0]) && AssetFile.EndsWith(TEXT(".umap"))) {
						LogWarningF("found map %s", *AssetFile);
						FString AssetShortName = FPackageName::GetShortName(AssetFile);
						AssetShortName.RemoveFromEnd(FPackageName::GetMapPackageExtension());
						const FString AssetLongName = FPackageName::GetLongPackagePath(AssetFile);
						FPaths::MakeStandardFilename(AssetFile);
						LogWarningF("opening level %s from %s", *AssetShortName, *AssetLongName);
						UGameplayStatics::OpenLevel(InWorld, FName(AssetShortName), false);
						return;
					}
				}
				LogWarningF("unable to find map %s", *InArgs[0]);
			}
		} else {
			LogErrorF("failed to get pak loader");
		}
	}
#endif
}
