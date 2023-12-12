// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.

#pragma once

#include "UIPageContentWidget.h"
#include "UIAuthPageWidget.generated.h"

/**
 * 
 */
UCLASS(HideDropdown)
class VEUI_API UUIAuthPageWidget final : public UUIPageContentWidget {
	GENERATED_BODY()

public:
#pragma region Buttons

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	class UUIButtonWidget* QuitButtonWidget;

#pragma endregion Buttons
	
	/** Sound to play on successful login. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Widget|Sound")
	USoundBase* LoginSuccessSound;
	
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

	UPROPERTY(EditAnywhere, Category="Page|SubPages", meta=(DisplayName="Auth Email Page"))
	TSubclassOf<UUIPageContentWidget> AuthEmailPageClass;

	UPROPERTY(EditAnywhere, Category="Page|SubPages", meta=(DisplayName="Auth Email Page"))
	TSubclassOf<class UUIAuthOAuthLoginPageWidget> OAuthPageClass;
	
	UPROPERTY(EditDefaultsOnly, Category="Game")
	float ExitDelay = 1.0f;

protected:
	virtual void NativeOnInitializedShared() override;
	void ReadOAuthProvidersConfig();
	void ShowOAuthButtonConfig();
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	bool bIsGoogleSupported;
	bool bIsLe7elSupported;
	bool bIsDiscordSupported;
	bool bIsEosSupported;
	
};
