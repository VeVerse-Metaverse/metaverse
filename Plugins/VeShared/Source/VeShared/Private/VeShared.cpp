// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#include "VeShared.h"

#if PLATFORM_WINDOWS
#include "Windows/AllowWindowsPlatformTypes.h"

static bool IsPlatformFileHidden(const FString& Path) {
	if (FPaths::GetCleanFilename(Path).StartsWith(TEXT("."))) {
		return true;
	}

	BY_HANDLE_FILE_INFORMATION HandleFileInformation;
	if (GetFileAttributesEx(*Path, GetFileExInfoStandard, &HandleFileInformation)) {
		uint32 Result = HandleFileInformation.dwFileAttributes;
		if (Result != 0xFFFFFFFF) {
			return !!(Result & FILE_ATTRIBUTE_HIDDEN) || !!(Result & FILE_ATTRIBUTE_SYSTEM) || !!(Result & FILE_ATTRIBUTE_DEVICE);
		}
	}

	return false;
}

#include "Windows/HideWindowsPlatformTypes.h"

#else
static bool IsPlatformFileHidden(const FString& Path) {
	return Path.StartsWith(".");
}
#endif

#include "Modules/ModuleManager.h"
#include "PlatformFileInfo.h"
#include "TimerManager.h"


VESHARED_API DEFINE_LOG_CATEGORY(LogVe);
VESHARED_API DEFINE_LOG_CATEGORY(LogVeShared);

IMPLEMENT_GAME_MODULE(FVeSharedModule, VeShared);

FVeSharedModule::FVeSharedTickableObject* FVeSharedModule::TickableObject = nullptr;


void FVeSharedModule::StartupModule() {
	VeLog("SharedModuleTimestamp: %s", ANSI_TO_TCHAR(__TIMESTAMP__));
	if (!TickableObject) {
		TickableObject = new FVeSharedTickableObject();
	}
}

void FVeSharedModule::ShutdownModule() {
	if (TickableObject) {
		delete TickableObject;
		TickableObject = nullptr;
	}
}

#if PLATFORM_WINDOWS
FString FVeSharedModule::PlatformString = TEXT("Win64");
EVePlatform FVeSharedModule::PlatformType = EVePlatform::Win64;
#elif PLATFORM_LINUX
FString FVeSharedModule::PlatformString = TEXT("Linux");
EVePlatform FVeSharedModule::PlatformType = EVePlatform::Linux;
#elif PLATFORM_MAC
FString FVeSharedModule::PlatformString = TEXT("Mac");
EVePlatform FVeSharedModule::PlatformType = EVePlatform::Mac;
#elif PLATFORM_ANDROID
FString FVeSharedModule::PlatformString = TEXT("Android");
EVePlatform FVeSharedModule::PlatformType = EVePlatform::Android;
#elif PLATFORM_IOS
FString FVeSharedModule::PlatformString = TEXT("IOS");
EVePlatform FVeSharedModule::PlatformType = EVePlatform::IOS;
#else
FString FVeSharedModule::PlatformString = TEXT("");
EVePlatform FVeSharedModule::PlatformType = EVePlatform::Unknown;
#endif

#pragma region Logging

void ISubsystemModule::AddSubsystem(const FName InSubsystemName, const TSharedRef<IModuleSubsystem> Subsystem) {
	FScopeLock Lock(&SubsystemCriticalSection);
	Subsystems.Add(InSubsystemName, Subsystem);
}

void ISubsystemModule::StartupModule() {
	FScopeLock Lock(&SubsystemCriticalSection);
	for (auto It = Subsystems.CreateConstIterator(); It; ++It) {
		It.Value()->Initialize();
	}
}

void ISubsystemModule::ShutdownModule() {
	FScopeLock Lock(&SubsystemCriticalSection);
	for (auto It = Subsystems.CreateConstIterator(); It; ++It) {
		It.Value()->Shutdown();
	}
}

void _Log(const FString Name, const FString Message, const TCHAR* Color) {
	SET_WARN_COLOR(Color);
	UE_LOG(LogVe, Log, TEXT("%s @ %s()"), *Message, *Name);
	CLEAR_WARN_COLOR();
}

void _LogDisplay(const FString Name, const FString Message, const TCHAR* Color) {
	SET_WARN_COLOR(Color);
	UE_LOG(LogVe, Display, TEXT("%s @ %s()"), *Message, *Name);
	CLEAR_WARN_COLOR();
}

void _LogVerbose(const FString Name, const FString Message, const TCHAR* Color) {
	SET_WARN_COLOR(Color);
	UE_LOG(LogVe, Verbose, TEXT("%s @ %s()"), *Message, *Name);
	CLEAR_WARN_COLOR();
}

void _LogWarning(const FString Name, const FString Message, const TCHAR* Color) {
	SET_WARN_COLOR(Color);
	UE_LOG(LogVe, Warning, TEXT("%s @ %s()"), *Message, *Name);
	CLEAR_WARN_COLOR();
}

void _LogError(const FString Name, const FString Message, const TCHAR* Color) {
	SET_WARN_COLOR(Color);
	UE_LOG(LogVe, Error, TEXT("%s @ %s()"), *Message, *Name);
	CLEAR_WARN_COLOR();
}

