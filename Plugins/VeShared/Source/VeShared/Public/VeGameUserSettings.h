// 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "VeGameUserSettings.generated.h"

/**
 * 
 */
UCLASS()
class VESHARED_API UVeGameUserSettings : public UGameUserSettings {
	GENERATED_BODY()

public:
	UVeGameUserSettings();

	static UVeGameUserSettings* Get();

	virtual void BeginDestroy() override;
	virtual void SetToDefaults() override;
	virtual float GetEffectiveFrameRateLimit() override;

	UPROPERTY(Config)
	FGuid RegionId = FGuid(TEXT("e77861a3-6b24-4772-9245-ea1ca91e46e5"));

private:
	void OnAppActivationStateChanged(bool bIsActive);

	FDelegateHandle OnApplicationActivationStateChangedHandle;

#pragma region Display

public:
	UFUNCTION()
	float GetFrameRateLimit_OnBattery() const;
	UFUNCTION()
	void SetFrameRateLimit_OnBattery(float NewLimitFPS);

	UFUNCTION()
	float GetFrameRateLimit_InMenu() const;
	UFUNCTION()
	void SetFrameRateLimit_InMenu(float NewLimitFPS);

	UFUNCTION()
	float GetFrameRateLimit_WhenBackgrounded() const;
	UFUNCTION()
	void SetFrameRateLimit_WhenBackgrounded(float NewLimitFPS);

	UFUNCTION()
	float GetFrameRateLimit_Always() const;
	UFUNCTION()
	void SetFrameRateLimit_Always(float NewLimitFPS);

protected:
	void UpdateEffectiveFrameRateLimit();

private:
	UPROPERTY(Config)
	float FrameRateLimit_OnBattery;
	UPROPERTY(Config)
	float FrameRateLimit_InMenu;
	UPROPERTY(Config)
	float FrameRateLimit_WhenBackgrounded;

#pragma endregion Display

};
