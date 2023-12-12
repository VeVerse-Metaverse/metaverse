// Author: Egor Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "UIAnimationWidget.generated.h"

/**
 * 
 */
UCLASS(HideDropdown)
class VEUI_API UUIAnimationWidget : public UUserWidget {
	GENERATED_BODY()

	DECLARE_EVENT_OneParam(UUIPageContainerWidget, FUIAnimationFinished, UUIAnimationWidget* InAnimationWidget);

public:
	/** Container for child widget. */
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UBorder* ContainerWidget;

	/** Root widget fade in animation. */
	UPROPERTY(BlueprintReadOnly, Transient, Category="Widget|Animation", meta=(BindWidgetAnimOptional))
	UWidgetAnimation* FadeInAnimation;

	/** Root widget fade out animation (optional). Reversed fade in animation will be used if not defined. */
	UPROPERTY(BlueprintReadOnly, Transient, Category="Widget|Animation", meta=(BindWidgetAnimOptional))
	UWidgetAnimation* FadeOutAnimation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget|Animation")
	ESlateVisibility VisibilityBeforeFadeIn = ESlateVisibility::SelfHitTestInvisible;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget|Animation")
	ESlateVisibility VisibilityAfterFadeIn = ESlateVisibility::SelfHitTestInvisible;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget|Animation")
	ESlateVisibility VisibilityBeforeFadeOut = ESlateVisibility::SelfHitTestInvisible;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget|Animation")
	ESlateVisibility VisibilityAfterFadeOut = ESlateVisibility::Collapsed;

	/** Sound to play on fade in. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget|Sound")
	USoundBase* FadeInSound;

	/** Sound to play on fade out. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget|Sound")
	USoundBase* FadeOutSound;

	FUIAnimationFinished& GetOnShowStarted() { return OnShowStarted; }
	FUIAnimationFinished& GetOnShowFinished() { return OnShowFinished; }
	FUIAnimationFinished& GetOnHideStarted() { return OnHideStarted; }
	FUIAnimationFinished& GetOnHideFinished() { return OnHideFinished; }

	/** Plays the fade in animation. */
	virtual UUMGSequencePlayer* Show(bool bPlayAnimation = true);

	/** Plays the fade out animation. */
	virtual UUMGSequencePlayer* Hide();

	void SetContentWidget(class UUIWidgetBase* InContentWidget);

	template <class T>
	T* GetContentWidget() const;

protected:
	/** Cached player for the current fade animation. */
	UPROPERTY()
	UUMGSequencePlayer* FadeAnimationSequencePlayer;

	/** Tracks the current fade animation complete event. */
	FDelegateHandle FadeAnimationFinishedDelegateHandle;

	virtual void NativeOnShowStarted();
	virtual void NativeOnShowFinished(bool bInHasCompleted);
	virtual void NativeOnHideStarted();
	virtual void NativeOnHideFinished(bool bInHasCompleted);

private:
	FUIAnimationFinished OnShowStarted;
	FUIAnimationFinished OnShowFinished;
	FUIAnimationFinished OnHideStarted;
	FUIAnimationFinished OnHideFinished;
};

template <class T>
T* UUIAnimationWidget::GetContentWidget() const {
	if (IsValid(ContainerWidget)) {
		return Cast<T>(ContainerWidget->GetChildAt(0));
	}
	return nullptr;
}
