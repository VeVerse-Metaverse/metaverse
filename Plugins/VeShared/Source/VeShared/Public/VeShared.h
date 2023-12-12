// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

#pragma once

#include "Misc/LocalTimestampDirectoryVisitor.h"
#include "Modules/ModuleManager.h"
#include "Serialization/JsonWriter.h"
#include "Policies/CondensedJsonPrintPolicy.h"

struct FPlatformFileInfo;
VESHARED_API DECLARE_LOG_CATEGORY_EXTERN(LogVe, All, All);

VESHARED_API DECLARE_LOG_CATEGORY_EXTERN(LogVeShared, All, All);

class IModuleSubsystem;


#pragma region Logging

#define VeLogExt(CategoryName, Verbosity, ModuleName, str, ...) \
	UE_LOG(CategoryName, Verbosity, TEXT("[%s] %s"), TEXT(VE_STRINGIFY2(ModuleName)), *FString::Printf(TEXT(str), ##__VA_ARGS__))

#define VeLogExtFunc(CategoryName, Verbosity, ModuleName, str, ...) \
	UE_LOG(CategoryName, Verbosity, TEXT("[%s] %s %s"), TEXT(VE_STRINGIFY2(ModuleName)), *FString(FUNC_NAME), *FString::Printf(TEXT(str), ##__VA_ARGS__))

#define VeLog(str, ...) VeLogExt(VE_LOG_CATEGORY, Log, VE_LOG_MODULE, str, ##__VA_ARGS__)
#define VeLogFunc(str, ...) VeLogExtFunc(VE_LOG_CATEGORY, Log, VE_LOG_MODULE, str, ##__VA_ARGS__)
#define VeLogError(str, ...) VeLogExt(VE_LOG_CATEGORY, Error, VE_LOG_MODULE, str, ##__VA_ARGS__)
#define VeLogErrorFunc(str, ...) VeLogExtFunc(VE_LOG_CATEGORY, Error, VE_LOG_MODULE, str, ##__VA_ARGS__)
#define VeLogWarning(str, ...) VeLogExt(VE_LOG_CATEGORY, Warning, VE_LOG_MODULE, str, ##__VA_ARGS__)
#define VeLogWarningFunc(str, ...) VeLogExtFunc(VE_LOG_CATEGORY, Warning, VE_LOG_MODULE, str, ##__VA_ARGS__)
#define VeLogVerbose(str, ...) VeLogExt(VE_LOG_CATEGORY, Verbose, VE_LOG_MODULE, str, ##__VA_ARGS__)
#define VeLogVerboseFunc(str, ...) VeLogExtFunc(VE_LOG_CATEGORY, Verbose, VE_LOG_MODULE, str, ##__VA_ARGS__)
#ifdef WITH_DEV_AUTOMATION_TESTS
#define VeLogTestFunc(str, ...) VeLogExtFunc(VE_LOG_CATEGORY, Log, VE_LOG_MODULE, str, ##__VA_ARGS__)
#define VeLogTestErrorFunc(str, ...) VeLogExtFunc(VE_LOG_CATEGORY, Error, VE_LOG_MODULE, str, ##__VA_ARGS__)
#else
#define VeLogTestFunc(str, ...)
#define VeLogTestErrorFunc(str, ...)
#endif

#ifndef LogSharedF
VESHARED_API void _Log(const FString Name, const FString Message, const TCHAR* Color = TEXT("0110"));
VESHARED_API void _LogDisplay(const FString Name, const FString Message, const TCHAR* Color = TEXT("0101"));
VESHARED_API void _LogVerbose(const FString Name, const FString Message, const TCHAR* Color = TEXT("0110"));
VESHARED_API void _LogWarning(const FString Name, const FString Message, const TCHAR* Color = TEXT("1100"));
VESHARED_API void _LogError(const FString Name, const FString Message, const TCHAR* Color = TEXT("1000"));
VESHARED_API void _Screen(const FString Name, const FString Message, const FColor& Color = FColor::Cyan, float Time = 0.0f);
#define LogF(Format, ...) _Log(FUNC_NAME, FString::Printf(TEXT(Format), ##__VA_ARGS__))
#define LogDisplayF(Format, ...) _LogVerbose(FUNC_NAME, FString::Printf(TEXT(Format), ##__VA_ARGS__))
#define LogVerboseF(Format, ...) _LogVerbose(FUNC_NAME, FString::Printf(TEXT(Format), ##__VA_ARGS__))
#define LogWarningF(Format, ...) _LogWarning(FUNC_NAME, FString::Printf(TEXT(Format), ##__VA_ARGS__))
#define LogErrorF(Format, ...) _LogError(FUNC_NAME, FString::Printf(TEXT(Format), ##__VA_ARGS__))
#if WITH_EDITOR && !UE_SERVER
#define ScreenF(Format, Time, ...) _Screen(FUNC_NAME, FString::Printf(TEXT(Format), ##__VA_ARGS__), Time)
#define ScreenColorF(Color, Format, Time, ...) _Screen(FUNC_NAME, FString::Printf(TEXT(Format), ##__VA_ARGS__), Color, Time)
#define LogScreenF(Color, Format, ...) ScreenColorF(Color, Format, 3.0f, ##__VA_ARGS__); LogF(Format, ##__VA_ARGS__);
#define LogScreenLogF(Format, ...) ScreenColorF(FColor::Cyan, Format, 3.0f, ##__VA_ARGS__); LogF(Format, ##__VA_ARGS__);
#define LogScreenDisplayF(Format, ...) ScreenColorF(FColor::Green, Format, 3.0f, ##__VA_ARGS__); LogDisplayF(Format, ##__VA_ARGS__);
#define LogScreenVerboseF(Format, ...) ScreenColorF(FColor::Cyan, Format, 3.0f, ##__VA_ARGS__); LogVerboseF(Format, ##__VA_ARGS__);
#define LogScreenWarningF(Format, ...) ScreenColorF(FColor::Yellow, Format, 5.0f, ##__VA_ARGS__); LogWarningF(Format, ##__VA_ARGS__);
#define LogScreenErrorF(Format, ...) ScreenColorF(FColor::Red, Format, 5.0f, ##__VA_ARGS__); LogErrorF(Format, ##__VA_ARGS__);
#else
#define ScreenF(Format, ...)
#define ScreenColorF(Color, Format, ...)
#define LogScreenF(Color, Format, ...) LogF(Format, ##__VA_ARGS__);
#define LogScreenLogF(Format, ...) LogF(Format, ##__VA_ARGS__);
#define LogScreenDisplayF(Format, ...) LogDisplayF(Format, ##__VA_ARGS__);
#define LogScreenVerboseF(Format, ...) LogVerboseF(Format, ##__VA_ARGS__);
#define LogScreenWarningF(Format, ...) LogWarningF(Format, ##__VA_ARGS__);
#define LogScreenErrorF(Format, ...) LogErrorF(Format, ##__VA_ARGS__);
#endif
#endif

#pragma endregion

#pragma region Subsystem Module Interface

/** Base interface for modules having subsystems. */
class VESHARED_API ISubsystemModule : public IModuleInterface {
protected:
	TMap<FName, TSharedRef<IModuleSubsystem>> Subsystems;
	mutable FCriticalSection SubsystemCriticalSection;

public:
	void AddSubsystem(FName InSubsystemName, TSharedRef<IModuleSubsystem> Subsystem);

	template <typename T = IModuleSubsystem>
	TSharedPtr<T> FindSubsystem(const FName InSubsystemName) const {
		if (Subsystems.Num() <= 0) {
			return nullptr;
		}

		FScopeLock Lock(&SubsystemCriticalSection);
		TSharedPtr<IModuleSubsystem> Result = nullptr;
		if (const auto* Subsystem = Subsystems.Find(InSubsystemName)) {
			Result = *Subsystem;
		}
		return StaticCastSharedPtr<T>(Result);
	}

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

#define VE_STRINGIFY(X) VE_STRINGIFY2(X)
#define VE_STRINGIFY2(X) #X

#define STRINGIFY_X(a) TEXT(a)
#define STRINGIFY(a) STRINGIFY_X(#a)
#define CONCAT(a, b) a ## b

#if PLATFORM_WINDOWS

#define USING_MODULE_SUBSYSTEM(_Module, _Subsystem, ...) \
	if (FVe##_Module##Module * ##_Module##Module = FVe##_Module##Module::Get()) { \
		if (const TSharedPtr<F##_Module##_Subsystem##Subsystem> ##_Subsystem##Subsystem = ##_Module##Module->Get##_Subsystem##Subsystem()) { \
			__VA_ARGS__ \
		} else { LogErrorF("Failed to get subsystem F%s%sSubsystem", TEXT(#_Module), TEXT(#_Subsystem)); } \
	} else { LogErrorF("Failed to get module FVe%sModule", TEXT(#_Module)); }

#define USING_MODULE_TWO_SUBSYSTEMS(_Module, _SubsystemA, _SubsystemB, ...) \
	if (FVe##_Module##Module * ##_Module##Module = FVe##_Module##Module::Get()) { \
		if (const TSharedPtr<F##_Module##_SubsystemA##Subsystem> ##_SubsystemA##Subsystem = ##_Module##Module->Get##_SubsystemA##Subsystem()) { \
			if (const TSharedPtr<F##_Module##_SubsystemB##Subsystem> ##_SubsystemB##Subsystem = ##_Module##Module->Get##_SubsystemB##Subsystem()) { \
				__VA_ARGS__ \
			} else { LogErrorF("Failed to get subsystem F%s%sSubsystem", TEXT(#_Module), TEXT(#_SubsystemB)); } \
		} else { LogErrorF("Failed to get subsystem F%s%sSubsystem", TEXT(#_Module), TEXT(#_SubsystemA)); } \
	} else { LogErrorF("Failed to get module FVe%sModule", TEXT(#_Module)); }

#endif

#define CONCAT_MODULE_SUBSYSTEM_TYPE(_Module, _Subsystem) F##_Module##_Subsystem##Subsystem
#define CONCAT_MODULE_SUBSYSTEM_MODULE(_Module) FVe##_Module##Module
#define CONCAT_MODULE_SUBSYSTEM_GET(_Subsystem) Get##_Subsystem##Subsystem
#define GET_MODULE_SUBSYSTEM(Variable, Module, Subsystem)                                                   \
	TSharedPtr<CONCAT_MODULE_SUBSYSTEM_TYPE(Module, Subsystem)> Variable = nullptr;                         \
	if (auto* _Module = CONCAT_MODULE_SUBSYSTEM_MODULE(Module)::Get()) {                                    \
		if ((Variable = _Module->CONCAT_MODULE_SUBSYSTEM_GET(Subsystem)()) == nullptr) {                    \
			LogErrorF("Failed to get subsystem F%s%sSubsystem", TEXT(#Module), TEXT(#Subsystem));           \
		}                                                                                                   \
	} else {                                                                                                \
		LogErrorF("Failed to get module FVe%sModule", TEXT(#Module));                                       \
	}

#define GET_GAME_STATE(Class, Variable)\
	Class* Variable = nullptr; \
	auto* _World = GetWorld(); \
	if (IsValid(_World)) { \
		Variable = _World->GetGameState<Class>(); \
	}

#define CONCAT_WORLD_SUBSYSTEM_GET(_Subsystem) U##_Subsystem##Subsystem
#define GET_WORLD_SUBSYSTEM(Variable, Subsystem)                                                            \
	CONCAT_WORLD_SUBSYSTEM_GET(Subsystem)* Variable = nullptr;                                              \
	if (const auto* const _World = GetWorld()) {                                                            \
		if ((Variable = _World->GetSubsystem<CONCAT_WORLD_SUBSYSTEM_GET(Subsystem)>()) == nullptr) {        \
			LogErrorF("Failed to get subsystem U%sSubsystem", TEXT(#Subsystem));                            \
		}                                                                                                   \
	} else {                                                                                                \
		LogErrorF("Failed to get world");                                                                   \
	}

#pragma endregion

enum class EVePlatform : uint8 {
	Unknown,
	Win64,
	Linux,
	Mac,
	Android,
	IOS
};

class VESHARED_API FVeSharedModule final : public ISubsystemModule {
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static FString GetPlatformString() {
		return PlatformString;
	}

	static EVePlatform GetPlatformType() {
		return PlatformType;
	}

	/** Returns TimerManager instance for subsystems. */
	static FTimerManager& GetTimerManager() {
		check(TickableObject);
		// if (!TickableObject) {
		// 	VeLogError("Failed to get TickableObject.");
		// }
		return TickableObject->GetTimerManager();
	}

private:
	static EVePlatform PlatformType;
	static FString PlatformString;

	class FVeSharedTickableObject : public FTickableGameObject {
	public:
		virtual void Tick(float DeltaTime) override { TimerManager.Tick(DeltaTime); }
		// virtual bool IsTickable() const override { return true; }
		virtual bool IsTickableInEditor() const override { return true; }

		virtual TStatId GetStatId() const override {
			RETURN_QUICK_DECLARE_CYCLE_STAT(FVeSharedTickableObject, STATGROUP_Tickables);
		}

		FTimerManager& GetTimerManager() { return TimerManager; }

	private:
		FTimerManager TimerManager;
	};

	static FVeSharedTickableObject* TickableObject;
};

/** Base interface for subsystems. */
class VESHARED_API IModuleSubsystem {
public:
	virtual ~IModuleSubsystem() = default;
	virtual void Initialize() = 0;
	virtual void Shutdown() = 0;

	FTimerManager& GetTimerManager() const { return FVeSharedModule::GetTimerManager(); }
};


#pragma region Check

#if defined(_MSC_VER)
#define FUNC_NAME __FUNCTION__
#elif defined(__PRETTY_FUNCTION__)
#define FUNC_NAME __PRETTY_FUNCTION__
#else
#define FUNC_NAME ANSI_TO_TCHAR(__func__)
#endif

// #if PLATFORM_LINUX
// #define CHECK_RETURN(cond) if (!(cond)) { UE_LOG(LogVeShared, Error, TEXT("%s -> CHECK_RETURN(%s) Failed"), FUNC_NAME, TEXT(#cond)); return; }
// #define CHECK_RETURN_WARNING(cond) if (!(cond)) { UE_LOG(LogVeShared, Warning, TEXT("%s -> CHECK_RETURN(%s) Failed"), FUNC_NAME, TEXT(#cond)); return; }
// #define CHECK_RETURN_VALUE(cond, val) if (!(cond)) { UE_LOG(LogVeShared, Error, TEXT("%s -> CHECK_RETURN(%s) Failed"), FUNC_NAME, TEXT(#cond)); return val; }
// #else
#define CHECK_RETURN(cond) if (!(cond)) { UE_LOG(LogVeShared, Error, TEXT("%s -> CHECK_RETURN(%s) Failed"), FUNC_NAME, TEXT(#cond)); return; }
#define CHECK_RETURN_WARNING(cond) if (!(cond)) { UE_LOG(LogVeShared, Warning, TEXT("%s -> CHECK_RETURN(%s) Failed"), FUNC_NAME, TEXT(#cond)); return; }
#define CHECK_RETURN_VALUE(cond, val) if (!(cond)) { UE_LOG(LogVeShared, Error, TEXT("%s -> CHECK_RETURN(%s) Failed"), FUNC_NAME, TEXT(#cond)); return val; }
// #endif
#pragma endregion

#pragma region Enums
#ifndef UEnumShortToString
template <typename TEnum>
static FORCEINLINE FString GetUEnumAsString(const FString& Name, TEnum Value, const bool ShortName) {
	const UEnum* EnumPtr = FindObject<UEnum>(nullptr, *Name, true);
	if (!EnumPtr) return FString("InvalidUEnum");
	if (ShortName) return EnumPtr->GetNameStringByIndex(static_cast<int64>(Value));
	return EnumPtr->GetNameByValue(static_cast<int64>(Value)).ToString();
}

#define UEnumFullToString(Name, Value) GetUEnumAsString<Name>(#Name, Value, false)
#define UEnumShortToString(Name, Value) GetUEnumAsString<Name>(#Name, Value, true)
#endif
#pragma endregion

#pragma region Actor & UserWidget
#if WITH_EDITORONLY_DATA
#define RUN_DELAYED(seconds, ...)\
if (IsValid(this)) {\
if (const UWorld* const World = GetWorld()) {\
	FTimerHandle __tempTimerHandle;\
	World->GetTimerManager().SetTimer(__tempTimerHandle, FTimerDelegate().CreateWeakLambda(__VA_ARGS__), seconds, false);\
} else if (IsValid(this) && IsValid(GEngine) && GEngine->IsEditor()) {\
	auto* EditorEngine = Cast<UEditorEngine>(GEngine);\
	if (IsValid(EditorEngine) && EditorEngine->IsTimerManagerValid()) {\
		FTimerHandle __tempTimerHandle;\
		EditorEngine->GetTimerManager()->SetTimer(__tempTimerHandle, FTimerDelegate().CreateWeakLambda(__VA_ARGS__), seconds, false);\
	}\
}\
}
#else
#define RUN_DELAYED(seconds, ...)\
if (IsValid(this)) {\
if (const UWorld* const World = GetWorld()) {\
FTimerHandle __tempTimerHandle;\
World->GetTimerManager().SetTimer(__tempTimerHandle, FTimerDelegate().CreateWeakLambda(__VA_ARGS__), seconds, false);\
}\
}
#endif

#define RUN_LOOPED(seconds, ...)\
if (!IsPendingKill()) {\
if (const UWorld* const World = GetWorld()) {\
FTimerHandle __tempTimerHandle;\
World->GetTimerManager().SetTimer(__tempTimerHandle, FTimerDelegate().CreateWeakLambda(__VA_ARGS__), seconds, true);\
}\
}
#pragma endregion

#pragma region Properties
template <class T>
class TCachedProperty {
public:
	explicit TCachedProperty(T InValue) {
		bIsDirty = false;
		Value = InValue;
	}

	const T& GetValue() const {
		return Value;
	}

	void SetValue(const T& InValue) {
		if (Value != InValue) {
			Value = InValue;
			bIsDirty = true;
		}
	}

	void SetDirty(const bool InDirty) {
		bIsDirty = InDirty;
	}

	bool IsDirty() const {
		return bIsDirty;
	}

protected:
	bool bIsDirty;
	T Value;
};
#pragma endregion

#pragma region Android
#if PLATFORM_ANDROID
#include "AndroidPermissionFunctionLibrary.h"
#include "AndroidPermissionCallbackProxy.h"
#define RUN_WITH_PERMISSION(perm, ...)\
LogF("AndroidPermission: Checking %s permission.", perm);\
if (!UAndroidPermissionFunctionLibrary::CheckPermission(perm)) {\
    TArray<FString> Permissions;\
    Permissions.Add(perm);\
    if (UAndroidPermissionCallbackProxy* Callback = UAndroidPermissionFunctionLibrary::AcquirePermissions(Permissions)) {\
        LogF("AndroidPermission: Requested %s permission.", perm);\
        Callback->OnPermissionsGrantedDelegate.BindWeakLambda(this, [=](const TArray<FString>& Permissions, const TArray<bool>& GrantResults) {\
            LogF("AndroidPermission: On permissions granted called.");\
            if (GrantResults.Num() > 0) {\
                if (GrantResults[0]) {\
                    LogF("AndroidPermission: Acquired %s permission.", perm);\
                    __VA_ARGS__;\
                } else {\
                    LogWarningF("AndroidPermission: Failed to acquire %s permission: grant result is false.", perm);\
                }\
            } else {\
                LogWarningF("AndroidPermission: Failed to acquire %s permission: grant results array is empty.", perm);\
            }\
        });\
    } else {\
        LogWarningF("AndroidPermission: Failed to acquire %s permission.", perm);\
    }\
} else {\
    LogF("AndroidPermission: Already have %s permission.", perm);\
    __VA_ARGS__;\
}
#else
#define RUN_WITH_PERMISSION(perm, ...)
#endif
#pragma endregion

#pragma region Platform File

/** Use to filter files in directory. */
DECLARE_DELEGATE_RetVal_OneParam(bool, FPlatformFileFilter, const FPlatformFileInfo&);

/** Returns array of file metadata in the specified directory. */
TArray<FPlatformFileInfo> VESHARED_API GetFilesInDirectory(const FString Directory, const FPlatformFileFilter* FilterFunction = nullptr, bool bForceIncludeDirectories = true);

#pragma endregion

typedef TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> TCondensedJsonWriterRef;
typedef TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>> TCondensedJsonWriterFactory;

#pragma region Debounce

// Allows to debounce button clicks and similar events so they will be called only once per period of time instead of repeated multiple calls.
#define DEBOUNCE(bDebounceActive, fSeconds, ...) \
if (!bDebounceActive) { \
	bDebounceActive = true; \
	__VA_ARGS__; \
	FTimerHandle DebounceTimerHandle; \
	GetWorld()->GetTimerManager().SetTimer(DebounceTimerHandle, FTimerDelegate().CreateLambda([this](){\
		bDebounceActive = false; \
	}), fSeconds, false); \
}

// Debounce code in editor.
#ifdef UE_EDITOR
#if UE_EDITOR
#include "Editor/EditorEngine.h"
#define DEBOUNCE_EDITOR(bDebounceActive, fSeconds, ...) \
if (!bDebounceActive) {\
	bDebounceActive = true;\
	__VA_ARGS__;\
	FTimerHandle DebounceTimerHandle;\
	if (GEditor && GEditor->IsTimerManagerValid()) {\
	    GEditor->GetTimerManager().Get().SetTimer(DebounceTimerHandle, FTimerDelegate().CreateLambda([this]() {\
			bDebounceActive = false;\
		}), fSeconds, false);\
	} else {\
		LogWarningF("Failed to debounce, GEditor is invalid.");\
	}\
}
#else
#define DEBOUNCE_EDITOR(bDebounceActive, fSeconds, ...)
#endif
#else
#define DEBOUNCE_EDITOR(bDebounceActive, fSeconds, ...)
#endif

#pragma endregion

FString VESHARED_API TruncateString(const FString& InString, int MaxLength = 42, bool bAppendEllipsis = true);

enum class EFileMediaType : uint8 {
	Image,
	Video,
	Mesh,
	Audio,
	PDF,
	Youtube,
	Unknown,
};

FString VESHARED_API FileMediaTypeToString(const EFileMediaType& Type);
EFileMediaType VESHARED_API FileMediaTypeFromString(const FString& Source);

FString VESHARED_API GetMediaType(const FString& MimeType, const FString& Url);

// Older version: return (static_cast<float>(TotalLikes) / (TotalLikes + TotalDislikes) * 5 - 1) / 4 * 5;
#define RATING(Likes, Dislikes) (\
	(Likes + Dislikes > 0) ? \
		static_cast<float>(Likes) / static_cast<float>(Likes + Dislikes) * 4.f + 1.f \
	: \
		0.f \
	)

#define DEPRECATED 0