void _Screen(const FString Name, const FString Message, const FColor& Color, const float Time) {
	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, Time, Color, FString::Printf(TEXT("%s @ %s()"), *Message, *Name));
	}
}

struct FPlatformFileVisitor final : public IPlatformFile::FDirectoryVisitor {
	explicit FPlatformFileVisitor(IPlatformFile& InPlatformFileInterface)
		: PlatformFileInterface(InPlatformFileInterface) { }

	virtual bool Visit(const TCHAR* FilenameOrDirectory, const bool bIsDirectory) override {
		// Make sure that all path are standard.
		FString RelativeFileName = FilenameOrDirectory;
		FPaths::MakeStandardFilename(RelativeFileName);

		if (IsPlatformFileHidden(RelativeFileName)) {
			return true;
		}

		FPlatformFileInfo FileInfo;
		FileInfo.bIsDirectory = bIsDirectory;
		if (!bIsDirectory) {
			FileInfo.Extension = FPaths::GetExtension(RelativeFileName);
		}
		FileInfo.Directory = FPaths::GetPath(RelativeFileName);
		FileInfo.Name = FPaths::GetCleanFilename(RelativeFileName);
		FileInfo.Path = RelativeFileName;
		FileInfo.Size = PlatformFileInterface.FileSize(FilenameOrDirectory);
		FileInfo.UpdatedAt = PlatformFileInterface.GetTimeStamp(FilenameOrDirectory);

		Files.Add(FileInfo);

		return true;
	}

	IPlatformFile& PlatformFileInterface;
	TArray<FPlatformFileInfo> Files;
};

TArray<FPlatformFileInfo> GetFilesInDirectory(const FString Directory, const FPlatformFileFilter* FilterFunction, const bool bForceIncludeDirectories) {
	TArray<FPlatformFileInfo> Files;

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	FPlatformFileVisitor Visitor(PlatformFile);
	PlatformFile.IterateDirectory(*Directory, Visitor);

	if (!FilterFunction || !FilterFunction->IsBound()) {
		return Visitor.Files;
	}

	for (auto File : Visitor.Files) {
		if (File.bIsDirectory) {
			if (bForceIncludeDirectories) {
				Files.Add(File);
				continue;
			}
		}

		if (FilterFunction->Execute(File)) {
			Files.Add(File);
		}
	}

	return Files;
}

#pragma endregion

FString TruncateString(const FString& InString, const int MaxLength, const bool bAppendEllipsis) {
	if (InString.Len() > MaxLength) {
		return InString.Left(MaxLength) + (bAppendEllipsis ? TEXT("…") : TEXT(""));
	}

	return InString;
}

FString FileTypeToString(const EFileMediaType& Type) {
	switch (Type) {
	case EFileMediaType::Audio:
		return TEXT("audio");
	case EFileMediaType::Image:
		return TEXT("image_2d");
	case EFileMediaType::Mesh:
		return TEXT("mesh_3d");
	case EFileMediaType::Video:
		return TEXT("video_2d");
	case EFileMediaType::Youtube:
		return TEXT("youtube");
	case EFileMediaType::PDF:
		return TEXT("pdf");
	default:
		return TEXT("unknown");
	}
}

// EFileMediaType FileTypeFromString(const FString& Source) {
// 	if (Source == TEXT("audio")) {
// 		return EFileMediaType::Audio;
// 	}
//
// 	if (Source == TEXT("image_2d")) {
// 		return EFileMediaType::Image;
// 	}
//
// 	if (Source == TEXT("mesh_3d")) {
// 		return EFileMediaType::Mesh;
// 	}
//
// 	if (Source == TEXT("video_2d")) {
// 		return EFileMediaType::Video;
// 	}
//
// 	if (Source == TEXT("youtube")) {
// 		return EFileMediaType::Youtube;
// 	}
//
// 	if (Source == TEXT("pdf")) {
// 		return EFileMediaType::PDF;
// 	}
//
// 	return EFileMediaType::Unknown;
// }

FString GetMediaType(const FString& MimeType, const FString& Url) {
	FString MediaType;
	if (MimeType.StartsWith(TEXT("image/"))) {
		MediaType = TEXT("image_2d");
	} else if (MimeType.EndsWith(TEXT("mp4")) || MimeType == TEXT("application/mp4")) {
		MediaType = TEXT("video_2d");
	} else if ((MimeType.StartsWith(TEXT("model/")) && MimeType.Contains(TEXT("gltf"))) || MimeType.Contains(TEXT("glb"))) {
		MediaType = TEXT("mesh_3d");
	} else if (MimeType.StartsWith(TEXT("audio/"))) {
		MediaType = TEXT("audio");
	} else if (MimeType == TEXT("application/x-pdf") || MimeType == TEXT("application/pdf")) {
		MediaType = TEXT("pdf");
	} else if (Url.StartsWith(TEXT("http")) && (Url.Contains(TEXT("youtube.com")) || Url.Contains(TEXT("youtu.be")))) {
		MediaType = TEXT("youtube");
	} else {
		MediaType = TEXT("unknown");
	}
	return MediaType;
}
