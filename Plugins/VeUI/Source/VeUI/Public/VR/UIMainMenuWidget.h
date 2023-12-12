// Author: Egor A. Pristavka, Nikolay Bulatov
// Copyright (c) 2022 LE7EL AS. All right reserved.
#pragma once

#include "UIWidget.h"
#include "Components/UIButtonWidget.h"
#include "UIMainMenuWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAvatarButtonClicked);

/**
 * 
 */
UCLASS()
class VEUI_API UUIMainMenuWidget final : public UUIWidget {
	GENERATED_BODY()

	virtual void RegisterCallbacks() override;
	virtual void UnregisterCallbacks() override;

	FDelegateHandle OnHomeButtonClickedDelegateHandle;
	FDelegateHandle OnLogoutButtonClickedDelegateHandle;
	FDelegateHandle OnExitButtonClickedDelegateHandle;
	FDelegateHandle OnModsButtonClickedDelegateHandle;
	FDelegateHandle OnSpacesButtonClickedDelegateHandle;
	FDelegateHandle OnUsersButtonClickedDelegateHandle;
	FDelegateHandle OnSettingsButtonClickedDelegateHandle;
	FDelegateHandle OnServersButtonClickedDelegateHandle;
	FDelegateHandle OnAboutButtonClickedDelegateHandle;
	FDelegateHandle OnReturnMainMenuClickedDelegateHandle;
	FDelegateHandle OnObjectsButtonClickedDelegateHandle;
	FDelegateHandle OnAvatarButtonClickedDelegateHandle;

public:
	//virtual void NativeOnInitialized() override;


	UPROPERTY(BlueprintAssignable)
	FAvatarButtonClicked AvatarButtonClicked;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* HomeButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* UsersButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* SpacesButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* ServersButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* ModsButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* SettingsButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* LogoutButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* AboutButtonWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* ObjectsButtonWidget;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* AvatarButtonWidget;

	DECLARE_EVENT(UUIMainMenuWidget, FOnExitConfirmed);
	FOnExitConfirmed OnExitConfirmed;

	UPROPERTY(BlueprintReadOnly, Category="Widget|Content", meta=(BindWidget))
	UUIButtonWidget* ExitButtonWidget;

	FOnButtonClicked OnHomeButtonClicked;
	FOnButtonClicked OnModsButtonClicked;
	FOnButtonClicked OnSpacesButtonClicked;
	FOnButtonClicked OnUsersButtonClicked;
	FOnButtonClicked OnSettingsButtonClicked;
	FOnButtonClicked OnServersButtonClicked;
	FOnButtonClicked OnAboutButtonClicked;
	FOnButtonClicked OnReturnMainMenu;
	FOnButtonClicked OnArteonDLCButtonClicked;
	FOnButtonClicked OnAvatarButtonClicked;
};
