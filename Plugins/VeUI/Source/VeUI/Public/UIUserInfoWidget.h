// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "Blueprint/UserWidget.h"
#include "VeUserMetadata.h"
#include "UIUserVoiceWidget.h"
#include "UIUserInfoWidget.generated.h"

class AVePlayerStateBase;
class UUILoadingWidget;
class UUIUserVoiceWidget;
class UImage;
class UTextBlock;

/**
 * Widget to be placed over user pawns to identify users.
 */
UCLASS()
class VEUI_API UUIUserInfoWidget final : public UUserWidget {
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	TWeakObjectPtr<AVePlayerStateBase> PlayerState;

public:
	// virtual void NativeConstruct() override;
	// virtual void NativeDestruct() override;

	/** Initialize this widget with the player state. */
	UFUNCTION(BlueprintCallable)
	void SetPlayerState(AVePlayerStateBase* InPlayerState);

	UFUNCTION()
	void OnUserMetadataUpdated(const FVeUserMetadata& InMetadata);

	UFUNCTION()
	void OnIsSpeakingChanged(bool bInIsSpeaking) const;

	/** Avatar image. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	class UUIImageWidget* AvatarWidget;

	/** User name. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UTextBlock* NameTextWidget;

	/** User title. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UTextBlock* TitleTextWidget;

	/** User rating. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UTextBlock* RatingTextWidget;

	/** User level. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UTextBlock* LevelTextWidget;

	/** User voice status. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	UUIUserVoiceWidget* UserVoiceWidget;
};
