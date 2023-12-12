// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "UIWidgetBase.h"

#include "UIUserVoiceWidget.generated.h"

class UImage;

enum class VEUI_API EVoiceWidgetStatus : uint8 {
	Inactive = 0,
	ActiveLow,
	Active,
	Muted,
};

/**
 * Widget to be placed over user pawns to identify users.
 */
UCLASS()
class VEUI_API UUIUserVoiceWidget final : public UUIWidgetBase
 {
	GENERATED_BODY()

public:
	virtual void RegisterCallbacks() override;
	virtual void UnregisterCallbacks() override;
	virtual void NativeConstruct() override;

	/** Updates the voice status icon. */
	void SetVoiceStatus(const bool InVoiceActive) const;

	/** Updates the voice status icon. */
	void SetVoiceStatus(const EVoiceWidgetStatus InVoiceStatus) const;

	/** Widget enabled when the user speaks loud. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UImage* VoiceActiveWidget;

	/** Widget enabled when the user speaks. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UImage* VoiceActiveLowWidget;

	/** Widget enabled when the user is silent. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UImage* VoiceInactiveWidget;

	/** Widget enabled when the user is muted. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UImage* VoiceMutedWidget;

};
