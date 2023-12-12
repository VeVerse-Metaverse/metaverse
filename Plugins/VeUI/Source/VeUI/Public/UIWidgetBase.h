// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once


#include "Blueprint/UserWidget.h"
#include "UIWidgetBase.generated.h"

struct FUINotificationData;

#define ASSIGN_WIDGET_TEXTURE(Widget, Result) \
if (IsValid(Widget)) { \
	if (Result.Texture.IsValid()) { \
	if (UTexture2D* Texture2D = Result.AsTexture2D()) { \
		Widget->SetBrushFromTexture(Texture2D); \
	} else if (UAnimatedTexture2D* AnimatedTexture2D = Result.AsAnimatedTexture2D()) { \
		Widget->SetBrushResourceObject(AnimatedTexture2D); \
	} \
	const FVector2D Size = { \
	Result.Texture->GetSurfaceWidth(), \
	Result.Texture->GetSurfaceHeight() \
	}; \
	Widget->SetDesiredSizeOverride(Size); } }


/**
 * Base animated widget.
 */
UCLASS()
class VEUI_API UUIWidgetBase : public UUserWidget {
	GENERATED_BODY()

	DECLARE_EVENT(UUIWidgetBase, FWidgetEvent);

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget|Animation")
	ESlateVisibility VisibilityAfterFadeIn = ESlateVisibility::SelfHitTestInvisible;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget|Animation")
	ESlateVisibility VisibilityAfterFadeOut = ESlateVisibility::Collapsed;

	/** Override initialization code to support custom SDK logic. */
	virtual bool Initialize() override;
	/** Called only in a game during widget initialization. */
	virtual void NativeOnInitialized() override;
	/** Called only in SDK during widget initialization. */
	virtual void NativeOnInitializedSdk();
	/** Shared initialization code called both with SDK and Game OnInitialized events. */
	virtual void NativeOnInitializedShared();

	/** Override to support tick and animations in SDK widgets */
	virtual UWorld* GetWorld() const override;

	class AVeHUD* GetHUD() const;

	virtual void Show();
	virtual void Hide();

	FWidgetEvent& GetOnMouseEnter() { return OnMouseEnter; }
	FWidgetEvent& GetOnMouseLeave() { return OnMouseLeave; }

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	virtual void RegisterCallbacks() { }
	virtual void UnregisterCallbacks() { }

	FTimerManager* GetTimerManager() const;

	template <typename WidgetT = UUserWidget>
	TObjectPtr<WidgetT> CreateWidget(TSubclassOf<UUserWidget> UserWidgetClass = WidgetT::StaticClass(), FName WidgetName = NAME_None);

private:
	FWidgetEvent OnMouseEnter;
	FWidgetEvent OnMouseLeave;

	/** In the SDK mode NativeInitialize is not called, so this flag used to detect this fact and do late initialization of the widget during Initialize call. */
	bool bNativeInitialized = false;

//------------------------------------------------------------------------------

public:
	[[deprecated]]
	virtual void SetWidgetFocus() const { }
	[[deprecated]]
	virtual void SetWidgetKeyboardFocus() const { }
	[[deprecated]]
	virtual bool ReceiveKeyEvent(const FKeyEvent& KeyEvent) { return false; }
	[[deprecated]]
	virtual bool ReceiveCharacterInput(const FString& InCharacter) { return false; }

	[[deprecated]]
	virtual void OnNavigate() { }

};


template <typename WidgetT>
TObjectPtr<WidgetT> UUIWidgetBase::CreateWidget(TSubclassOf<UUserWidget> UserWidgetClass, FName WidgetName) {
	return ::CreateWidget<WidgetT>(WidgetTree.Get(), UserWidgetClass, WidgetName);
}
