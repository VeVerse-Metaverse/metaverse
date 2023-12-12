// Author: Egor A. Pristavka. Copyright 2022-2023 VeVerse AS.
// Copyright (c) 2023 LE7EL AS. All right reserved.

// Copyright 2019-2020 Egor Pristavka. All Rights Reserved.

#pragma once

#include "Logging/LogMacros.h"

class VEINTERACTION_API FVeInteractionModule final : public FDefaultGameModuleImpl
{
public:

    /** IModuleInterface implementation */
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};

DECLARE_LOG_CATEGORY_EXTERN(LogInteraction, All, All);
DECLARE_LOG_CATEGORY_EXTERN(LogInteractionTest, All, All);
DECLARE_LOG_CATEGORY_EXTERN(LogVeInteraction, All, All);

#ifndef ECC_InteractionTraceChannel
#define ECC_InteractionTraceChannel ECC_GameTraceChannel1
#endif

// #ifndef FUNC_NAME
// #if defined(_MSC_VER)
// #define FUNC_NAME TEXT(__FUNCTION__)
// #elif defined(__PRETTY_FUNCTION__)
// #define FUNC_NAME __PRETTY_FUNCTION__
// #else
// #define FUNC_NAME ANSI_TO_TCHAR(__func__)
// #endif
// #endif

// #ifndef CHECK_RETURN
// #if PLATFORM_LINUX
// #define CHECK_RETURN(cond) if (!(cond)) { UE_LOG(LogInteraction, Error, TEXT("CHECK_RETURN(%s) Failed @ %s"), TEXT(#cond), FUNC_NAME); return; }
// #define CHECK_CONTINUE(cond) if (!(cond)) { UE_LOG(LogInteraction, Error, TEXT("CHECK_CONTINUE(%s) Failed @ %s"), TEXT(#cond), FUNC_NAME); continue; }
// #define CHECK_RETURN_VALUE(cond, val) if (!(cond)) { UE_LOG(LogInteraction, Error, TEXT("CHECK_RETURN(%s) Failed @ %s"), TEXT(#cond), FUNC_NAME); return val; }
// #else
// #define CHECK_RETURN(cond) if (!(cond)) { UE_LOG(LogInteraction, Error, TEXT("CHECK_RETURN(%s) Failed @ %s"), TEXT(#cond), FUNC_NAME); return; }
// #define CHECK_CONTINUE(cond) if (!(cond)) { UE_LOG(LogInteraction, Error, TEXT("CHECK_CONTINUE(%s) Failed @ %s"), TEXT(#cond), FUNC_NAME); continue; }
// #define CHECK_RETURN_VALUE(cond, val) if (!(cond)) { UE_LOG(LogInteraction, Error, TEXT("CHECK_RETURN(%s) Failed @ %s"), TEXT(#cond), FUNC_NAME); return val; }
// #endif
// #define IF_RETURN(cond, val) if (cond) { return val; }
// #define IF_RETURN_VALUE(cond, val) if (cond) { return val; }
// #endif

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
