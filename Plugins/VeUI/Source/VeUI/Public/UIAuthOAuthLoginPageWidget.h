// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#pragma once

#include "UIPageContentWidget.h"
#include "UIAuthOAuthLoginPageWidget.generated.h"

/**
 * 
 */
UCLASS(HideDropdown)
class VEUI_API UUIAuthOAuthLoginPageWidget final : public UUIPageContentWidget {
	GENERATED_BODY()

public:
#pragma region Widgets

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UTextBlock* HeaderText;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIButtonWidget* OpenButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIButtonWidget* LoginOptionsButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIButtonWidget* QuitButtonWidget;

#pragma endregion Widgets

	void SetHeaderText(const FText& InText);
	void SetProvider(const FString& InProvider);

protected:
	virtual void NativeOnInitializedShared() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	FString Provider;

	UPROPERTY(EditDefaultsOnly, Category="Game")
	float ExitDelay = 1.0f;
};
