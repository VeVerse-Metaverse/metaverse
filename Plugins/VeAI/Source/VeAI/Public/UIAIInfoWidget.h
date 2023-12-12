// Authors: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022-2023 VeVerse AS. All rights reserved.
// Copyright (c) 2023 LE7EL AS. All rights reserved.
#pragma once

#include "UIWidgetBase.h"

#include "VeUserMetadata.h"
#include "UIUserVoiceWidget.h"
#include "UIAIInfoWidget.generated.h"

class UBorder;
class AVeAiPlayerState;
class AVePlayerStateBase;
class UUILoadingWidget;
class UUIUserVoiceWidget;
class UImage;
class UTextBlock;

/**
 * Widget to be placed over AI pawns to identify users.
 */
UCLASS()
class VEAI_API UUIAIInfoWidget final : public UUIWidgetBase {
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	TWeakObjectPtr<AVeAiPlayerState> PlayerState;

public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void SetOpacity(float InOpacity);

	/** Initialize this widget with the player state. */
	UFUNCTION(BlueprintCallable)
	void SetAIPlayerState(AVeAiPlayerState* InPlayerState);

	UFUNCTION()
	void OnUserMetadataUpdated(const FVeUserMetadata& InMetadata);

	/**
	 * @brief Called when the person starts saying something to display subtitles and the voice icon.
	 */
	UFUNCTION(BlueprintCallable)
	void OnSay(const FText& InText);

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	TObjectPtr<UBorder> RootWidget = nullptr;

	/**
	 * @brief User name.
	 */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	TObjectPtr<UTextBlock> NameTextWidget = nullptr;

	/**
	 * @brief User title.
	 */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> TitleTextWidget = nullptr;

	/**
	 * @brief User voice status.
	 */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	TObjectPtr<UUIUserVoiceWidget> UserVoiceWidget = nullptr;

	/**
	 * @brief Subtitles.
	 */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> SubtitleTextWidget = nullptr;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	TObjectPtr<UWidget> SubtitleContainerWidget = nullptr;

	/**
	 * @brief Current subtitle text to display.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Widget|Content")
	FText SubtitleText = {};

	/**
	 * @brief How long it takes to read a single subtitle symbol (used to calculate hide delay).
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Widget|Content")
	float TypingCharactersPerSecond = 0.1f;

	/**
	 * @brief How long it takes to read a single subtitle symbol (used to calculate hide delay).
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Widget|Content")
	float ResetDelaySecondsPerCharacter = 0.15f;

private:
	/**
	 * @brief Timer handle used to type subtitles character by character.
	 */
	FTimerHandle SayTypingTimerHandle;

	/**
	 * @brief Timer handle used to hide subtitles after some time enough to read subtitles.
	 */
	FTimerHandle SayResetTimerHandle;
};
