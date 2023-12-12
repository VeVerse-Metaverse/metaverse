// 

#include "VeGameUserSettings.h"

#if WITH_EDITOR
static TAutoConsoleVariable<bool> CVarApplyFrameRateSettingsInPIE(TEXT("Ve.Settings.ApplyFrameRateSettingsInPIE"),
	false,
	TEXT("Should we apply frame rate settings in PIE?"),
	ECVF_Default);

#endif

// Combines two limits, always taking the minimum of the two (with special handling for values of <= 0 meaning unlimited)
float CombineFrameRateLimits(float Limit1, float Limit2) {
	if (Limit1 <= 0.0f) {
		return Limit2;
	} else if (Limit2 <= 0.0f) {
		return Limit1;
	} else {
		return FMath::Min(Limit1, Limit2);
	}
}

UVeGameUserSettings::UVeGameUserSettings() {
	if (!HasAnyFlags(RF_ClassDefaultObject) && FSlateApplication::IsInitialized()) {
		OnApplicationActivationStateChangedHandle = FSlateApplication::Get().OnApplicationActivationStateChanged().AddUObject(this, &ThisClass::OnAppActivationStateChanged);
	}

	SetToDefaults();
}

UVeGameUserSettings* UVeGameUserSettings::Get() {
	return GEngine ? CastChecked<UVeGameUserSettings>(GEngine->GetGameUserSettings()) : nullptr;
}

void UVeGameUserSettings::SetToDefaults() {
	Super::SetToDefaults();

	FrameRateLimit_InMenu = 144.0f;
	FrameRateLimit_WhenBackgrounded = 5.0f;
	FrameRateLimit_OnBattery = 60.0f;
}

void UVeGameUserSettings::BeginDestroy() {
	if (FSlateApplication::IsInitialized()) {
		FSlateApplication::Get().OnApplicationActivationStateChanged().Remove(OnApplicationActivationStateChangedHandle);
	}

	Super::BeginDestroy();
}

float UVeGameUserSettings::GetEffectiveFrameRateLimit() {
	// const ULyraPlatformSpecificRenderingSettings* PlatformSettings = ULyraPlatformSpecificRenderingSettings::Get();

#if WITH_EDITOR
	if (GIsEditor && !CVarApplyFrameRateSettingsInPIE.GetValueOnGameThread()) {
		return Super::GetEffectiveFrameRateLimit();
	}
#endif

	// if (PlatformSettings->FramePacingMode == ELyraFramePacingMode::ConsoleStyle)
	// {
	// 	return 0.0f;
	// }

	float EffectiveFrameRateLimit = Super::GetEffectiveFrameRateLimit();

	// if (ShouldUseFrontendPerformanceSettings())
	// {
	// 	EffectiveFrameRateLimit = CombineFrameRateLimits(EffectiveFrameRateLimit, FrameRateLimit_InMenu);
	// }
	//
	// if (PlatformSettings->FramePacingMode == ELyraFramePacingMode::DesktopStyle)
	// {
	if (FPlatformMisc::IsRunningOnBattery()) {
		EffectiveFrameRateLimit = CombineFrameRateLimits(EffectiveFrameRateLimit, FrameRateLimit_OnBattery);
	}

	if (FSlateApplication::IsInitialized() && !FSlateApplication::Get().IsActive()) {
		EffectiveFrameRateLimit = CombineFrameRateLimits(EffectiveFrameRateLimit, FrameRateLimit_WhenBackgrounded);
	}
	// }

	return EffectiveFrameRateLimit;
}

void UVeGameUserSettings::OnAppActivationStateChanged(bool bIsActive) {
	// We might want to adjust the frame rate when the app loses/gains focus on multi-window platforms
	UpdateEffectiveFrameRateLimit();
}

float UVeGameUserSettings::GetFrameRateLimit_OnBattery() const {
	return FrameRateLimit_OnBattery;
}

void UVeGameUserSettings::SetFrameRateLimit_OnBattery(float NewLimitFPS) {
	FrameRateLimit_OnBattery = NewLimitFPS;
	UpdateEffectiveFrameRateLimit();
}

float UVeGameUserSettings::GetFrameRateLimit_InMenu() const {
	return FrameRateLimit_InMenu;
}

void UVeGameUserSettings::SetFrameRateLimit_InMenu(float NewLimitFPS) {
	FrameRateLimit_InMenu = NewLimitFPS;
	UpdateEffectiveFrameRateLimit();
}

float UVeGameUserSettings::GetFrameRateLimit_WhenBackgrounded() const {
	return FrameRateLimit_WhenBackgrounded;
}

void UVeGameUserSettings::SetFrameRateLimit_WhenBackgrounded(float NewLimitFPS) {
	FrameRateLimit_WhenBackgrounded = NewLimitFPS;
	UpdateEffectiveFrameRateLimit();
}

float UVeGameUserSettings::GetFrameRateLimit_Always() const {
	return GetFrameRateLimit();
}

void UVeGameUserSettings::SetFrameRateLimit_Always(float NewLimitFPS) {
	SetFrameRateLimit(NewLimitFPS);
	UpdateEffectiveFrameRateLimit();
}

void UVeGameUserSettings::UpdateEffectiveFrameRateLimit() {
	if (!IsRunningDedicatedServer()) {
		SetFrameRateLimitCVar(GetEffectiveFrameRateLimit());
	}
}
