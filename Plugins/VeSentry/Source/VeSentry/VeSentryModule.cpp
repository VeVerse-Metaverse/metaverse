// Author: Egor A. Pristavka. 
// Copyright (c) 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All rights reserved.

#include "VeSentryModule.h"

#if WITH_SENTRY
#if PLATFORM_WINDOWS
#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows/prewindowsapi.h"
#endif
#include "sentry.h"
#endif

#include "Interfaces/IPluginManager.h"

DEFINE_LOG_CATEGORY(LogVeSentry);

IMPLEMENT_GAME_MODULE(FVeSentryModule, VeSentry);

void FVeSentryModule::StartupModule() {
#if WITH_SENTRY

#if PLATFORM_WINDOWS
	_sentrySdkHandle = LoadSharedLibrary(TEXT("sentry.dll"));
#elif PLATFORM_LINUX
	_sentrySdkHandle = LoadSharedLibrary(TEXT("libsentry.so"));
#elif PLATFORM_MAC
	_sentrySdkHandle = LoadSharedLibrary(TEXT("libsentry.so"));
#endif

	if (!_sentrySdkHandle) {
		return;
	}

	sentry_options_t* options = sentry_options_new();
	sentry_options_set_dsn(options, "https://a6c4b39d37d74cdd8fca48b7ce8bf05c@o1111473.ingest.sentry.io/6140756");
	sentry_options_set_debug(options, 1);
	sentry_options_set_environment(options, "Production");
	sentry_options_set_release(options, "1.0.0");
	if (const int ReturnCode = sentry_init(options)) {
		UE_LOG(LogVeSentry, Error, TEXT("sentry init returned code: %d"), ReturnCode);
	}
#endif
}

void FVeSentryModule::ShutdownModule() {
#if WITH_SENTRY
	// make sure everything flushes
	sentry_close();

	if (_sentrySdkHandle) {
		FPlatformProcess::FreeDllHandle(_sentrySdkHandle);
	}
#endif
}

#if WITH_SENTRY
void* FVeSentryModule::LoadSharedLibrary(const TCHAR* libraryName) {
	void* LibraryHandle;
	// Get the base directory of this plugin
	FString BaseDir = IPluginManager::Get().FindPlugin("VeSentry")->GetBaseDir();

	// Add on the relative location of the third party dll and load it
	FString LibraryPath;
#if PLATFORM_WINDOWS
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/SentryLibrary/Windows"));
#elif PLATFORM_LINUX
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/SentryLibrary/Linux"));
#elif PLATFORM_MAC
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/SentryLibrary/Linux"));
#endif

	FPlatformProcess::PushDllDirectory(*LibraryPath);
	LibraryHandle = !LibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*(LibraryPath / libraryName)) : nullptr;
	FPlatformProcess::PopDllDirectory(*LibraryPath);

	if (LibraryHandle == nullptr) {
		UE_LOG(LogVeSentry, Error, TEXT("Failed to load library (%s)"), *(LibraryPath / libraryName));
	} else {
		UE_LOG(LogVeSentry, Log, TEXT("Loaded Library (%s)"), *(LibraryPath / libraryName));
	}

	return LibraryHandle;
}
#endif

void FVeSentryModule::SetUser(const FString& InId, const FString& InEmail, const FString& InName) {
#if WITH_SENTRY
	sentry_remove_user();

	const sentry_value_t User = sentry_value_new_object();
	sentry_value_set_by_key(User, "id", sentry_value_new_string(TCHAR_TO_UTF8(*InId)));
	sentry_value_set_by_key(User, "email", sentry_value_new_string(TCHAR_TO_UTF8(*InEmail)));
	sentry_value_set_by_key(User, "name", sentry_value_new_string(TCHAR_TO_UTF8(*InName)));
	sentry_value_set_by_key(User, "ip_address", sentry_value_new_string("{{auto}}"));
	sentry_set_user(User);
#endif
}

void FVeSentryModule::SendMessage(const FString& InCategory, const FString& InLevel, const FString& InMessage) {
#if WITH_SENTRY
	FString Category = InCategory.IsEmpty() ? TEXT("custom") : InCategory;

	sentry_level_t Level = SENTRY_LEVEL_INFO;
	if (InLevel == TEXT("error")) {
		Level = SENTRY_LEVEL_ERROR;
	} else if (InLevel == TEXT("fatal")) {
		Level = SENTRY_LEVEL_FATAL;
	} else if (InLevel == TEXT("debug")) {
		Level = SENTRY_LEVEL_DEBUG;
	} else if (InLevel == TEXT("warning")) {
		Level = SENTRY_LEVEL_WARNING;
	}

	const sentry_value_t Event = sentry_value_new_message_event(
		/*   level */ Level,
					  /*  logger */ TCHAR_TO_UTF8(*Category),
					  /* message */ TCHAR_TO_UTF8(*InMessage)
	);

	const sentry_uuid_t EventId = sentry_capture_event(Event);

	if (sentry_uuid_is_nil(&EventId)) {
		UE_LOG(LogVeSentry, Error, TEXT("failed to capture a sentry event"));
	}
#endif
}

#if PLATFORM_WINDOWS
#include "Windows/PostWindowsApi.h"
#include "Windows/HideWindowsPlatformTypes.h"
#endif
