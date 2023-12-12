// Author: Egor Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "UIPageContentWidget.h"
#include "UISocialLoginPageWidget.generated.h"

class UUIButtonWidget;

/**
 * 
 */
UCLASS(HideDropdown)
class VEUI_API UUISocialLoginPageWidget final : public UUIPageContentWidget {
	GENERATED_BODY()

public:
#pragma region Widgets
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* GoogleLoginButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* LevelLoginButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* DiscordLoginButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* EpicLoginButtonWidget;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* EmailLoginButtonWidget;
#pragma endregion Widgets

	UPROPERTY(EditAnywhere, Category="Page|SubPages", meta=(DisplayName="Auth Email Page"))
	TSubclassOf<UUIPageContentWidget> AuthEmailPageClass;
protected:
	virtual void NativeOnInitializedShared() override;
};
