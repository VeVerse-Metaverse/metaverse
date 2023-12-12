// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "UIPageContentWidget.h"
#include "UIAuthResetPageWidget.generated.h"

/**
 * 
 */
UCLASS(HideDropdown)
class VEUI_API UUIAuthResetPageWidget final : public UUIPageContentWidget {
	GENERATED_BODY()

public:

#pragma region Widgets
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	class UUIFormInputWidget* EmailInputWidget;

#pragma endregion Widgets

#pragma region Buttons

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	class UUIButtonWidget* ResetPasswordButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	class UUIButtonWidget* LoginButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidgetOptional))
	class UUIButtonWidget* RegisterButtonWidget;

#pragma endregion Buttons

	UPROPERTY(EditAnywhere, Category="Page|SubPages", meta=(DisplayName="Register Page"))
	TSubclassOf<UUIPageContentWidget> RegisterPageClass;

protected:
	virtual void NativeOnInitializedShared() override;

	void Reset();
	void ResetPasswordRequest();
	void ResetPasswordResponse(const bool bInOk, const bool bInSuccessful, const FString& Error);
};
