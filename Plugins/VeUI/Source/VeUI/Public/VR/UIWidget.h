// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "ApiSubsystem.h"
#include "ICallbackInterface.h"
#include "UINotificationData.h"
#include "Blueprint/UserWidget.h"
#include "UIWidget.generated.h"


// #define ASSIGN_WIDGET_TEXTURE(Widget, Result) \
// 	if (IsValid(Widget)) { \
// if (UTexture2D* Texture2D = Result.AsTexture2D()) { \
// 	Widget->SetBrushFromTexture(Texture2D); \
// } else if (UAnimatedTexture2D* AnimatedTexture2D = Result.AsAnimatedTexture2D()) { \
// 	Widget->SetBrushResourceObject(AnimatedTexture2D); \
// } \
// const FVector2D Size = { \
// 	Result.Texture->GetSurfaceWidth(), \
// 	Result.Texture->GetSurfaceHeight() \
// }; \
// Widget->SetBrushSize(Size); }

/**
 * Base animated widget.
 */
UCLASS()
class VEUI_API UUIWidget : public UUserWidget, public ICallbackInterface {
	GENERATED_BODY()

protected:
	virtual bool Initialize() override;
	/** Shared initialization code called both with SDK and Game OnInitialized events. */
	virtual void NativeOnInitializedShared();
	/** Called only in a game during widget initialization. */
	virtual void NativeOnInitialized() override;
	/** Called only in SDK during widget initialization. */
	virtual void NativeOnInitializedSdk();

	bool IsSdk() const;

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	virtual void RegisterCallbacks() override;
	virtual void UnregisterCallbacks() override;

	virtual void OnNavigate();

public:
	static void AddNotification(FUINotificationData NotificationData);

	DECLARE_EVENT_TwoParams(UUIWidget, FOnMouseEnter, const FGeometry&, const FPointerEvent&);

	FOnMouseEnter OnMouseEnter;

	DECLARE_EVENT_OneParam(UUIWidget, FOnMouseLeave, const FPointerEvent&);

	FOnMouseLeave OnMouseLeave;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UPanelWidget* RootWidget;

	/** Returns the root widget top-level animations are applied to. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Widget|Content")
	UPanelWidget* GetRootWidget();

	/** Root widget fade in animation. */
	UPROPERTY(BlueprintReadOnly, Transient, Category="Widget|Animation", meta=(BindWidgetAnimOptional))
	UWidgetAnimation* FadeInAnimation;

	/** Root widget fade out animation (optional). Reversed fade in animation will be used if not defined. */
	UPROPERTY(BlueprintReadOnly, Transient, Category="Widget|Animation", meta=(BindWidgetAnimOptional))
	UWidgetAnimation* FadeOutAnimation;

	/** Sound to play on fade in. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget|Sound")
	USoundBase* FadeInSound;

	/** Sound to play on fade out. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget|Sound")
	USoundBase* FadeOutSound;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget|Animation")
	ESlateVisibility VisibilityBeforeFadeIn = ESlateVisibility::SelfHitTestInvisible;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget|Animation")
	ESlateVisibility VisibilityAfterFadeIn = ESlateVisibility::SelfHitTestInvisible;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget|Animation")
	ESlateVisibility VisibilityBeforeFadeOut = ESlateVisibility::SelfHitTestInvisible;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget|Animation")
	ESlateVisibility VisibilityAfterFadeOut = ESlateVisibility::Collapsed;

	UFUNCTION(BlueprintGetter, Category="Widget|Content")
	class AVeHUD* GetHUD() const;

	/** Resets properties state to initial. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Widget|Content")
	void ResetState();

	/** Resets animation state to initial. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Widget|Animation")
	void ResetAnimationState();

	bool bIsHidden = false;
	void SetIsHidden(bool bInIsHidden);
	bool GetIsHidden() const;

	/** Cached player for the current fade animation. */
	UPROPERTY()
	UUMGSequencePlayer* FadeAnimationSequencePlayer;

	/** Tracks the current fade animation complete event. */
	FDelegateHandle FadeAnimationCompleteDelegateHandle;

	DECLARE_EVENT_TwoParams(UUIWidget, FUIAnimationFinished, UUMGSequencePlayer* /*InSequencePlayer*/, bool /*bInHasCompleted*/);

	FUIAnimationFinished OnFadeInAnimationFinished;
	FUIAnimationFinished OnFadeOutAnimationFinished;

	/** Plays the fade in animation. */
	virtual UUMGSequencePlayer* Show(bool bAnimate = true, bool bPlaySound = true);
	void OnShowAnimationFinishedCallback(UUMGSequencePlayer& InSequencePlayer);

	/** Plays the fade out animation. */
	virtual UUMGSequencePlayer* Hide(bool bAnimate = true, bool bPlaySound = true);
	void OnHideAnimationFinishedCallback(UUMGSequencePlayer& InSequencePlayer);

	/** Plays the fade out animation and destroys the widget by removing it from the parent. */
	virtual void HideAndDestroy(bool bAnimate = true, bool bPlaySound = true);

	FText GetLocalizedErrorText(const FString& ErrorString) const;

	virtual void SetWidgetFocus() const;
	virtual void SetWidgetKeyboardFocus() const;
	virtual bool ReceiveKeyEvent(const FKeyEvent& KeyEvent);
	virtual bool ReceiveCharacterInput(const FString& InCharacter);

	UApiSubsystem* GetApiSubsystem() const;
};
