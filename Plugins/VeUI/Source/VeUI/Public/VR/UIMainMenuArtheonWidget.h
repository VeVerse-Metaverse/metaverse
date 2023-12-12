// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "UIWidget.h"
#include "Components/UIButtonWidget.h"
#include "UIMainMenuArtheonWidget.generated.h"

/**
 * 
 */
UCLASS()
class VEUI_API UUIMainMenuArtheonWidget final : public UUIWidget {
	GENERATED_BODY()

	virtual void RegisterCallbacks() override;
	virtual void UnregisterCallbacks() override;

	FDelegateHandle OnLogoutButtonClickedDelegateHandle;
	FDelegateHandle OnExitButtonClickedDelegateHandle;
	FDelegateHandle OnSpacesButtonClickedDelegateHandle;
	FDelegateHandle OnUsersButtonClickedDelegateHandle;
	FDelegateHandle OnReturnMainMenuClickedDelegateHandle;
	FDelegateHandle OnObjectsButtonClickedDelegateHandle;
	FDelegateHandle OnOpenSeaNFTsButtonClickedDelegateHandle;
	FDelegateHandle OnBackToLe7elButtonClickedDelegateHandle;

public:
	//virtual void NativeOnInitialized() override;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* HomeButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* SpacesButtonWidget;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* LogoutButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* ObjectsButtonWidget;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* OpenSeaNFTsButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* BackToLe7elButtonWidget;

	DECLARE_EVENT(UUIMainMenuWidget, FOnExitConfirmed);
	FOnExitConfirmed OnExitConfirmed;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* ExitButtonWidget;
	
	FOnButtonClicked OnSpacesButtonClicked;
	FOnButtonClicked OnUsersButtonClicked;
	FOnButtonClicked OnReturnMainMenu;
	FOnButtonClicked OnObjectsButtonClicked;
	FOnButtonClicked OnOpenSeaNFTsButtonClicked;
	FOnButtonClicked OnBackToLe7elButtonClicked;
};
