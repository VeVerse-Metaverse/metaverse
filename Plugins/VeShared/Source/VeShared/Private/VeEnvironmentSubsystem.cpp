// Author: Egor A. Pristavka

#include "VeEnvironmentSubsystem.h"

FString FVeEnvironmentSubsystem::GetEnvironmentVariable(const FString& Key) {
#if PLATFORM_WINDOWS
#include "Windows/WindowsPlatformMisc.h"
	return FWindowsPlatformMisc::GetEnvironmentVariable(*Key);
#elif PLATFORM_LINUX
#include "Linux/LinuxPlatformMisc.h"
	return FLinuxPlatformMisc::GetEnvironmentVariable(*Key);
#elif PLATFORM_MAC
#include "Mac/MacPlatformMisc.h"
	return FMacPlatformMisc::GetEnvironmentVariable(*Key);
#elif PLATFORM_IOS
#include "IOS/IOSPlatformMisc.h"
	return FIOSPlatformMisc::GetEnvironmentVariable(*Key);
#elif PLATFORM_ANDROID
#include "Android/AndroidPlatformMisc.h"
	return FAndroidMisc::GetEnvironmentVariable(*Key);
#endif
}

bool FVeEnvironmentSubsystem::IsMobilePlatform() {
#if PLATFORM_IOS || PLATFORM_ANDROID || PLATFORM_ANDROID_ARM || PLATFORM_ANDROID_ARM64 || PLATFORM_ANDROID_X64 || PLATFORM_ANDROID_X86
	return true;
#else
	return false;
#endif
}

bool FVeEnvironmentSubsystem::IsDesktopPlatform() {
#if PLATFORM_IOS || PLATFORM_ANDROID || PLATFORM_ANDROID_ARM || PLATFORM_ANDROID_ARM64 || PLATFORM_ANDROID_X64 || PLATFORM_ANDROID_X86
	return false;
#else
	return true;
#endif
}
